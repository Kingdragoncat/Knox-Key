/**
 ******************************************************************************
 * @file    stm32f0xx_hal_conf.h
 * @brief   HAL configuration file for Knox Key (STM32F072CB).
 ******************************************************************************
 */

#ifndef __STM32F0XX_HAL_CONF_H
#define __STM32F0XX_HAL_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* ########################## Module Selection ############################## */
#define HAL_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
// Note: We do NOT enable HAL_PCD_MODULE because TinyUSB drives USB directly
// #define HAL_PCD_MODULE_ENABLED

/* ########################## HSE/HSI Values ################################ */

/* Value of the External oscillator in Hz */
#if !defined(HSE_VALUE)
#define HSE_VALUE    ((uint32_t)8000000U) /*!< 8 MHz external crystal on Knox Key */
#endif

#if !defined(HSE_STARTUP_TIMEOUT)
#define HSE_STARTUP_TIMEOUT    ((uint32_t)100U)   /*!< Time out for HSE start up, in ms */
#endif

/* Value of the Internal oscillator in Hz */
#if !defined(HSI_VALUE)
#define HSI_VALUE    ((uint32_t)8000000U) /*!< Value of the Internal oscillator in Hz */
#endif

/* Value of the Internal 48 MHz oscillator in Hz (for USB) */
#if !defined(HSI48_VALUE)
#define HSI48_VALUE  ((uint32_t)48000000U) /*!< Value of HSI48 */
#endif

/* Value of the Internal Low Speed oscillator in Hz */
#if !defined(LSI_VALUE)
#define LSI_VALUE    ((uint32_t)40000U)
#endif

#if !defined(LSE_VALUE)
#define LSE_VALUE    ((uint32_t)32768U)    /*!< Value of the External Low Speed oscillator in Hz */
#endif

#if !defined(LSE_STARTUP_TIMEOUT)
#define LSE_STARTUP_TIMEOUT    ((uint32_t)5000U)   /*!< Time out for LSE start up, in ms */
#endif

/* ########################### System Configuration ######################### */
#define VDD_VALUE                    ((uint32_t)3300U) /*!< Value of VDD in mv */
#define TICK_INT_PRIORITY            ((uint32_t)0U)    /*!< tick interrupt priority (lowest by default)  */
#define USE_RTOS                     0U
#define PREFETCH_ENABLE              1U
#define INSTRUCTION_CACHE_ENABLE     0U
#define DATA_CACHE_ENABLE            0U

/* ########################## Assert Selection ############################## */
/* #define USE_FULL_ASSERT    1U */

/* Includes ------------------------------------------------------------------*/
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32f0xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32f0xx_hal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32f0xx_hal_dma.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32f0xx_hal_cortex.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32f0xx_hal_flash.h"
#endif

#ifdef HAL_I2C_MODULE_ENABLED
  #include "stm32f0xx_hal_i2c.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32f0xx_hal_pwr.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32f0xx_hal_tim.h"
#endif

#ifdef HAL_PCD_MODULE_ENABLED
  #include "stm32f0xx_hal_pcd.h"
#endif

/* ########################## Assert Configuration ########################## */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t *file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32F0XX_HAL_CONF_H */
