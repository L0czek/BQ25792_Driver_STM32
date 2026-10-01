# BQ25792 STM32 Driver

This repository provides a modern C++17 driver for the **TI BQ25792 Smart Battery Manager** with **STM32 HAL I2C** integration. The driver follows the same pattern as the APS6404L PSRAM driver, using `std::expected` for error handling.

## 📋 Overview

The BQ25792 is a high-efficiency, I2C-controlled charging controller for single-cell Li+ batteries. This driver provides:

- **Modern C++17 interface** with `std::expected` error handling
- **I2C HAL integration** for STM32 microcontrollers
- **Full register access** with type-safe methods
- **Compile-time configuration** for flexibility
- **Host-based unit tests** with mock HAL
- **CMake build system** for easy integration

## 🚀 Features

- **I2C initialization** and device detection
- **Voltage/current limit configuration** (charge, input)
- **Precharge control** with configurable thresholds
- **Status monitoring** (charge status, VBUS status, battery presence)
- **Fault detection** with fault status registers
- **ADC readings** (VBAT, IBUS, VSYS)
- **Device information** retrieval (part ID)

## 🛠 Hardware Details

- **Microcontroller:** STM32 (any with I2C support)
- **Charger IC:** BQ25792 (TI Smart Battery Manager)
- **Interface:** I2C (Standard Mode: 100kHz, Fast Mode: 400kHz)
- **Battery Support:** Single-cell Li+ (3.5V - 12V system)

## 📂 Project Structure

```
BQ25792_Driver_STM32/
├── library/
│   └── bq25792_expected.hpp     # Namespace-specific C++17 expected implementation
├── Core/
│   ├── Inc/
│   │   ├── bq25792.hpp          # Main header with API
│   └── Src/
│       └── bq25792.cpp          # Driver implementation
├── tests/
│   ├── CMakeLists.txt
│   ├── mock_hal.h               # Mock HAL for testing
│   ├── test_bq25792.cpp         # Unit tests
│   └── test_main.cpp
├── CMakeLists.txt               # Main CMake configuration
├── README.md                    # This file
└── QUICK_START.md               # Quick start guide
```

## 🏁 Quick Start - CMake Build

```bash
# Clone and setup
cd BQ25792_Driver_STM32

# Configure with CMake
mkdir build && cd build
cmake .. -DSTM32_DEVICE=STM32G474 \
         -DSTM32_CUBE_PATH=/path/to/STM32CubeG4/Drivers \
         -DBQ25792_BUILD_UNIT_TESTS=ON

# Build and run tests
make test_bq25792
ctest -V
```

### Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `BQ25792_ENABLE_LOGGING` | OFF | Enable verbose logging |
| `BQ25792_ENABLE_ASSERTS` | OFF | Enable debug assertions |
| `BQ25792_BUILD_TESTS` | OFF | Build STM32 test app |
| `BQ25792_BUILD_UNIT_TESTS` | ON | Build host unit tests |
| `BQ25792_INSTALL` | OFF | Install target |

## 🔌 I2C Pin Connections

| BQ25792 Pin | STM32 Pin | Description |
|-------------|-----------|-------------|
| VCC | 3.3V | Power (3.3V) |
| GND | GND | Ground |
| SCL | I2C_SCL | I2C Clock |
| SDA | I2C_SDA | I2C Data |
| ADD0 | GND/VCC | I2C Address LSB (0x6B or 0x6A) |
| ADD1 | GND/VCC | I2C Address LSB (0x6B or 0x6A) |

**Default I2C Address:** `0x6B` (both ADD pins to GND)

## 📖 Integration Guide

### Option 1: Submodule Integration (Recommended)

1. **Add as Git Submodule**
```bash
cd your_project
git submodule add https://github.com/yourusername/BQ25792_Driver_STM32.git drivers/bq25792
```

2. **Update your CMakeLists.txt**
```cmake
# In your project CMakeLists.txt
add_subdirectory(drivers/bq25792)
target_link_libraries(your_target PRIVATE bq25792)
```

### Option 2: Copy Source Files

1. **Copy the driver files**
```bash
cp -r BQ25792_Driver_STM32/Core/Inc drivers/bq25792/
cp -r BQ25792_Driver_STM32/Core/Src/bq25792.cpp drivers/bq25792/
```

2. **Add to your project**
```cmake
# In your CMakeLists.txt
add_library(bq25792 STATIC
    drivers/bq25792/bq25792.cpp
)
target_include_directories(bq25792 PUBLIC drivers/bq25792)
```

## 💻 Application Code

### Basic Usage

```cpp
#include "bq25792.hpp"
#include <iostream>

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();

    // I2C handle (defined in main.c)
    extern I2C_HandleTypeDef hi2c1;

    // Create charger instance
    bq25792::Charger charger(&hi2c1);

    // Initialize charger
    auto result = charger.initialize();
    if (!result) {
        // Handle error
        std::cerr << "Failed to initialize: " << result.error().message() << std::endl;
        return 1;
    }

    // Configure charging parameters
    charger.set_charge_voltage_limit(4.2f);  // 4.2V
    charger.set_charge_current_limit(2.0f);  // 2A
    charger.set_input_voltage_limit(5.0f);   // 5V
    charger.set_input_current_limit(3.0f);   // 3A

    // Monitor status
    auto charge_status = charger.get_charge_status();
    auto vbus_status = charger.get_vbus_status();
    bool plugged_in = charger.is_plugged_in();
    bool battery_present = charger.is_battery_present();

    // Read battery voltage
    float vbat = charger.get_vbat();
    float ibus = charger.get_ibus();

    while (1) {
        // Your application code
    }
}
```

### Using the Buffer Class

