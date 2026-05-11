/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   Knox Key stm32l433 target.
 ******************************************************************************
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/

typedef enum {
    BUTTON_EVENT_NONE = 0,
    BUTTON_EVENT_SINGLE_CLICK,
    BUTTON_EVENT_DOUBLE_CLICK,
    BUTTON_EVENT_LONG_PRESS
} Button_Event;

extern volatile Button_Event CurrentButtonEvent;
void Button_Update(void);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

int Debug_UART_Get_Byte(void);

typedef enum Status_LED_Mode {
	STATUS_LED_MODE_OFF = 0,
	STATUS_LED_MODE_ON = 1,
	STATUS_LED_MODE_BLINKING_NORMAL = 2,
	STATUS_LED_MODE_BLINKING_SPECIAL = 3,
    STATUS_LED_MODE_SUCCESS = 4,
    STATUS_LED_MODE_ERROR = 5,
    STATUS_LED_MODE_WINK = 6,
} Status_LED_Mode;

void Status_LED_Set_Mode(Status_LED_Mode mode);
void Status_LED_Update(void);

/* Private defines -----------------------------------------------------------*/
/* Knox Key board pin assignments (from schematic) */

/* RGB LED - PA5, PA6, PA7 */
#define LED_R_Pin             GPIO_PIN_5
#define LED_G_Pin             GPIO_PIN_6
#define LED_B_Pin             GPIO_PIN_7
#define LED_GPIO_Port         GPIOA

/* User button - PA0 */
#define USER_BTN_Pin          GPIO_PIN_0
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
