/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   Knox Key STM32F072 target.
 ******************************************************************************
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/

extern __IO uint32_t ButtonState;

#define BUTTON_RELEASED  0
#define BUTTON_PRESSED   1

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

int Debug_UART_Get_Byte(void);

typedef enum Status_LED_Mode {
	STATUS_LED_MODE_OFF = 0,
	STATUS_LED_MODE_ON = 1,
	STATUS_LED_MODE_BLINKING_NORMAL = 2,
	STATUS_LED_MODE_BLINKING_SPECIAL = 3,
} Status_LED_Mode;

void Status_LED_Set_Mode(Status_LED_Mode mode);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Private defines -----------------------------------------------------------*/
/* Knox Key board pin assignments (from schematic) */

/* Status LED - PA0 (active high, optional replace with RGB) */
#define STATUS_LED_Pin        GPIO_PIN_0
#define STATUS_LED_GPIO_Port  GPIOA

/* User button - PA1 */
#define USER_BTN_Pin          GPIO_PIN_1
#define USER_BTN_GPIO_Port    GPIOA

/* ATECC608B I2C - PB6 (SCL), PB7 (SDA) */
#define ATECC_SCL_Pin         GPIO_PIN_6
#define ATECC_SCL_GPIO_Port   GPIOB
#define ATECC_SDA_Pin         GPIO_PIN_7
#define ATECC_SDA_GPIO_Port   GPIOB

/* USB D+ / D- are on PA11 / PA12 (handled by USB peripheral) */

/* SWD debug - PA13 (SWDIO), PA14 (SWCLK) */
#define SWDIO_Pin             GPIO_PIN_13
#define SWDIO_GPIO_Port       GPIOA
#define SWCLK_Pin             GPIO_PIN_14
#define SWCLK_GPIO_Port       GPIOA

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
