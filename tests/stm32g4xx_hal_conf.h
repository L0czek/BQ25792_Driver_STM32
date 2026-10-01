/**
 * @file stm32g4xx_hal_conf.h
 * @brief Minimal HAL configuration for unit testing
 */

#ifndef __STM32G4xx_HAL_CONF_H
#define __STM32G4xx_HAL_CONF_H

/* Select the appropriate device */
#define STM32G4
#define STM32G474xx

/* HAL module clock */
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED

/* HSE/HSI values */
#define HSE_VALUE    16000000U
#define HSI_VALUE    16000000U
#define LSE_VALUE    32768U
#define LSI_VALUE    32000U

/* Flash prefetch */
#define FLASH_PREFETCH_ENABLE 1

/* Include the HAL common header which defines HAL_StatusTypeDef */
#include "stm32g4xx_hal_def.h"

/* Include individual module headers needed for testing */
#include "stm32g4xx_hal_dma.h"
#include "stm32g4xx_hal_i2c.h"

/* Include the main HAL header */
#include "stm32g4xx_hal.h"

#endif /* __STM32G4xx_HAL_CONF_H */
