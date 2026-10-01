# =============================================================================
# BQ25792 Charger Driver - Library Integration CMakeLists.txt
# =============================================================================
# Usage in parent project:
#   add_subdirectory(path/to/BQ25792_Driver_STM32/library)
#   target_link_libraries(your_target PRIVATE bq25792)
# =============================================================================

cmake_minimum_required(VERSION 3.15)

# =============================================================================
# Library Definition
# =============================================================================

add_library(bq25792 STATIC)

# Add source files
target_sources(bq25792
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Src/bq25792.cpp
)

# Add include directories
# Core/Inc is at ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
target_include_directories(bq25792
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
)

# Dependencies (HAL library must be provided by parent project)
target_link_libraries(bq25792
    PUBLIC
        $<TARGET_NAME_IF_EXISTS:stm32_hal>
)

# =============================================================================
# Requirements
# =============================================================================

set_target_properties(bq25792
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

# =============================================================================
# Compile Definitions
# =============================================================================

target_compile_definitions(bq25792
    PUBLIC
        BQ25792_I2C_ADDRESS=0x6B
        BQ25792_TIMEOUT_MS=1000
        BQ25792_IBUS_ADC_RESOLUTION=16
        BQ25792_VBAT_ADC_RESOLUTION=16
        BQ25792_VSYS_ADC_RESOLUTION=16
)
