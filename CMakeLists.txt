# =============================================================================
# BQ25792 Charger Driver CMake Library
# =============================================================================
# This library provides an I2C driver for the TI BQ25792 Smart Battery Manager
# for use with STM32 microcontrollers using the HAL driver.
#
# Usage in parent project:
#   add_subdirectory(BQ25792_Driver_STM32)
#   target_link_libraries(your_target PRIVATE bq25792)
# =============================================================================

cmake_minimum_required(VERSION 3.15)

# =============================================================================
# Configuration
# =============================================================================

# Generate compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(bq25792_driver
    LANGUAGES CXX C
    VERSION 1.0.0
    DESCRIPTION "BQ25792 Charger Driver for STM32 HAL"
)

# =============================================================================
# Configuration Options
# =============================================================================

# STM32 device (adjust as needed)
set(STM32_DEVICE STM32G474 CACHE STRING "STM32 device")

# Path to STM32CubeG4 (adjust as needed)
set(STM32_CUBE_PATH "" CACHE PATH "Path to STM32CubeG4 Drivers directory")

# Optional features
option(BQ25792_ENABLE_LOGGING "Enable verbose logging" OFF)
option(BQ25792_ENABLE_ASSERTS "Enable debug assertions" OFF)
option(BQ25792_BUILD_TESTS "Build STM32 test application" OFF)
option(BQ25792_BUILD_UNIT_TESTS "Build unit tests with mock HAL" ON)
option(BQ25792_INSTALL "Install BQ25792 driver" OFF)

# =============================================================================
# Find STM32 HAL
# =============================================================================

# Check if STM32_CUBE_PATH is set
if(NOT STM32_CUBE_PATH)
    message(FATAL_ERROR "STM32_CUBE_PATH must be set. Example: -DSTM32_CUBE_PATH=/path/to/STM32CubeG4/Drivers")
endif()

# Check if path exists
if(NOT EXISTS ${STM32_CUBE_PATH})
    message(FATAL_ERROR "STM32_CUBE_PATH does not exist: ${STM32_CUBE_PATH}")
endif()

# =============================================================================
# STM32 HAL Library
# =============================================================================

# Set C language for HAL library
enable_language(C)

add_library(stm32_hal STATIC)
target_sources(stm32_hal
    PRIVATE
        ${STM32_CUBE_PATH}/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
        ${STM32_CUBE_PATH}/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_i2c.c
        ${STM32_CUBE_PATH}/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
        ${STM32_CUBE_PATH}/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
        ${STM32_CUBE_PATH}/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
)

target_include_directories(stm32_hal
    PUBLIC
        ${STM32_CUBE_PATH}/STM32G4xx_HAL_Driver/Inc
        ${STM32_CUBE_PATH}/CMSIS/Device/ST/STM32G4xx/Include
)

target_compile_definitions(stm32_hal
    PUBLIC
        ${STM32_DEVICE}
        USE_HAL_DRIVER
        HSE_VALUE=16000000
        HSI_VALUE=16000000
)

target_compile_options(stm32_hal
    PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -O2
        -Wall
        -Wextra
)

# =============================================================================
# Library Definition
# =============================================================================

add_library(${PROJECT_NAME} STATIC)

# Add source files
target_sources(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/bq25792.cpp
)

# Add include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
        ${STM32_CUBE_PATH}/STM32G4xx_HAL_Driver/Inc
        ${STM32_CUBE_PATH}/CMSIS/Include
        ${STM32_CUBE_PATH}/CMSIS/Device/ST/STM32G4xx/Include
)

# Dependencies
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        stm32_hal
)

# =============================================================================
# Requirements
# =============================================================================

# Require C++17 standard (for std::expected)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

# =============================================================================
# Compile Definitions
# =============================================================================

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        # BQ25792 configuration
        BQ25792_I2C_ADDRESS=0x6B
        BQ25792_TIMEOUT_MS=1000
        BQ25792_IBUS_ADC_RESOLUTION=16
        BQ25792_VBAT_ADC_RESOLUTION=16
        BQ25792_VSYS_ADC_RESOLUTION=16
)

# =============================================================================
# Optional Features
# =============================================================================

if(BQ25792_ENABLE_LOGGING)
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            BQ25792_ENABLE_LOGGING=1
    )
endif()

if(BQ25792_ENABLE_ASSERTS)
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            BQ25792_ENABLE_ASSERTS=1
    )
endif()

# Compiler flags
target_compile_options(${PROJECT_NAME}
    PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfloat-abi=hard
        -mfpu=fpv4-sp-d16
        -O2
        -Wall
        -Wextra
)

