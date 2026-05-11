cmake_minimum_required(VERSION 3.22)
# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Macro definitions
set(MX_Defines_Syms
	USE_HAL_DRIVER
	STM32F072xB
    $<$<CONFIG:Debug>:DEBUG>
)

set(MX_Root_Dir ../..)

# Include paths
set(MX_Include_Dirs
    ${MX_Root_Dir}/Inc
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Inc
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Inc/Legacy
    ${MX_Root_Dir}/_deps/cmsis_device_f0/Include
    ${MX_Root_Dir}/_deps/cmsis_core/CMSIS/Core/Include
)

# Application sources
set(MX_Application_Src
    ${MX_Root_Dir}/Src/main.c
    ${MX_Root_Dir}/Src/stm32f0xx_it.c
    ${MX_Root_Dir}/Src/sysmem.c
    ${MX_Root_Dir}/Src/syscalls.c
    ${MX_Root_Dir}/startup_stm32f072xb.s
)

# STM32 HAL/LL Drivers — only the modules we actually use
set(STM32_Drivers_Src
    ${MX_Root_Dir}/Src/system_stm32f0xx.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_cortex.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_rcc.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_rcc_ex.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_gpio.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_flash.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_flash_ex.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_dma.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_pwr.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_pwr_ex.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_i2c.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_i2c_ex.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_tim.c
    ${MX_Root_Dir}/_deps/stm32f0xx_hal_driver/Src/stm32f0xx_hal_tim_ex.c
)

# Interface library for includes and symbols
add_library(stm32cubemx INTERFACE)
target_include_directories(stm32cubemx INTERFACE ${MX_Include_Dirs})
target_compile_definitions(stm32cubemx INTERFACE ${MX_Defines_Syms})

# Create STM32_Drivers static library
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${STM32_Drivers_Src})
target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)

# Add application sources to the project
target_sources(${PROJECT_NAME} PRIVATE ${MX_Application_Src})

# Add libraries to the project
target_link_libraries(${PROJECT_NAME} STM32_Drivers)

# Validate C standard
if((CMAKE_C_STANDARD EQUAL 90) OR (CMAKE_C_STANDARD EQUAL 99))
    message(ERROR "Generated code requires C11 or higher")
endif()
