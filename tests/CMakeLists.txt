cmake_minimum_required(VERSION 3.15)

# =============================================================================
# Unit Tests for BQ25792 Driver
# =============================================================================

# Create test executable
add_executable(test_bq25792
    ${CMAKE_CURRENT_SOURCE_DIR}/test_bq25792.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Src/bq25792.cpp
)

target_include_directories(test_bq25792
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/../library
        ${CMAKE_CURRENT_SOURCE_DIR}/../tests
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
