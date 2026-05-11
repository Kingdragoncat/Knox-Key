/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body — Knox Key stm32l433 target
 ******************************************************************************
 *
 * Knox Key hardware:
 *   MCU:       stm32l433CBU6 (Cortex-M0, 48 MHz, 128KB flash, 16KB RAM)
 *   Crystal:   8 MHz HSE
 *   USB:       USB 2.0 Full-Speed (USB FS peripheral with integrated PHY)
 *   Security:  ATECC608B via I2C (PB6/PB7)
 *   LED:       PA0 (status LED, active high)
 *   Button:    PA1 (user presence button)
 *   LDO:      MCP1700-3.3V
 *
 ******************************************************************************
 */

#include "main.h"
#include "app.h"

/* Private variables ---------------------------------------------------------*/
volatile Button_Event CurrentButtonEvent = BUTTON_EVENT_NONE;
static uint32_t button_press_start = 0;
static uint32_t last_button_release = 0;
static uint8_t click_count = 0;
static bool button_is_pressed = false;
TIM_HandleTypeDef hTIM2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);

int Debug_UART_Get_Byte(void) {
	return -1;
}

static Status_LED_Mode current_led_mode = STATUS_LED_MODE_OFF;
static uint32_t led_animation_start = 0;
static Status_LED_Mode previous_led_mode = STATUS_LED_MODE_OFF;

void Status_LED_Set_Mode(Status_LED_Mode mode) {
    if (mode == STATUS_LED_MODE_SUCCESS || mode == STATUS_LED_MODE_ERROR || mode == STATUS_LED_MODE_WINK) {
        if (current_led_mode != mode) {
            previous_led_mode = current_led_mode;
        }
        led_animation_start = HAL_GetTick();
    }
	current_led_mode = mode;
	if (mode == STATUS_LED_MODE_OFF) {
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin | LED_G_Pin | LED_B_Pin, GPIO_PIN_RESET);
	} else if (mode == STATUS_LED_MODE_ON) {
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin | LED_G_Pin | LED_B_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_G_Pin, GPIO_PIN_SET); /* Solid Green */
	}
}

void Status_LED_Update(void) {
	uint32_t tick = HAL_GetTick();
	if (current_led_mode == STATUS_LED_MODE_BLINKING_NORMAL) {
		if ((tick % 1000) < 500) {
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin | LED_G_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_B_Pin, GPIO_PIN_SET); /* Blue blink */
		} else {
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);
		}
	} else if (current_led_mode == STATUS_LED_MODE_BLINKING_SPECIAL) {
		if ((tick % 100) < 50) {
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin | LED_G_Pin, GPIO_PIN_SET); /* Yellow blink */
		} else {
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin | LED_G_Pin, GPIO_PIN_RESET);
		}
	} else if (current_led_mode == STATUS_LED_MODE_SUCCESS) {
        if (tick - led_animation_start < 1000) {
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin | LED_B_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_G_Pin, GPIO_PIN_SET); /* Solid Green */
        } else {
            Status_LED_Set_Mode(previous_led_mode);
        }
    } else if (current_led_mode == STATUS_LED_MODE_ERROR) {
        if (tick - led_animation_start < 1000) {
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_G_Pin | LED_B_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin, GPIO_PIN_SET); /* Solid Red */
        } else {
            Status_LED_Set_Mode(previous_led_mode);
        }
    } else if (current_led_mode == STATUS_LED_MODE_WINK) {
        if (tick - led_animation_start < 2000) {
            if ((tick % 200) < 100) {
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin | LED_G_Pin | LED_B_Pin, GPIO_PIN_SET); /* White wink */
            } else {
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin | LED_G_Pin | LED_B_Pin, GPIO_PIN_RESET);
            }
        } else {
            Status_LED_Set_Mode(previous_led_mode);
        }
    }
}

int main(void) {
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_I2C1_Init();

	app_run();
}

/**
 * @brief System Clock Configuration
 *        HSE 8 MHz → PLL → 48 MHz SYSCLK
 *        HSI48 → USB 48 MHz clock
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11; /* 48 MHz */
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief TIM2 Initialization Function — used for LED PWM on PA0 (TIM2_CH1)
 */


I2C_HandleTypeDef hi2c1;

static void MX_I2C1_Init(void) {
	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x10909CEC; /* 100kHz at 48MHz SYSCLK (from CubeMX) */
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
		Error_Handler();
	}
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(hi2c->Instance==I2C1) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**I2C1 GPIO Configuration
		PB6     ------> I2C1_SCL
		PB7     ------> I2C1_SDA
		*/
		GPIO_InitStruct.Pin = ATECC_SCL_Pin|ATECC_SDA_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		__HAL_RCC_I2C1_CLK_ENABLE();
	}
}

/**
 * @brief GPIO Initialization Function
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	/* LED pins */
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_R_Pin | LED_G_Pin | LED_B_Pin, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = LED_R_Pin | LED_G_Pin | LED_B_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

	/* User button — PA1, external interrupt on rising edge */
	GPIO_InitStruct.Pin = USER_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(USER_BTN_GPIO_Port, &GPIO_InitStruct);

	/* Enable EXTI interrupt for the button */
	HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

/**
 * @brief  This function is executed in case of error occurrence.
 */
void Error_Handler(void) {
	__disable_irq();
	/* Fast-blink LED to signal error */
	/* Since interrupts are disabled, just toggle manually */
	while (1) {
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
	(void)file;
	(void)line;
}
#endif

void Button_Update(void) {
    uint32_t tick = HAL_GetTick();
    bool raw_pressed = (HAL_GPIO_ReadPin(USER_BTN_GPIO_Port, USER_BTN_Pin) == GPIO_PIN_SET);

    if (raw_pressed && !button_is_pressed) {
        if (tick - last_button_release > 20) { // Debounce 20ms
            button_is_pressed = true;
            button_press_start = tick;
        }
    } else if (!raw_pressed && button_is_pressed) {
        if (tick - button_press_start > 20) { // Debounce 20ms
            button_is_pressed = false;
            last_button_release = tick;
            uint32_t duration = tick - button_press_start;
            
            if (duration >= 1000) {
                // Long press
                CurrentButtonEvent = BUTTON_EVENT_LONG_PRESS;
                click_count = 0;
            } else {
                click_count++;
            }
        }
    }

    if (!button_is_pressed && click_count > 0 && (tick - last_button_release > 300)) {
        if (click_count == 1) {
            CurrentButtonEvent = BUTTON_EVENT_SINGLE_CLICK;
        } else if (click_count >= 2) {
            CurrentButtonEvent = BUTTON_EVENT_DOUBLE_CLICK;
        }
        click_count = 0;
    }
}
