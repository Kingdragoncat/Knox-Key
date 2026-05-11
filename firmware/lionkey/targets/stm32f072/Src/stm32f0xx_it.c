/**
 ******************************************************************************
 * @file    stm32f0xx_it.c
 * @brief   Interrupt Service Routines — Knox Key STM32F072
 ******************************************************************************
 */

#include "main.h"
#include "stm32f0xx_it.h"
#include "tusb.h"

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/

void NMI_Handler(void) {
}

void HardFault_Handler(void) {
	Error_Handler();
}

void SVC_Handler(void) {
}

void PendSV_Handler(void) {
}

void SysTick_Handler(void) {
	HAL_IncTick();
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

/**
 * @brief  EXTI line 0 and 1 interrupt handler (user button on PA1)
 */
void EXTI0_1_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(USER_BTN_Pin);
}

/**
 * @brief  GPIO EXTI callback — called by HAL when EXTI fires
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == USER_BTN_Pin) {
		ButtonState = BUTTON_PRESSED;
	}
}

/**
 * @brief  USB interrupt handler — routes to TinyUSB
 */
void USB_IRQHandler(void) {
	tud_int_handler(0);
}
