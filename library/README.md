# BQ25792 Charger Driver

I2C driver for the TI BQ25792 Smart Battery Manager for STM32 microcontrollers.

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

## API

See `../Core/Inc/bq25792.hpp` for the complete API reference.