```cpp
#include "bq25792.hpp"

int main(void)
{
    bq25792::Charger charger(&hi2c1);

    // Create aligned buffer (8-byte default for I2C)
    bq25792::Buffer buffer(256);

    // Use buffer.data() to access the buffer
    charger.write_bytes(0x00, buffer.data(), buffer.size());

    // Fill with pattern
    buffer.fill(0xAA);
    buffer.clear();
}
```

## 📚 API Reference

### Charger Class

```cpp
#include "bq25792.hpp"

// Create charger instance
extern I2C_HandleTypeDef hi2c1;
bq25792::Charger charger(&hi2c1);

// Initialization
charger.initialize();      // Initialize and verify device
charger.reset();           // Software reset

// Voltage Configuration
charger.get_vsys_min();                    // Get minimal system voltage
charger.set_vsys_min(7.5f);                // Set to 7.5V

charger.get_charge_voltage_limit();        // Get charge voltage limit
charger.set_charge_voltage_limit(4.2f);    // Set charge voltage

charger.get_input_voltage_limit();         // Get input voltage limit
charger.set_input_voltage_limit(5.0f);     // Set input voltage

// Current Configuration
charger.get_charge_current_limit();        // Get charge current limit
charger.set_charge_current_limit(2.0f);    // Set charge current

charger.get_input_current_limit();         // Get input current limit
charger.set_input_current_limit(3.0f);     // Set input current

// Precharge Control
charger.get_precharge_control();           // Get precharge settings
charger.set_precharge_control(3.1f, 0.2f); // Set Vbat_low=3.1V, Iprechrg=200mA

// Status Monitoring
charger.is_plugged_in();                   // Check if VBUS present
charger.get_charge_status();               // Get charge status enum
charger.get_vbus_status();                 // Get VBUS status enum
charger.is_battery_present();              // Check if battery present
charger.is_fault_present();                // Check for faults

// ADC Readings
charger.get_vbat();                        // Get battery voltage
charger.get_ibus();                        // Get IBUS current

// Device Information
charger.get_device_info();                 // Get part ID
charger.get_i2c_address();                 // Get I2C address (static)
```

### Status Enums

```cpp
// Charge Status
bq25792::ChargeStatus: NotCharging, TrickleCharge, Precharge, 
                       FastCharge, TaperCharge, Reserved, TopOff, ChargeDone

// VBUS Status
bq25792::VBUSStatus: NoInput, USBSDP500mA, USBCDP1500mA, USBDCP3250mA,
                     AdjustableHigh, UnknownAdapter, NonStandardAdapter,
                     OTGMode, NotQualifiedAdapter, PoweredFromVBUS
```

## ⚙️ Configuration

### Compile Definitions

| Definition | Default | Description |
|------------|---------|-------------|
| `BQ25792_I2C_ADDRESS` | 0x6B | I2C address (7-bit) |
| `BQ25792_TIMEOUT_MS` | 1000 | I2C timeout in ms |
| `BQ25792_IBUS_ADC_RESOLUTION` | 16 | IBUS ADC bits |
| `BQ25792_VBAT_ADC_RESOLUTION` | 16 | VBAT ADC bits |
| `BQ25792_VSYS_ADC_RESOLUTION` | 16 | VSYS ADC bits |

### Optional Features

| Feature | Definition | Default | Description |
|---------|------------|---------|-------------|
| Logging | `BQ25792_ENABLE_LOGGING` | OFF | Enable debug logging |
| Assertions | `BQ25792_ENABLE_ASSERTS` | OFF | Enable debug assertions |

## 🔧 Error Handling

The driver uses `std::expected` for error handling (C++17 compatible implementation included):

```cpp
auto result = charger.get_vsys_min();
if (result) {
    float voltage = result.value();
    std::cout << "VSYS_MIN: " << voltage << "V" << std::endl;
} else {
    std::cerr << "Error: " << result.error().message() << std::endl;
    
    // Check specific error codes
    if (result.error() == bq25792::make_error_code(bq25792::ErrorCode::Timeout)) {
        // Handle timeout
    }
}
```

### Error Codes

| Code | Description |
|------|-------------|
| `None` | No error |
| `HALError` | HAL operation failed |
| `Timeout` | Operation timed out |
| `I2CError` | I2C communication error |
| `InvalidAddress` | Invalid register address |
| `InvalidParameter` | Invalid parameter value |
| `NotInitialized` | Driver not initialized |
| `BatteryNotPresent` | Battery not detected |
| `FaultDetected` | Fault condition detected |

## 🧪 Testing

### Unit Tests (Host-based)

```bash
cd build
cmake .. -DSTM32_DEVICE=STM32G474 -DSTM32_CUBE_PATH=/path/to/STM32CubeG4/Drivers -DBQ25792_BUILD_UNIT_TESTS=ON
make test_bq25792
ctest -V
```

### Test Coverage

- Constructor and static methods
- I2C communication (read/write)
- Voltage/current configuration
- Status register reading
- Error handling
- Buffer class
- Device identification

## 📝 Notes

1. **I2C Speed:** The BQ25792 supports up to 400kHz (Fast Mode)
2. **Register Access:** All register access goes through I2C; ensure proper pull-up resistors
3. **Power Sequence:** Follow the power-up sequence in the datasheet
4. **Interrupts:** The BQ25792 has an NCHG interrupt pin (not currently implemented in driver)

## 📚 References

- [BQ25792 Datasheet](https://www.ti.com/lit/ds/symlink/bq25792.pdf)
- [APS6404L PSRAM Driver](../APS6404L_STM32_DRIVER) (same pattern)

## 🤝 Contributing

Contributions are welcome! Please follow the existing code style and add tests for new features.

## 📄 License

This driver is provided as-is for use with STM32 microcontrollers.

---

**Happy Coding! 🚀**
