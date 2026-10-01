/**
 * @file stm32g4xx_hal_msp.c
 * @brief HAL MSP module
 */

#include "stm32g4xx_hal.h"

/**
 * @brief I2C MSP Initialization
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if (hi2c->Instance == I2C1) {
        // Enable GPIO clock
        __HAL_RCC_GPIOB_CLK_ENABLE();
        
        // Enable I2C clock
        __HAL_RCC_I2C1_CLK_ENABLE();
        
        // Configure SCL and SDA pins
        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}
