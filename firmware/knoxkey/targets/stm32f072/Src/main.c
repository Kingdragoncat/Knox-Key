/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body — Knox Key STM32F072 target
 ******************************************************************************
 *
 * Knox Key hardware:
 *   MCU:       STM32F072CBU6 (Cortex-M0, 48 MHz, 128KB flash, 16KB RAM)
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
__IO uint32_t ButtonState = BUTTON_RELEASED;
TIM_HandleTypeDef htim3;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);

/* Debug UART stub — Knox Key has no dedicated debug UART in the minimal design.
   When needed, SWO or semihosting can be used instead. */
int Debug_UART_Get_Byte(void) {
	return -1;
}

void Status_LED_Set_Mode(Status_LED_Mode mode) {
	switch (mode) {
		case STATUS_LED_MODE_OFF:
			/* Force output low */
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
			break;
		case STATUS_LED_MODE_ON:
			/* Force output high */
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
			break;
		case STATUS_LED_MODE_BLINKING_NORMAL:
			/* ~1 Hz blink via PWM (50% duty, period = 48M / 48000 = 1000 ticks at prescaler giving 1kHz) */
			htim3.Instance->ARR = 999;
			htim3.Instance->CCR1 = 500;
			htim3.Instance->PSC = 47999; /* 48 MHz / 48000 = 1 kHz tick */
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
			break;
		case STATUS_LED_MODE_BLINKING_SPECIAL:
			/* Fast blink ~10 Hz */
			htim3.Instance->ARR = 99;
			htim3.Instance->CCR1 = 50;
			htim3.Instance->PSC = 47999; /* 48 MHz / 48000 = 1 kHz tick */
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
			break;
	}
}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM3_Init();

	/* Infinite loop — app_run never returns */
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

	/* Enable HSE (8 MHz crystal) and PLL (8 MHz / 1 * 6 = 48 MHz) */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI48;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/* SYSCLK = PLLCLK = 48 MHz, HCLK = 48 MHz, PCLK1 = 48 MHz */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}

	/* USB clock from HSI48 (dedicated 48 MHz oscillator) */
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief TIM3 Initialization Function — used for LED PWM on PA0 (TIM3_CH1)
 */
static void MX_TIM3_Init(void) {
	TIM_OC_InitTypeDef sConfigOC = {0};

	__HAL_RCC_TIM3_CLK_ENABLE();

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 47999; /* 48 MHz / 48000 = 1 kHz */
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 999;      /* 1 kHz / 1000 = 1 Hz */
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
		Error_Handler();
	}

	/* TIM3 CH1 GPIO — PA0 AF1 (TIM3_CH1) */
	HAL_TIM_MspPostInit(&htim3);
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (htim->Instance == TIM3) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/* PA0 — TIM3_CH1, AF1 */
		GPIO_InitStruct.Pin = STATUS_LED_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
		HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);
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
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/* LED pin — start as push-pull output (will be switched to AF when PWM starts) */
	HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = STATUS_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);

	/* User button — PA1, external interrupt on rising edge */
	GPIO_InitStruct.Pin = USER_BTN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(USER_BTN_GPIO_Port, &GPIO_InitStruct);

	/* Enable EXTI interrupt for the button */
	HAL_NVIC_SetPriority(EXTI0_1_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
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
