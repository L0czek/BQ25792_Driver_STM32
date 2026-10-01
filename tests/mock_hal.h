/**
 * @file mock_hal.h
 * @brief Mock HAL implementations for host-based unit testing
 */

#ifndef __MOCK_HAL_H
#define __MOCK_HAL_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>
#include <stdexcept>

// Mock HAL status codes
enum HAL_StatusTypeDef {
    HAL_OK = 0x00,
    HAL_ERROR = 0x01,
    HAL_BUSY = 0x02,
    HAL_TIMEOUT = 0x03
};

// Mock I2C handle structure
typedef struct {
    I2C_TypeDef* Instance;
    void* Init;
    void* State;
    void* ErrorCode;
    void* pCpltCallback;
    void* pErrorCallback;
    void* pCpltCallbackArg;
    void* pErrorCallbackArg;
} I2C_HandleTypeDef;

// Mock I2C macros
#define I2C_MODE_MASTER        0x00
#define I2C_MODE_SLAVE         0x01
#define I2C_MODE_MIXED         0x02
#define I2C_SPEED_FREQ_STANDARD 0x00
#define I2C_SPEED_FREQ_FAST    0x01
#define I2C_SPEED_FREQ_FAST_PLUS 0x02
#define I2C_DUTY_CYCLE_2       0x00
#define I2C_DUTY_CYCLE_16_9    0x04
#define I2C_ACK_DISABLE        0x00
#define I2C_ACK_ENABLE         0x01
#define I2C_NACK_POSITION_1    0x00
#define I2C_NACK_POSITION_2    0x02
#define I2C_ACKNOWLEDGE_ADDRESS_7BIT 0x00
#define I2C_ACKNOWLEDGE_ADDRESS_10BIT 0x04

// I2C register structure (minimal)
typedef struct {
    uint32_t CR1;
    uint32_t CR2;
    uint32_t OAR1;
    uint32_t OAR2;
    uint32_t DR;
    uint32_t SR1;
    uint32_t SR2;
    uint32_t CCR;
    uint32_t TRISE;
    uint32_t SFR;
} I2C_TypeDef;

// Mock HAL functions
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint8_t* pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef* hi2c, uint16_t DevAddress, uint8_t* pData, uint16_t Size, uint32_t Timeout);

// Mock HAL_Init
HAL_StatusTypeDef HAL_Init(void);

// Mock System Clock
void SystemClock_Config(void);

// Mock GPIO
void MX_GPIO_Init(void);

// Mock I2C
void MX_I2C1_Init(void);

#endif // __MOCK_HAL_H
