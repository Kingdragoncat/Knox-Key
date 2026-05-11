/**
 * @file    system_stm32f0xx.c
 * @brief   CMSIS Cortex-M0 Device Peripheral Access Layer System Source File
 *          for STM32F072 (Knox Key).
 *
 * Adapted from STM32CubeF0 template. Sets SystemCoreClock to 48 MHz
 * (will be updated by SystemClock_Config in main.c via HAL_RCC_ClockConfig).
 */

#include "stm32f0xx.h"

#if !defined(HSE_VALUE)
  #define HSE_VALUE    ((uint32_t)8000000U)
#endif

#if !defined(HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)8000000U)
#endif

#if !defined(HSI48_VALUE)
  #define HSI48_VALUE  ((uint32_t)48000000U)
#endif

/* This variable is updated in three ways:
    1) by calling CMSIS function SystemCoreClockUpdate()
    2) by calling HAL API function HAL_RCC_GetHCLKFreq()
    3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
*/
uint32_t SystemCoreClock = 48000000U;

const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};

/**
 * @brief  Setup the microcontroller system.
 *         Initialize the default HSI clock source, vector table location and the PLL configuration is reset.
 * @retval None
 */
void SystemInit(void) {
	/* NOTE: The actual clock configuration is done in SystemClock_Config() in main.c,
	   which is called after HAL_Init(). This function only performs minimal setup. */
}

/**
 * @brief  Update SystemCoreClock variable according to Clock Register Values.
 *         The SystemCoreClock variable contains the core clock (HCLK), it can
 *         be used by the user application to setup the SysTick timer or configure
 *         other parameters.
 * @retval None
 */
void SystemCoreClockUpdate(void) {
	uint32_t tmp = 0, pllmull = 0, pllsource = 0, predivfactor = 0;

	/* Get SYSCLK source */
	tmp = RCC->CFGR & RCC_CFGR_SWS;

	switch (tmp) {
		case RCC_CFGR_SWS_HSI:
			SystemCoreClock = HSI_VALUE;
			break;
		case RCC_CFGR_SWS_HSE:
			SystemCoreClock = HSE_VALUE;
			break;
		case RCC_CFGR_SWS_PLL:
			pllmull = RCC->CFGR & RCC_CFGR_PLLMUL;
			pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
			pllmull = (pllmull >> 18) + 2;
			predivfactor = (RCC->CFGR2 & RCC_CFGR2_PREDIV) + 1;

			if (pllsource == RCC_CFGR_PLLSRC_HSE_PREDIV) {
				SystemCoreClock = (HSE_VALUE / predivfactor) * pllmull;
			} else {
				SystemCoreClock = (HSI_VALUE / 2) * pllmull;
			}
			break;
		case RCC_CFGR_SWS_HSI48:
			SystemCoreClock = HSI48_VALUE;
			break;
		default:
			SystemCoreClock = HSI_VALUE;
			break;
	}

	/* Compute HCLK clock frequency */
	tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
	SystemCoreClock >>= tmp;
}
