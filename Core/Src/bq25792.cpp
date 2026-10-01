#include "bq25792.hpp"
#include <cstring>
#include <cmath>

namespace bq25792 {

// =============================================================================
// BQ25792 Implementation
// =============================================================================

Charger::Charger(I2C_HandleTypeDef* hi2c)
    : hi2c_(hi2c), initialized_(false)
{
}

Charger::~Charger() = default;

std::error_code Charger::convert_hal_status(HAL_StatusTypeDef status) {
    switch (status) {
        case HAL_OK:
            return make_error_code(ErrorCode::None);
        case HAL_TIMEOUT:
            return make_error_code(ErrorCode::Timeout);
        case HAL_ERROR:
            return make_error_code(ErrorCode::HALError);
        case HAL_BUSY:
            return make_error_code(ErrorCode::Timeout);
        default:
            return make_error_code(ErrorCode::HALError);
    }
}

bq25792::expected<void> Charger::initialize() {
    if (!hi2c_) {
        return make_error_code(ErrorCode::HALError);
    }

    // Verify device is present by reading part information
    auto part_info = read_byte(BQ25792_REG_PART_INFORMATION);
    if (!part_info) {
        return part_info.error();
    }

    if (part_info.value() != BQ25792_PART_ID) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    initialized_ = true;
    return {};
}

bq25792::expected<void> Charger::reset() {
    if (!hi2c_) {
        return make_error_code(ErrorCode::HALError);
    }

    // Write reset command to Termination Control register (0x09)
    // Bit 6 = 1 triggers software reset
    auto result = write_byte(BQ25792_REG_TERMINATION_CONTROL, 0x40);
    if (!result) {
        return result.error();
    }

    // Reset initialized flag (will be set again on next operation)
    initialized_ = false;
    return {};
}

bq25792::expected<uint8_t> Charger::read_byte(uint8_t reg_addr) const {
    if (!hi2c_) {
        return make_error_code(ErrorCode::HALError);
    }

    uint8_t data = 0;
    HAL_StatusTypeDef status;

    // First, send the register address to read from
    status = HAL_I2C_Master_Transmit(
        hi2c_,
        (BQ25792_I2C_ADDRESS << 1),
        &reg_addr,
        1,
        BQ25792_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return convert_hal_status(status);
    }

    // Then, restart and read the data
    status = HAL_I2C_Master_Receive(
        hi2c_,
        (BQ25792_I2C_ADDRESS << 1),
        &data,
        1,
        BQ25792_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return convert_hal_status(status);
    }

    return data;
}

bq25792::expected<void> Charger::read_bytes(uint8_t reg_addr, uint8_t* data, uint8_t size) const {
    if (!hi2c_) {
        return make_error_code(ErrorCode::HALError);
    }

    HAL_StatusTypeDef status;

    // First, send the register address to read from
    status = HAL_I2C_Master_Transmit(
        hi2c_,
        (BQ25792_I2C_ADDRESS << 1),
        &reg_addr,
        1,
        BQ25792_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return convert_hal_status(status);
    }

    // Then, restart and read the data
    status = HAL_I2C_Master_Receive(
        hi2c_,
        (BQ25792_I2C_ADDRESS << 1),
        data,
        size,
        BQ25792_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return convert_hal_status(status);
    }

    return {};
}

bq25792::expected<void> Charger::write_byte(uint8_t reg_addr, uint8_t data) const {
    if (!hi2c_) {
        return make_error_code(ErrorCode::HALError);
    }

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(
        hi2c_,
        (BQ25792_I2C_ADDRESS << 1),
        &reg_addr,
        1,
        BQ25792_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return convert_hal_status(status);
    }

    status = HAL_I2C_Master_Transmit(
        hi2c_,
        (BQ25792_I2C_ADDRESS << 1),
        &data,
        1,
        BQ25792_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return convert_hal_status(status);
    }

    return {};
}

bq25792::expected<void> Charger::write_bytes(uint8_t reg_addr, const uint8_t* data, uint8_t size) const {
    if (!hi2c_) {
        return make_error_code(ErrorCode::HALError);
    }

    // First send register address
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(
        hi2c_,
        (BQ25792_I2C_ADDRESS << 1),
        &reg_addr,
        1,
        BQ25792_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return convert_hal_status(status);
    }

    // Then send data
    status = HAL_I2C_Master_Transmit(
        hi2c_,
        (BQ25792_I2C_ADDRESS << 1),
        const_cast<uint8_t*>(data),
        size,
        BQ25792_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return convert_hal_status(status);
    }

    return {};
}

bq25792::expected<void> Charger::write_word(uint8_t reg_addr, uint16_t data) const {
    if (!hi2c_) {
        return make_error_code(ErrorCode::HALError);
    }

    // Big-endian: MSB first
    uint8_t buffer[3];
    buffer[0] = reg_addr;
    buffer[1] = static_cast<uint8_t>((data >> 8) & 0xFF);  // MSB
    buffer[2] = static_cast<uint8_t>(data & 0xFF);          // LSB

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(
        hi2c_,
        (BQ25792_I2C_ADDRESS << 1),
        buffer,
        3,
        BQ25792_TIMEOUT_MS
    );

    if (status != HAL_OK) {
        return convert_hal_status(status);
    }

    return {};
}

// =============================================================================
// Public Methods
// =============================================================================

bq25792::expected<float> Charger::get_vsys_min() const {
    auto result = read_byte(BQ25792_REG_MINIMAL_SYSTEM_VOLTAGE);
    if (!result) {
        return result.error();
    }

    uint8_t reg_value = result.value() & BQ25792_VSYS_MIN_MASK;
    float voltage = (reg_value * BQ25792_VSYS_MIN_STEP_SIZE) + BQ25792_VSYS_MIN_FIXED_OFFSET;
    return voltage / 1000.0f;  // Convert to volts
}

bq25792::expected<void> Charger::set_vsys_min(float voltage) {
    // Validate voltage (must be 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, or 10.5)
    const float valid_voltages[] = {3.5f, 4.5f, 5.5f, 6.5f, 7.5f, 8.5f, 9.5f, 10.5f};
    bool valid = false;
    for (float v : valid_voltages) {
        if (std::abs(v - voltage) < 0.01f) {
            valid = true;
            break;
        }
    }
    if (!valid) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    uint16_t reg_value = static_cast<uint16_t>((voltage * 1000.0f - BQ25792_VSYS_MIN_FIXED_OFFSET) / BQ25792_VSYS_MIN_STEP_SIZE);
    if (reg_value > BQ25792_VSYS_MIN_MASK) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    auto result = read_byte(BQ25792_REG_MINIMAL_SYSTEM_VOLTAGE);
    if (!result) {
        return result.error();
    }

    uint8_t current = result.value();
    uint8_t new_value = (current & ~BQ25792_VSYS_MIN_MASK) | (reg_value & BQ25792_VSYS_MIN_MASK);

    return write_byte(BQ25792_REG_MINIMAL_SYSTEM_VOLTAGE, new_value);
}

bq25792::expected<float> Charger::get_charge_voltage_limit() const {
    uint8_t buffer[2];
    auto result = read_bytes(BQ25792_REG_CHARGE_VOLTAGE_LIMIT, buffer, 2);
    if (!result) {
        return result.error();
    }

    uint16_t raw_value = ((buffer[0] & 0x07) << 8) | buffer[1];  // 11 bits
    return static_cast<float>(raw_value) / 100.0f;  // Convert to volts
}

bq25792::expected<void> Charger::set_charge_voltage_limit(float voltage) {
    // Validate voltage range (3.5V to 19.2V)
    if (voltage < 3.5f || voltage > 19.2f) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    uint16_t raw_value = static_cast<uint16_t>(voltage * 100.0f);
    return write_word(BQ25792_REG_CHARGE_VOLTAGE_LIMIT, raw_value);
}

bq25792::expected<float> Charger::get_charge_current_limit() const {
    uint8_t buffer[2];
    auto result = read_bytes(BQ25792_REG_CHARGE_CURRENT_LIMIT, buffer, 2);
    if (!result) {
        return result.error();
    }

    uint16_t raw_value = ((buffer[0] & 0x01) << 8) | buffer[1];  // 9 bits
    return static_cast<float>(raw_value) / 100.0f;  // Convert to amps
}

bq25792::expected<void> Charger::set_charge_current_limit(float current) {
    // Validate current range (0A to 12.79A)
    if (current < 0.0f || current > 12.79f) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    uint16_t raw_value = static_cast<uint16_t>(current * 100.0f);
    return write_word(BQ25792_REG_CHARGE_CURRENT_LIMIT, raw_value);
}

bq25792::expected<float> Charger::get_input_voltage_limit() const {
    auto result = read_byte(BQ25792_REG_INPUT_VOLTAGE_LIMIT);
    if (!result) {
        return result.error();
    }

    return static_cast<float>(result.value()) / 10.0f;  // Convert to volts
}

bq25792::expected<void> Charger::set_input_voltage_limit(float voltage) {
    // Validate voltage range (3.9V to 25.5V)
    if (voltage < 3.9f || voltage > 25.5f) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    uint8_t raw_value = static_cast<uint8_t>(voltage * 10.0f);
    return write_byte(BQ25792_REG_INPUT_VOLTAGE_LIMIT, raw_value);
}

bq25792::expected<float> Charger::get_input_current_limit() const {
    uint8_t buffer[2];
    auto result = read_bytes(BQ25792_REG_INPUT_CURRENT_LIMIT, buffer, 2);
    if (!result) {
        return result.error();
    }

    uint16_t raw_value = ((buffer[0] & 0x01) << 8) | buffer[1];  // 9 bits
    return static_cast<float>(raw_value) / 100.0f;  // Convert to amps
}

bq25792::expected<void> Charger::set_input_current_limit(float current) {
    // Validate current range (0A to 12.79A)
    if (current < 0.0f || current > 12.79f) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    uint16_t raw_value = static_cast<uint16_t>(current * 100.0f);
    return write_word(BQ25792_REG_INPUT_CURRENT_LIMIT, raw_value);
}

bq25792::expected<std::pair<float, float>> Charger::get_precharge_control() const {
    auto result = read_byte(BQ25792_REG_PRECHARGE_CONTROL);
    if (!result) {
        return result.error();
    }

    uint8_t data = result.value();
    uint8_t vbat_low_reg = (data & BQ25792_PRECHARGE_VBAT_LOW_MASK) >> 6;
    uint8_t iprechrg_reg = data & BQ25792_PRECHARGE_CURRENT_MASK;

    // Calculate voltages (2.5V, 2.8V, 3.1V, 3.4V for 2-bit value)
    const float vbat_low_values[] = {2.5f, 2.8f, 3.1f, 3.4f};
    float vbat_low = vbat_low_values[vbat_low_reg];
    float iprechrg = static_cast<float>(iprechrg_reg * BQ25792_PRECHARGE_CURRENT_STEP) / 1000.0f;

    return std::make_pair(vbat_low, iprechrg);
}

bq25792::expected<void> Charger::set_precharge_control(float vbat_low, float iprechrg) {
    // Validate vbat_low
    const float valid_vbat_low[] = {2.5f, 2.8f, 3.1f, 3.4f};
    uint8_t vbat_low_reg = 0;
    bool found = false;
    for (size_t i = 0; i < 4; i++) {
        if (std::abs(valid_vbat_low[i] - vbat_low) < 0.01f) {
            vbat_low_reg = static_cast<uint8_t>(i);
            found = true;
            break;
        }
    }
    if (!found) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    // Validate iprechrg (0.04A to 2.52A in 40mA steps)
    if (iprechrg < 0.04f || iprechrg > 2.52f) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    uint8_t iprechrg_reg = static_cast<uint8_t>(iprechrg * 1000.0f / BQ25792_PRECHARGE_CURRENT_STEP);
    if (iprechrg_reg > BQ25792_PRECHARGE_CURRENT_MASK) {
        return make_error_code(ErrorCode::InvalidParameter);
    }

    uint8_t data = (vbat_low_reg << 6) | iprechrg_reg;
    return write_byte(BQ25792_REG_PRECHARGE_CONTROL, data);
}

bq25792::expected<bool> Charger::is_plugged_in() const {
    auto result = read_byte(BQ25792_REG_CHARGER_STATUS_0);
    if (!result) {
        return result.error();
    }

    return (result.value() & BQ25792_VBUS_PRESENT_STAT_MASK) != 0;
}

bq25792::expected<ChargeStatus> Charger::get_charge_status() const {
    auto result = read_byte(BQ25792_REG_CHARGER_STATUS_1);
    if (!result) {
        return result.error();
    }

    uint8_t data = result.value();
    uint8_t charge_stat = (data & BQ25792_CHG_STAT_MASK) >> BQ25792_CHG_STAT_SHIFT;

    return static_cast<ChargeStatus>(charge_stat);
}

bq25792::expected<VBUSStatus> Charger::get_vbus_status() const {
    auto result = read_byte(BQ25792_REG_CHARGER_STATUS_1);
    if (!result) {
        return result.error();
    }

    uint8_t data = result.value();
    uint8_t vbus_stat = (data & BQ25792_VBUS_STAT_MASK) >> BQ25792_VBUS_STAT_SHIFT;

    return static_cast<VBUSStatus>(vbus_stat);
}

bq25792::expected<bool> Charger::is_battery_present() const {
    auto result = read_byte(BQ25792_REG_CHARGER_STATUS_2);
    if (!result) {
        return result.error();
    }

    return (result.value() & BQ25792_BATTERY_PRESENT_MASK) != 0;
}

bq25792::expected<bool> Charger::is_fault_present() const {
    auto fault0 = read_byte(BQ25792_REG_FAULT_STATUS_0);
    auto fault1 = read_byte(BQ25792_REG_FAULT_STATUS_1);

    if (!fault0 || !fault1) {
        if (!fault0) return fault0.error();
        return fault1.error();
    }

    return (fault0.value() | fault1.value()) != 0;
}

bq25792::expected<float> Charger::get_vbat() const {
    // Enable VBAT ADC
    auto result = write_byte(BQ25792_REG_ADC_DISABLE_0, 0b10001111);
    if (!result) {
        return result.error();
    }

    result = write_byte(BQ25792_REG_ADC_DISABLE_1, 0b11111111);
    if (!result) {
        return result.error();
    }

    result = write_byte(BQ25792_REG_ADC_CONTROL, 0b10001100);
    if (!result) {
        return result.error();
    }

    uint8_t buffer[2];
    result = read_bytes(BQ25792_REG_VBAT_ADC, buffer, 2);
    if (!result) {
        return result.error();
    }

    uint16_t raw_value = (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1];
    return static_cast<float>(raw_value) / 1000.0f;  // Convert to volts
}

bq25792::expected<float> Charger::get_ibus() const {
    // Enable IBUS ADC
    auto result = write_byte(BQ25792_REG_ADC_CONTROL, 0b10001100);
    if (!result) {
        return result.error();
    }

    uint8_t buffer[2];
    result = read_bytes(BQ25792_REG_IBUS_ADC, buffer, 2);
    if (!result) {
        return result.error();
    }

    int16_t raw_value = static_cast<int16_t>((static_cast<uint16_t>(buffer[0]) << 8) | buffer[1]);

    // Convert from two's complement to float
    float value;
    if (raw_value & 0x8000) {
        // Negative value
        value = -static_cast<float>(0x10000 - raw_value);
    } else {
        value = static_cast<float>(raw_value);
    }

    // IBUS ADC resolution is 16-bit with 10mV/100mV scale factor
    // For BQ25792, IBUS ADC code = (IBUS * 100) / 100 = IBUS * 1 (in mA)
    return value / 1000.0f;  // Convert to amps
}

bq25792::expected<uint8_t> Charger::get_device_info() const {
    return read_byte(BQ25792_REG_PART_INFORMATION);
}

} // namespace bq25792
