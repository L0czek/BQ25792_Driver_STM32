# Quick Start Guide - BQ25792 STM32 Driver

## Overview

This guide helps you get started with the BQ25792 charger driver on STM32.

---

## Prerequisites

### Hardware
- STM32 board with I2C support (STM32G474RE, NUCLEO-G474RE, etc.)
- BQ25792 evaluation module or custom board
- ST-Link programmer/debugger
- USB cable for programming

### Software
- STM32CubeIDE 1.13.x or later
- STM32CubeG4 package
- CMake 3.15 or later

### Pin Connections

| BQ25792 Pin | STM32 Pin | Description |
|-------------|-----------|-------------|
| VCC | 3.3V | Power (3.3V) |
| GND | GND | Ground |
| SCL | PB8 | I2C1 SCL |
| SDA | PB9 | I2C1 SDA |
| ADD0 | GND | I2C Address LSB |
| ADD1 | GND | I2C Address LSB |

**Note:** Ensure 3.3V power and proper grounding. Add 4.7kΩ pull-up resistors on SCL and SDA if not already present on the board.

---

## Step-by-Step Setup

### 1. Clone and Setup

```bash
cd /path/to/your/project

# Option A: Clone the driver
git clone https://github.com/yourusername/BQ25792_Driver_STM32.git drivers/bq25792

# Option B: Add as submodule
git submodule add https://github.com/yourusername/BQ25792_Driver_STM32.git drivers/bq25792
```

### 2. Configure Your Project CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(my_bq25792_project)

# Add the BQ25792 driver
add_subdirectory(drivers/bq25792)

# Your application
add_executable(my_app
    Core/Src/main.c
    Core/Src/syscalls.c
    Core/Src/stm32g4xx_it.c
    Core/Src/stm32g4xx_hal_msp.c
)

# Link with BQ25792 driver
target_link_libraries(my_app
    PRIVATE
        bq25792
        stm32_hal
)

# Required compiler flags
target_compile_options(my_app
    PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -std=c++17
)
```

### 3. Configure I2C in main.c

```c
// In MX_I2C1_Init()
hi2c1.Instance = I2C1;
hi2c1.Init.ClockSpeed = 100000;
hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
hi2c1.Init.OwnAddress1 = 0;
hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
// ... rest of configuration
```

### 4. Use the Driver in Your Code

```cpp
#include "bq25792.hpp"

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();

    extern I2C_HandleTypeDef hi2c1;
    bq25792::Charger charger(&hi2c1);

    // Initialize
    auto result = charger.initialize();
    if (!result) {
        // Handle error
    }

    // Configure
    charger.set_charge_voltage_limit(4.2f);
    charger.set_charge_current_limit(2.0f);

    // Monitor
    float vbat = charger.get_vbat().value();
    float ibus = charger.get_ibus().value();
}
```

---

## Building and Testing

```bash
# Build
mkdir build && cd build
cmake .. -DSTM32_DEVICE=STM32G474 \
         -DSTM32_CUBE_PATH=/path/to/STM32CubeG4/Drivers
make

# Run unit tests (host-based)
make test_bq25792
ctest -V
```

---

## Common Issues

### Issue 1: I2C Error

**Symptoms:** `I2C communication error`

**Solution:**
1. Verify SCL/SDA pins are correct
2. Check pull-up resistors (4.7kΩ typical)
3. Verify I2C clock speed (100kHz for standard mode)
4. Check for address conflict (default: 0x6B)

### Issue 2: Device Not Found

**Symptoms:** `Invalid parameter` on initialize

**Solution:**
1. Verify BQ25792 is powered (3.3V)
2. Check I2C address with scope or logic analyzer
3. Verify ADD0/ADD1 pin configuration
4. Check for wiring issues

### Issue 3: Build Errors

**Symptoms:** Undefined reference to I2C functions

**Solution:**
1. Ensure `stm32g4xx_hal_i2c.c` is in HAL library
2. Verify `MX_I2C1_Init()` is called before driver usage
3. Check that `hi2c1` handle is declared globally

### Issue 4: Wrong Data

**Symptoms:** Read values don't match expected

**Solution:**
1. Verify I2C clock speed matches BQ25792 specs (max 400kHz)
2. Check for noise on I2C lines
3. Verify register addresses in code match datasheet
4. Use oscilloscope to verify I2C transactions

---

## Example Projects

See the following files for complete examples:

- **`Core/Src/main.c`** - STM32 test application
- **`tests/test_bq25792.cpp`** - Unit tests with mock HAL

---

## Next Steps

1. **Read Full Documentation**: See [README.md](README.md)
2. **API Reference**: See `Core/Inc/bq25792.hpp`
3. **Modify for Your Application**:
   - Adjust charging parameters
   - Add battery monitoring logic
   - Implement fault recovery
   - Add RTOS support if needed
4. **Optimize Performance**:
   - Use polling vs interrupts based on your needs
   - Consider DMA for bulk data transfers (if needed)

---

## Support

For issues:

1. Check this guide and README.md
2. Verify hardware connections
3. Use oscilloscope to check I2C signals
4. Review STM32 reference manual

---

**Happy Coding! 🚀**
