# BQ25792 Charger Driver

I2C driver for the Texas Instruments BQ25792 Smart Battery Manager for STM32 microcontrollers.

[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C++17-blue.svg)](https://en.cppreference.com/w/cpp/17)

## Features

- **Wide Input Range**: 3.9V to 25.5V input voltage range
- **High Current Support**: Up to 12.79A charge current
- **Multiple Input Sources**: Supports USB DPDM, AC1, AC2 adapters
- **USB OTG Support**: 5V boost mode for USB OTG
- **Adaptive Input Current Optimization (AICO)**: Optimizes input current
- **Battery Charge Protection**: Programmable charge voltage and current limits
- **System Voltage Minimization**: VSYS_MIN to optimize battery runtime
- **Precharge & Taper Charging**: Full charge profile support
- **Fault Monitoring**: Comprehensive fault status with maskable interrupts
- **ADC Monitoring**: Built-in ADC for voltage and current sensing

## Integration as Submodule

Add this driver as a Git submodule to your project:

```bash
git submodule add https://github.com/yourorg/BQ25792_Driver_STM32.git vendor/BQ25792
```

In your project's `CMakeLists.txt`:

```cmake
# Add the driver as a subdirectory
add_subdirectory(vendor/BQ25792/library)

# Link the library to your target
target_link_libraries(your_target PRIVATE bq25792)
```

## Requirements

- C++17 compiler (for `std::expected`)
- STM32 HAL library (must be defined in parent project as `stm32_hal` target)
- I2C peripheral support

## Quick Start

```cpp
#include "bq25792.hpp"

// Initialize driver with I2C handle
bq25792::Charger charger(&hi2c);

// Initialize the charger
charger.initialize();

// Set charge voltage to 12.6V (typical for Li-ion)
charger.set_charge_voltage_limit(12.6f);

// Set charge current limit to 2A
charger.set_charge_current_limit(2.0f);

// Set input current limit to 3A
charger.set_input_current_limit(3.0f);

// Read battery voltage
if (auto vbat = charger.get_vbat(); vbat.has_value()) {
    float battery_voltage = vbat.value();
}
```

## I2C Address

The BQ25792 uses a fixed I2C address:
- **Address**: 0x6B (7-bit)

## Register Map (Key Registers)

| Address | Name | Description |
|---------|------|-------------|
| 0x00 | VSYS_MIN | Minimal system voltage |
| 0x01 | CHARGEV | Charge voltage limit |
| 0x03 | CHARGEI | Charge current limit |
| 0x05 | VINDPM | Input voltage limit |
| 0x06 | INDPMI | Input current limit |
| 0x1B-0x1F | STAT0-4 | Charger status |
| 0x20-0x21 | FAULT0-1 | Fault status |
| 0x31-0x45 | ADC readings | Voltage/current measurements |

## API Reference

### Device Control

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize the charger |
| `reset()` | Reset the charger |
| `is_initialized()` | Check if driver is initialized |

### System Configuration

| Method | Description |
|--------|-------------|
| `set_vsys_min(float)` | Set minimal system voltage (3.5-10.5V) |
| `get_vsys_min()` | Get minimal system voltage |

### Charge Configuration

| Method | Description |
|--------|-------------|
| `set_charge_voltage_limit(float)` | Set charge voltage (3.5-19.2V) |
| `get_charge_voltage_limit()` | Get charge voltage |
| `set_charge_current_limit(float)` | Set charge current (0-12.79A) |
| `get_charge_current_limit()` | Get charge current |

### Input Configuration

| Method | Description |
|--------|-------------|
| `set_input_voltage_limit(float)` | Set input voltage limit (3.9-25.5V) |
| `get_input_voltage_limit()` | Get input voltage limit |
| `set_input_current_limit(float)` | Set input current limit (0-12.79A) |
| `get_input_current_limit()` | Get input current limit |

### Precharge Configuration

| Method | Description |
|--------|-------------|
| `set_precharge_control(vbat_low, iprechrg)` | Set precharge settings |
| `get_precharge_control()` | Get precharge settings |

### Status & Monitoring

| Method | Description |
|--------|-------------|
| `is_plugged_in()` | Check if input is plugged in |
| `get_charge_status()` | Get charge status enum |
| `get_vbus_status()` | Get VBUS status enum |
| `is_battery_present()` | Check if battery is present |
| `is_fault_present()` | Check for faults |
| `get_vbat()` | Get battery voltage |
| `get_ibus()` | Get IBUS current |
| `get_device_info()` | Get part ID |

### ADC Configuration

| Method | Description |
|--------|-------------|
| `get_vbus()` | Get VBUS voltage |
| `get_vsys()` | Get VSYS voltage |
| `get_ibat()` | Get IBAT current |
| `get_ts()` | Get TS (temperature) voltage |

## Charge Status

| Enum | Description |
|------|-------------|
| `NotCharging` | Not charging |
| `TrickleCharge` | Trickle charging |
| `Precharge` | Precharging |
| `FastCharge` | Fast charging |
| `TaperCharge` | Taper charging |
| `TopOff` | Top-off charging |
| `ChargeDone` | Charge complete |

## VBUS Status

| Enum | Description |
|------|-------------|
| `NoInput` | No input |
| `USBSDP500mA` | USB SDP (500mA) |
| `USBCDP1500mA` | USB CDP (1.5A) |
| `USBDCP3250mA` | USB DCP (3.25A) |
| `AdjustableHigh` | Adjustable high power adapter |
| `UnknownAdapter` | Unknown adapter |
| `NonStandardAdapter` | Non-standard adapter |
| `OTGMode` | OTG mode (boost) |

## Complete Example

```cpp
#include "bq25792.hpp"
#include <iostream>

void print_status(bq25792::Charger& charger) {
    // Check if plugged in
    if (auto plugged = charger.is_plugged_in(); plugged.has_value() && plugged.value()) {
        std::cout << "Input: Plugged in" << std::endl;
        
        // Get charge status
        if (auto status = charger.get_charge_status(); status.has_value()) {
            switch (status.value()) {
                case bq25792::ChargeStatus::FastCharge:
                    std::cout << "Status: Fast charging" << std::endl;
                    break;
                case bq25792::ChargeStatus::ChargeDone:
                    std::cout << "Status: Charge complete" << std::endl;
                    break;
            }
        }
        
        // Read battery voltage and current
        if (auto vbat = charger.get_vbat(); vbat.has_value()) {
            std::cout << "Battery: " << vbat.value() << "V" << std::endl;
        }
        if (auto ibus = charger.get_ibus(); ibus.has_value()) {
            std::cout << "Current: " << ibus.value() << "A" << std::endl;
        }
    } else {
        std::cout << "Input: Not plugged in" << std::endl;
    }
}

int main() {
    bq25792::Charger charger(&hi2c1);
    
    // Initialize
    if (!charger.initialize().has_value()) {
        std::cout << "Failed to initialize charger" << std::endl;
        return -1;
    }
    
    // Configure
    charger.set_charge_voltage_limit(12.6f);
    charger.set_charge_current_limit(2.0f);
    charger.set_input_current_limit(3.0f);
    
    // Main loop
    while (true) {
        print_status(charger);
        
        // Check for faults
        if (auto fault = charger.is_fault_present(); fault.has_value() && fault.value()) {
            std::cout << "Warning: Fault detected!" << std::endl;
        }
        
        HAL_Delay(1000);
    }
}
```

## Error Handling

```cpp
auto result = charger.set_charge_voltage_limit(12.6f);
if (!result.has_value()) {
    std::error_code ec = result.error();
    
    if (ec.category() == bq25792::get_error_category()) {
        switch (static_cast<bq25792::ErrorCode>(ec.value())) {
            case bq25792::ErrorCode::Timeout:
                // Handle timeout
                break;
            case bq25792::ErrorCode::HALError:
                // Handle HAL error
                break;
            case bq25792::ErrorCode::InvalidParameter:
                // Handle invalid parameter
                break;
        }
    }
}
```

## Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `BQ25792_I2C_ADDRESS` | 0x6B | I2C address |
| `BQ25792_TIMEOUT_MS` | 1000 | Command timeout |
| `BQ25792_IBUS_ADC_RESOLUTION` | 16 | IBUS ADC resolution |
| `BQ25792_VBAT_ADC_RESOLUTION` | 16 | VBAT ADC resolution |

## Troubleshooting

### Device Not Detected
- Verify I2C address (0x6B)
- Check I2C pull-up resistors (4.7kΩ typical)
- Verify I2C clock speed (100kHz or 400kHz)

### Charging Not Starting
- Verify battery is present
- Check charge voltage and current limits
- Ensure no fault conditions exist

### Fault Conditions
- **SCP**: Short circuit - check for shorts on output
- **OCP**: Over current - reduce load or increase limit
- **OVP**: Over voltage - check input voltage
- **OTG**: Over temperature - improve cooling

## License

MIT License - see LICENSE file for details