# =============================================================================
# Test Application (STM32)
# =============================================================================

if(BQ25792_BUILD_TESTS)
    add_executable(bq25792_test
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/syscalls.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/stm32g4xx_it.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/stm32g4xx_hal_msp.c
    )

    target_include_directories(bq25792_test
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    )

    target_link_libraries(bq25792_test
        PRIVATE
            ${PROJECT_NAME}
            stm32_hal
    )

    # Linker script
    target_link_options(bq25792_test
        PRIVATE
            -T${CMAKE_CURRENT_SOURCE_DIR}/STM32G474VETX_FLASH.ld
            -mcpu=cortex-m4
            -mthumb
            -mfloat-abi=hard
            -mfpu=fpv4-sp-d16
            -Wl,-Map=bq25792_test.map
    )

    # Create HEX file
    add_custom_command(TARGET bq25792_test
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex bq25792_test.elf bq25792_test.hex
        COMMENT "Generating HEX file"
    )

    message(STATUS "STM32 test application enabled - build with 'make bq25792_test'")
endif()

# =============================================================================
# Unit Tests with Mocks (Host-based testing)
# =============================================================================

if(BQ25792_BUILD_UNIT_TESTS)
    enable_testing()

    # Create test executable
    add_executable(test_bq25792
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_bq25792.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/bq25792.cpp
    )

    target_include_directories(test_bq25792
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
            ${STM32_CUBE_PATH}/STM32G4xx_HAL_Driver/Inc
            ${STM32_CUBE_PATH}/CMSIS/Include
            ${STM32_CUBE_PATH}/CMSIS/Device/ST/STM32G4xx/Include
    )

    target_compile_definitions(test_bq25792
        PUBLIC
            ${STM32_DEVICE}xx
            BQ25792_I2C_ADDRESS=0x6B
            BQ25792_TIMEOUT_MS=1000
    )

    # Use host C++17 compiler
    set_target_properties(test_bq25792
        PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            COMPILE_OPTIONS "-std=c++17"
    )

    # Link with standard C++ library
    target_link_libraries(test_bq25792
        PRIVATE
            stdc++
    )

    # Add test (use full path to ensure CTest can find it)
    add_test(NAME test_bq25792 COMMAND $<TARGET_FILE:test_bq25792>)

    message(STATUS "Unit tests enabled - build with 'make test_bq25792'")
endif()

# =============================================================================
# Installation
# =============================================================================

if(BQ25792_INSTALL)
    include(GNUInstallDirs)

    install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT ${PROJECT_NAME}-targets
        FILE
            ${PROJECT_NAME}-targets.cmake
        NAMESPACE
            bq25792::
        DESTINATION
            lib/cmake/bq25792
    )

    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
        DESTINATION include
    )

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md
        DESTINATION share/doc/bq25792
    )
endif()

# =============================================================================
# Summary
# =============================================================================

message(STATUS "")
message(STATUS "BQ25792 Charger Driver Configuration:")
message(STATUS "  Version:           ${PROJECT_VERSION}")
message(STATUS "  Device:            ${STM32_DEVICE}")
message(STATUS "  Cube Path:         ${STM32_CUBE_PATH}")
message(STATUS "  Logging:           ${BQ25792_ENABLE_LOGGING}")
message(STATUS "  Assertions:        ${BQ25792_ENABLE_ASSERTS}")
message(STATUS "  Build Tests:       ${BQ25792_BUILD_TESTS}")
message(STATUS "  Build Unit Tests:  ${BQ25792_BUILD_UNIT_TESTS}")
message(STATUS "  Install:           ${BQ25792_INSTALL}")
message(STATUS "")
message(STATUS "Available targets:")
message(STATUS "  bq25792           - Build BQ25792 library")
if(BQ25792_BUILD_TESTS)
    message(STATUS "  bq25792_test      - Build test application (STM32)")
endif()
if(BQ25792_BUILD_UNIT_TESTS)
    message(STATUS "  test_bq25792      - Build unit tests (host)")
endif()
message(STATUS "")
message(STATUS "To build:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake .. -DSTM32_DEVICE=${STM32_DEVICE} -DSTM32_CUBE_PATH=${STM32_CUBE_PATH}")
message(STATUS "  make")
if(BQ25792_BUILD_TESTS)
    message(STATUS "  make bq25792_test - Build STM32 test application")
endif()
if(BQ25792_BUILD_UNIT_TESTS)
    message(STATUS "  make test_bq25792  - Build unit tests")
    message(STATUS "  ctest             - Run unit tests")
endif()
message(STATUS "")
