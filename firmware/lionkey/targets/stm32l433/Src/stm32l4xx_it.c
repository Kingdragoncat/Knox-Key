/**
 ******************************************************************************
 * @file    stm32l4xx_it.c
 * @brief   Interrupt Service Routines — Knox Key stm32l433
 ******************************************************************************
 */

#include "main.h"
#include "stm32l4xx_it.h"
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
	Status_LED_Update();
	Button_Update();
}

/******************************************************************************/
/* stm32l4xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

/**
 * @brief  EXTI line 0 and 1 interrupt handler (user button on PA1)
 */
void EXTI1_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(USER_BTN_Pin);
}

/**
 * @brief  GPIO EXTI callback — called by HAL when EXTI fires
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    (void)GPIO_Pin;
}

/**
 * @brief  USB interrupt handler — routes to TinyUSB
 */
void USB_IRQHandler(void) {
	tud_int_handler(0);
}
