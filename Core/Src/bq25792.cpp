#include "bq25792.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace bq25792 {

namespace {

std::unexpected<std::error_code> fail(ErrorCode ec) {
    return std::unexpected(make_error_code(ec));
}

// Round a physical value to the nearest register step. Plain truncation
// programmed e.g. 4.2 V as 419 * 10 mV because 4.2f * 100 = 419.99997.
uint32_t to_code(float value, float step) {
    return static_cast<uint32_t>(std::lround(value / step));
}

uint16_t be16(const uint8_t* p) {
    return static_cast<uint16_t>((static_cast<uint16_t>(p[0]) << 8) | p[1]);
}

// IBUS / IBAT ADC results are 16-bit two's complement, 1 mA/LSB.
float signed_ma_to_a(const uint8_t* p) {
    return static_cast<float>(static_cast<int16_t>(be16(p))) / 1000.0f;
}

float mv_to_v(const uint8_t* p) {
    return static_cast<float>(be16(p)) / 1000.0f;
}

constexpr uint8_t kLastRegister = BQ25792_REG_PART_INFORMATION;

} // namespace

// =============================================================================
// BQ25792 Implementation
// =============================================================================

Charger::Charger(I2C_HandleTypeDef* hi2c)
    : hi2c_(hi2c), initialized_(false)
{
}

Charger::~Charger() = default;

std::error_code Charger::convert_hal_status(HAL_StatusTypeDef status) const {
    // Only called for failed transfers.
    const uint32_t err = hi2c_ ? HAL_I2C_GetError(hi2c_) : 0u;
    if (err & HAL_I2C_ERROR_AF) {
        return make_error_code(ErrorCode::Nack);
    }
    if (err & (HAL_I2C_ERROR_BERR | HAL_I2C_ERROR_ARLO)) {
        return make_error_code(ErrorCode::BusError);
    }
    switch (status) {
        case HAL_TIMEOUT: return make_error_code(ErrorCode::Timeout);
        case HAL_BUSY:    return make_error_code(ErrorCode::BusError);  // BUSY flag stuck: SDA/SCL held low
        default:          return make_error_code(ErrorCode::HALError);
    }
}

bq25792::expected<void> Charger::initialize() {
    if (!hi2c_) {
        return fail(ErrorCode::HALError);
    }

    // REG48: bits 5-3 PN_2:0 = 001b identifies BQ25792, bits 2-0 are DEV_REV.
    auto part_info = read_byte(BQ25792_REG_PART_INFORMATION);
    if (!part_info) {
        return std::unexpected(part_info.error());
    }
    if (((*part_info & BQ25792_PART_NUMBER_MASK) >> BQ25792_PART_NUMBER_SHIFT) != BQ25792_PART_NUMBER) {
        return fail(ErrorCode::InvalidParameter);
    }

    // First write moves the charger from default mode to host mode and starts
    // the watchdog (datasheet 8.4.1).
    if (auto r = kick_watchdog(); !r) {
        return r;
    }
    if (auto r = configure_adc(); !r) {
        return r;
    }

    initialized_ = true;
    return {};
}

bq25792::expected<void> Charger::reset() {
    // REG09 bit 6 REG_RST; read-modify-write so ITERM/reserved bits are kept.
    auto result = update_bits(BQ25792_REG_TERMINATION_CONTROL, BQ25792_REG_RST_MASK, BQ25792_REG_RST_MASK);
    if (!result) {
        return result;
    }
    initialized_ = false;
    return {};
}

// =============================================================================
// Low-level register access
//
// Every access is a single I2C transaction: write = S|addr W|reg|data..|P,
// read = S|addr W|reg|Sr|addr R|data..|P (datasheet Fig 8-20..8-23).
// =============================================================================

bq25792::expected<uint8_t> Charger::read_byte(uint8_t reg_addr) const {
    uint8_t data = 0;
    if (auto r = read_bytes(reg_addr, &data, 1); !r) {
        return std::unexpected(r.error());
    }
    return data;
}

bq25792::expected<void> Charger::read_bytes(uint8_t reg_addr, uint8_t* data, uint8_t size) const {
    if (!hi2c_) {
        return fail(ErrorCode::HALError);
    }
    if (!data || size == 0 || reg_addr > kLastRegister || reg_addr + size - 1 > kLastRegister) {
        return fail(ErrorCode::InvalidAddress);
    }

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(hi2c_, BQ25792_I2C_ADDRESS << 1, reg_addr,
                                                I2C_MEMADD_SIZE_8BIT, data, size, BQ25792_TIMEOUT_MS);
    if (status != HAL_OK) {
        return std::unexpected(convert_hal_status(status));
    }
    return {};
}

bq25792::expected<void> Charger::write_byte(uint8_t reg_addr, uint8_t data) const {
    return write_bytes(reg_addr, &data, 1);
}

bq25792::expected<void> Charger::write_bytes(uint8_t reg_addr, const uint8_t* data, uint8_t size) const {
    if (!hi2c_) {
        return fail(ErrorCode::HALError);
    }
    if (!data || size == 0 || reg_addr > kLastRegister || reg_addr + size - 1 > kLastRegister) {
        return fail(ErrorCode::InvalidAddress);
    }

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(hi2c_, BQ25792_I2C_ADDRESS << 1, reg_addr,
                                                 I2C_MEMADD_SIZE_8BIT, const_cast<uint8_t*>(data), size,
                                                 BQ25792_TIMEOUT_MS);
    if (status != HAL_OK) {
        return std::unexpected(convert_hal_status(status));
    }
    return {};
}

bq25792::expected<void> Charger::write_word(uint8_t reg_addr, uint16_t data) const {
    // 16-bit registers are big-endian: MSB at the lower address.
    const uint8_t buffer[2] = {
        static_cast<uint8_t>(data >> 8),
        static_cast<uint8_t>(data & 0xFF),
    };
    return write_bytes(reg_addr, buffer, 2);
}

bq25792::expected<void> Charger::update_bits(uint8_t reg_addr, uint8_t mask, uint8_t value) const {
    auto current = read_byte(reg_addr);
    if (!current) {
        return std::unexpected(current.error());
    }
    const uint8_t next = static_cast<uint8_t>((*current & ~mask) | (value & mask));
    return write_byte(reg_addr, next);
}

// =============================================================================
// Watchdog / ADC
// =============================================================================

bq25792::expected<void> Charger::kick_watchdog() {
    return update_bits(BQ25792_REG_CHARGER_CONTROL_1, BQ25792_WD_RST_MASK, BQ25792_WD_RST_MASK);
}

bq25792::expected<bool> Charger::is_watchdog_expired() const {
    auto status0 = read_byte(BQ25792_REG_CHARGER_STATUS_0);
    if (!status0) {
        return std::unexpected(status0.error());
    }
    return (*status0 & BQ25792_WD_STAT_MASK) != 0;
}

bq25792::expected<void> Charger::configure_adc() {
    // REG2F/REG30: 1 = disable. Enable everything we read; reserved bits are 0.
    if (auto r = write_byte(BQ25792_REG_ADC_DISABLE_0, 0x00); !r) {
        return r;
    }
    // D+/D- (REG30 bits 7:6, Table 8-57, txt:11279-11289) are not connected on the board:
    // disabling them shortens the continuous scan, so VAC1 (USB-C VBUS) refreshes faster.
    if (auto r = write_byte(BQ25792_REG_ADC_DISABLE_1, BQ25792_ADC_DISABLE_1_VALUE); !r) {
        return r;
    }
    // Continuous conversion, 14-bit (12 ms per channel), no averaging.
    return update_bits(BQ25792_REG_ADC_CONTROL,
                       BQ25792_ADC_EN_MASK | BQ25792_ADC_RATE_MASK | BQ25792_ADC_SAMPLE_MASK |
                           BQ25792_ADC_AVG_MASK | BQ25792_ADC_AVG_INIT_MASK,
                       BQ25792_ADC_CONTROL_VALUE);
}

bq25792::expected<AdcMeasurements> Charger::get_adc_measurements() const {
    // 0x31 IBUS, 0x33 IBAT, 0x35 VBUS, 0x37 VAC1, 0x39 VAC2, 0x3B VBAT, 0x3D VSYS
    uint8_t buf[14];
    if (auto r = read_bytes(BQ25792_REG_IBUS_ADC, buf, sizeof(buf)); !r) {
        return std::unexpected(r.error());
    }
    AdcMeasurements m{};
    m.ibus_a = signed_ma_to_a(&buf[0]);
    m.ibat_a = signed_ma_to_a(&buf[2]);
    m.vbus_v = mv_to_v(&buf[4]);
    m.vac1_v = mv_to_v(&buf[6]);
    m.vac2_v = mv_to_v(&buf[8]);
    m.vbat_v = mv_to_v(&buf[10]);
    m.vsys_v = mv_to_v(&buf[12]);
    return m;
}

// =============================================================================
// Configuration
// =============================================================================

bq25792::expected<float> Charger::get_vsys_min() const {
    auto result = read_byte(BQ25792_REG_MINIMAL_SYSTEM_VOLTAGE);
    if (!result) {
        return std::unexpected(result.error());
    }
    // VSYSMIN = code * 250 mV + 2500 mV (bits 5-0)
    const uint8_t code = *result & BQ25792_VSYS_MIN_MASK;
    return (static_cast<float>(code) * BQ25792_VSYS_MIN_STEP_SIZE + BQ25792_VSYS_MIN_FIXED_OFFSET) / 1000.0f;
}

bq25792::expected<void> Charger::set_vsys_min(float voltage) {
    if (!(voltage >= 2.5f && voltage <= 16.0f)) {
        return fail(ErrorCode::InvalidParameter);
    }
    const uint32_t code = to_code(voltage * 1000.0f - BQ25792_VSYS_MIN_FIXED_OFFSET, BQ25792_VSYS_MIN_STEP_SIZE);
    if (code > BQ25792_VSYS_MIN_MASK) {
        return fail(ErrorCode::InvalidParameter);
    }
    return update_bits(BQ25792_REG_MINIMAL_SYSTEM_VOLTAGE, BQ25792_VSYS_MIN_MASK, static_cast<uint8_t>(code));
}

bq25792::expected<float> Charger::get_charge_voltage_limit() const {
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_CHARGE_VOLTAGE_LIMIT, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    const uint16_t code = be16(buffer) & BQ25792_CHARGE_VOLTAGE_LIMIT_MASK;  // 11 bits, 10 mV
    return static_cast<float>(code) / 100.0f;
}

bq25792::expected<void> Charger::set_charge_voltage_limit(float voltage) {
    if (!(voltage >= 3.0f && voltage <= 18.8f)) {
        return fail(ErrorCode::InvalidParameter);
    }
    const uint32_t code = to_code(voltage, 0.01f);
    return write_word(BQ25792_REG_CHARGE_VOLTAGE_LIMIT, static_cast<uint16_t>(code & BQ25792_CHARGE_VOLTAGE_LIMIT_MASK));
}

bq25792::expected<float> Charger::get_charge_current_limit() const {
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_CHARGE_CURRENT_LIMIT, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    const uint16_t code = be16(buffer) & BQ25792_CHARGE_CURRENT_LIMIT_MASK;  // 9 bits, 10 mA
    return static_cast<float>(code) / 100.0f;
}

bq25792::expected<void> Charger::set_charge_current_limit(float current) {
    if (!(current >= 0.05f && current <= 5.0f)) {
        return fail(ErrorCode::InvalidParameter);
    }
    const uint32_t code = to_code(current, 0.01f);
    return write_word(BQ25792_REG_CHARGE_CURRENT_LIMIT, static_cast<uint16_t>(code & BQ25792_CHARGE_CURRENT_LIMIT_MASK));
}

bq25792::expected<float> Charger::get_input_voltage_limit() const {
    auto result = read_byte(BQ25792_REG_INPUT_VOLTAGE_LIMIT);
    if (!result) {
        return std::unexpected(result.error());
    }
    return static_cast<float>(*result) / 10.0f;  // 100 mV steps
}

bq25792::expected<void> Charger::set_input_voltage_limit(float voltage) {
    if (!(voltage >= 3.6f && voltage <= 22.0f)) {
        return fail(ErrorCode::InvalidParameter);
    }
    const uint32_t code = to_code(voltage, 0.1f);
    return write_byte(BQ25792_REG_INPUT_VOLTAGE_LIMIT, static_cast<uint8_t>(code));
}

bq25792::expected<float> Charger::get_input_current_limit() const {
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_INPUT_CURRENT_LIMIT, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    const uint16_t code = be16(buffer) & BQ25792_INPUT_CURRENT_LIMIT_MASK;  // 9 bits, 10 mA
    return static_cast<float>(code) / 100.0f;
}

bq25792::expected<void> Charger::set_input_current_limit(float current) {
    if (!(current >= 0.1f && current <= 3.3f)) {
        return fail(ErrorCode::InvalidParameter);
    }
    const uint32_t code = to_code(current, 0.01f);
    return write_word(BQ25792_REG_INPUT_CURRENT_LIMIT, static_cast<uint16_t>(code & BQ25792_INPUT_CURRENT_LIMIT_MASK));
}

bq25792::expected<std::pair<VbatLowV, float>> Charger::get_precharge_control() const {
    auto result = read_byte(BQ25792_REG_PRECHARGE_CONTROL);
    if (!result) {
        return std::unexpected(result.error());
    }
    const auto vbat_low = static_cast<VbatLowV>((*result & BQ25792_PRECHARGE_VBAT_LOW_MASK) >> 6);
    const float iprechg = static_cast<float>((*result & BQ25792_PRECHARGE_CURRENT_MASK) * BQ25792_PRECHARGE_CURRENT_STEP) / 1000.0f;
    return std::make_pair(vbat_low, iprechg);
}

bq25792::expected<void> Charger::set_precharge_control(VbatLowV vbat_low, float iprechrg) {
    if (static_cast<uint8_t>(vbat_low) > 3 || !(iprechrg >= 0.04f && iprechrg <= 2.0f)) {
        return fail(ErrorCode::InvalidParameter);
    }
    const uint32_t code = to_code(iprechrg * 1000.0f, BQ25792_PRECHARGE_CURRENT_STEP);
    if (code > BQ25792_PRECHARGE_CURRENT_MASK) {
        return fail(ErrorCode::InvalidParameter);
    }
    const uint8_t value = static_cast<uint8_t>((static_cast<uint8_t>(vbat_low) << 6) | code);
    return write_byte(BQ25792_REG_PRECHARGE_CONTROL, value);  // register is fully defined by the two fields
}

// =============================================================================
// Status
// =============================================================================

bq25792::expected<bool> Charger::is_plugged_in() const {
    auto result = read_byte(BQ25792_REG_CHARGER_STATUS_0);
    if (!result) {
        return std::unexpected(result.error());
    }
    return (*result & BQ25792_VBUS_PRESENT_STAT_MASK) != 0;
}

bq25792::expected<ChargeStatus> Charger::get_charge_status() const {
    auto result = read_byte(BQ25792_REG_CHARGER_STATUS_1);
    if (!result) {
        return std::unexpected(result.error());
    }
    return static_cast<ChargeStatus>((*result & BQ25792_CHG_STAT_MASK) >> BQ25792_CHG_STAT_SHIFT);
}

bq25792::expected<VBUSStatus> Charger::get_vbus_status() const {
    auto result = read_byte(BQ25792_REG_CHARGER_STATUS_1);
    if (!result) {
        return std::unexpected(result.error());
    }
    return static_cast<VBUSStatus>((*result & BQ25792_VBUS_STAT_MASK) >> BQ25792_VBUS_STAT_SHIFT);
}

bq25792::expected<bool> Charger::is_battery_present() const {
    auto result = read_byte(BQ25792_REG_CHARGER_STATUS_2);
    if (!result) {
        return std::unexpected(result.error());
    }
    return (*result & BQ25792_BATTERY_PRESENT_MASK) != 0;
}

bq25792::expected<bool> Charger::is_fault_present() const {
    uint8_t fault[2];
    if (auto r = read_bytes(BQ25792_REG_FAULT_STATUS_0, fault, 2); !r) {
        return std::unexpected(r.error());
    }
    return (fault[0] & BQ25792_FAULT0_MASK) != 0 || (fault[1] & BQ25792_FAULT1_MASK) != 0;
}

bq25792::expected<bool> Charger::is_enabled() const {
    auto result = read_byte(BQ25792_REG_CHARGER_CONTROL_0);
    if (!result) {
        return std::unexpected(result.error());
    }
    return (*result & BQ25792_EN_CHG_MASK) != 0;
}

bq25792::expected<void> Charger::set_enabled(bool enable) {
    return update_bits(BQ25792_REG_CHARGER_CONTROL_0, BQ25792_EN_CHG_MASK, enable ? BQ25792_EN_CHG_MASK : 0);
}

// =============================================================================
// ADC getters (read-only; ADC is configured once by configure_adc())
// =============================================================================

bq25792::expected<float> Charger::get_vbat() const {
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_VBAT_ADC, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    return mv_to_v(buffer);
}

bq25792::expected<float> Charger::get_ibus() const {
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_IBUS_ADC, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    return signed_ma_to_a(buffer);
}

bq25792::expected<float> Charger::get_vbus() const {
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_VBUS_ADC, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    return mv_to_v(buffer);
}

bq25792::expected<float> Charger::get_vsys() const {
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_VSYS_ADC, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    return mv_to_v(buffer);
}

bq25792::expected<float> Charger::get_ibat() const {
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_IBAT_ADC, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    return signed_ma_to_a(buffer);
}

bq25792::expected<uint8_t> Charger::get_device_info() const {
    return read_byte(BQ25792_REG_PART_INFORMATION);
}

// =============================================================================
// Input power mux (datasheet 8.3.5.4, REG12 DIS_ACDRV, REG13 EN_ACDRV1/2)
// =============================================================================

bq25792::expected<ChargerInputSource> Charger::get_input_source() const {
    auto ctrl3 = read_byte(BQ25792_REG_CHARGER_CONTROL_3);
    if (!ctrl3) {
        return std::unexpected(ctrl3.error());
    }
    auto ctrl4 = read_byte(BQ25792_REG_CHARGER_CONTROL_4);
    if (!ctrl4) {
        return std::unexpected(ctrl4.error());
    }
    if (*ctrl3 & BQ25792_DIS_ACDRV_MASK) {
        // Both paths forced off; there is no protocol value for that, the closest is "no mux path".
        return ChargerInputSource::USB_DPDM;
    }
    // The charger keeps EN_ACDRV1/2 updated with the active path, including after autonomous swaps.
    const bool ac1 = (*ctrl4 & BQ25792_EN_ACDRV1_MASK) != 0;
    const bool ac2 = (*ctrl4 & BQ25792_EN_ACDRV2_MASK) != 0;
    if (ac1 && !ac2) {
        return ChargerInputSource::AC1;
    }
    if (ac2 && !ac1) {
        return ChargerInputSource::AC2;
    }
    return ChargerInputSource::AUTO;
}

bq25792::expected<void> Charger::set_input_source(ChargerInputSource source) const {
    uint8_t acdrv = 0;
    switch (source) {
        case ChargerInputSource::AC1:  acdrv = BQ25792_EN_ACDRV1_MASK; break;
        case ChargerInputSource::AC2:  acdrv = BQ25792_EN_ACDRV2_MASK; break;
        case ChargerInputSource::AUTO: break;
        default:
            return fail(ErrorCode::InvalidParameter);
    }

    // DIS_ACDRV=0 allows the mux to turn a path on.
    if (auto r = update_bits(BQ25792_REG_CHARGER_CONTROL_3, BQ25792_DIS_ACDRV_MASK, 0); !r) {
        return r;
    }
    if (source == ChargerInputSource::AUTO) {
        // Leave EN_ACDRV1/2 to the charger: it enables the first valid input and fails over.
        return {};
    }
    // Both EN bits in one write to minimise the change-over time (datasheet 8.3.5.4).
    return update_bits(BQ25792_REG_CHARGER_CONTROL_4, BQ25792_EN_ACDRV1_MASK | BQ25792_EN_ACDRV2_MASK, acdrv);
}

// =============================================================================
// Input / power-path status
// =============================================================================

InputStatus Charger::decode_input_status(uint8_t reg1b) {
    // REG1B, Table 8-36 (txt:8405-8516)
    InputStatus s{};
    s.vbus_present = (reg1b & BQ25792_VBUS_PRESENT_STAT_MASK) != 0;
    s.ac1_present = (reg1b & BQ25792_AC1_PRESENT_STAT_MASK) != 0;
    s.ac2_present = (reg1b & BQ25792_AC2_PRESENT_STAT_MASK) != 0;
    s.power_good = (reg1b & BQ25792_PG_STAT_MASK) != 0;
    s.poor_source = (reg1b & BQ25792_POORSRC_STAT_MASK) != 0;
    s.wd_expired = (reg1b & BQ25792_WD_STAT_MASK) != 0;
    s.vindpm = (reg1b & BQ25792_VINDPM_STAT_MASK) != 0;
    s.iindpm = (reg1b & BQ25792_IINDPM_STAT_MASK) != 0;
    s.raw = reg1b;
    return s;
}

bq25792::expected<InputStatus> Charger::get_input_status() const {
    auto status0 = read_byte(BQ25792_REG_CHARGER_STATUS_0);
    if (!status0) {
        return std::unexpected(status0.error());
    }
    return decode_input_status(*status0);
}

bq25792::expected<InputVoltages> Charger::get_input_voltages() const {
    // 0x37 VAC1, 0x39 VAC2, 0x3B VBAT: 16-bit big-endian, 1 mV (txt:11575, 11641, 11708)
    uint8_t buf[6];
    if (auto r = read_bytes(BQ25792_REG_VAC1_ADC, buf, sizeof(buf)); !r) {
        return std::unexpected(r.error());
    }
    InputVoltages v{};
    v.vac1_mv = be16(&buf[0]);
    v.vac2_mv = be16(&buf[2]);
    v.vbat_mv = be16(&buf[4]);
    return v;
}

bq25792::expected<MuxState> Charger::get_mux_state() const {
    uint8_t buf[2];  // REG12, REG13
    if (auto r = read_bytes(BQ25792_REG_CHARGER_CONTROL_3, buf, sizeof(buf)); !r) {
        return std::unexpected(r.error());
    }
    MuxState m{};
    m.dis_acdrv = (buf[0] & BQ25792_DIS_ACDRV_MASK) != 0;   // txt:7507
    m.en_otg = (buf[0] & BQ25792_EN_OTG_MASK) != 0;         // txt:7516
    m.en_acdrv2 = (buf[1] & BQ25792_EN_ACDRV2_MASK) != 0;   // txt:7630
    m.en_acdrv1 = (buf[1] & BQ25792_EN_ACDRV1_MASK) != 0;   // txt:7647
    return m;
}

bq25792::expected<BatteryOtgStatus> Charger::get_battery_otg_status() const {
    // REG1D..REG21 only: REG22-REG27 are clear-on-read flags owned by other readers.
    uint8_t buf[5];
    if (auto r = read_bytes(BQ25792_REG_CHARGER_STATUS_2, buf, sizeof(buf)); !r) {
        return std::unexpected(r.error());
    }
    const uint8_t reg1d = buf[0];
    const uint8_t reg1f = buf[2];
    const uint8_t reg20 = buf[3];
    const uint8_t reg21 = buf[4];
    BatteryOtgStatus s{};
    s.battery_present = (reg1d & BQ25792_BATTERY_PRESENT_MASK) != 0;   // txt:8792
    s.vbat_otg_low = (reg1f & BQ25792_VBATOTG_LOW_STAT_MASK) != 0;     // txt:9006
    s.ts_cold = (reg1f & BQ25792_TS_COLD_STAT_MASK) != 0;              // txt:9023
    s.ts_hot = (reg1f & BQ25792_TS_HOT_STAT_MASK) != 0;                // txt:9071
    s.vac2_ovp = (reg20 & BQ25792_VAC2_OVP_STAT_MASK) != 0;            // txt:9223
    s.vac1_ovp = (reg20 & BQ25792_VAC1_OVP_STAT_MASK) != 0;            // txt:9231
    s.otg_ovp = (reg21 & BQ25792_OTG_OVP_STAT_MASK) != 0;              // txt:9323
    s.otg_uvp = (reg21 & BQ25792_OTG_UVP_STAT_MASK) != 0;              // txt:9337
    return s;
}

bq25792::expected<float> Charger::get_effective_input_current_limit() const {
    // REG19/1A ICO_ILIM_8:0, 10 mA, bits 15:9 reserved (Table 8-35, txt:8344)
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_ICO_CURRENT_LIMIT, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    const uint16_t code = be16(buffer) & BQ25792_ICO_ILIM_MASK;
    return static_cast<float>(code * BQ25792_ICO_ILIM_STEP_MA) / 1000.0f;
}

// =============================================================================
// Input current / detection policy bits
// =============================================================================

bq25792::expected<void> Charger::set_ext_ilim_enabled(bool enable) {
    // REG14 bit 1 EN_EXTILIM (Table 8-30, txt:7882)
    return update_bits(BQ25792_REG_CHARGER_CONTROL_5, BQ25792_EN_EXTILIM_MASK,
                       enable ? BQ25792_EN_EXTILIM_MASK : 0);
}

bq25792::expected<void> Charger::set_ibat_discharge_sense(bool enable) {
    // REG14 bit 5 EN_IBAT (Table 8-30, txt:7830)
    return update_bits(BQ25792_REG_CHARGER_CONTROL_5, BQ25792_EN_IBAT_MASK, enable ? BQ25792_EN_IBAT_MASK : 0);
}

bq25792::expected<void> Charger::set_auto_input_detection(bool enable) {
    // REG11 bit 6 AUTO_INDET_EN (txt:7365), bit 3 HVDCP_EN (txt:7400). HVDCP needs D+/D-,
    // so it goes off together with the detection; enabling leaves HVDCP_EN as it is.
    if (enable) {
        return update_bits(BQ25792_REG_CHARGER_CONTROL_2, BQ25792_AUTO_INDET_EN_MASK, BQ25792_AUTO_INDET_EN_MASK);
    }
    return update_bits(BQ25792_REG_CHARGER_CONTROL_2, BQ25792_AUTO_INDET_EN_MASK | BQ25792_HVDCP_EN_MASK, 0);
}

bq25792::expected<void> Charger::set_vac1_pulldown(bool enable) {
    // REG16 bit 2 VAC1_PD_EN (Table 8-32, txt:8047)
    return update_bits(BQ25792_REG_TEMPERATURE_CONTROL, BQ25792_VAC1_PD_EN_MASK, enable ? BQ25792_VAC1_PD_EN_MASK : 0);
}

// =============================================================================
// OTG
// =============================================================================

bq25792::expected<void> Charger::set_otg_voltage(float voltage) {
    // REG0B VOTG_10:0: 2800 mV offset, 10 mV/LSB, 2.8-22 V (Table 8-22, txt:6850-6866)
    if (!(voltage >= 2.8f && voltage <= 22.0f)) {
        return fail(ErrorCode::InvalidParameter);
    }
    const uint32_t code = to_code(voltage * 1000.0f - BQ25792_VOTG_OFFSET_MV, BQ25792_VOTG_STEP_MV);
    return write_word(BQ25792_REG_VOTG_REGULATION, static_cast<uint16_t>(code & BQ25792_VOTG_MASK));
}

bq25792::expected<float> Charger::get_otg_voltage() const {
    uint8_t buffer[2];
    if (auto r = read_bytes(BQ25792_REG_VOTG_REGULATION, buffer, 2); !r) {
        return std::unexpected(r.error());
    }
    const uint16_t code = be16(buffer) & BQ25792_VOTG_MASK;
    return static_cast<float>(BQ25792_VOTG_OFFSET_MV + code * BQ25792_VOTG_STEP_MV) / 1000.0f;
}

bq25792::expected<void> Charger::set_otg_current_limit(float current) {
    // REG0D IOTG_6:0: 40 mA/LSB, 0.12-3.32 A; bit 7 is PRECHG_TMR (Table 8-23, txt:6909-6935)
    if (!(current >= 0.12f && current <= 3.32f)) {
        return fail(ErrorCode::InvalidParameter);
    }
    const uint32_t code = to_code(current * 1000.0f, BQ25792_IOTG_STEP_MA);
    return update_bits(BQ25792_REG_IOTG_REGULATION, BQ25792_IOTG_MASK, static_cast<uint8_t>(code));
}

bq25792::expected<float> Charger::get_otg_current_limit() const {
    auto result = read_byte(BQ25792_REG_IOTG_REGULATION);
    if (!result) {
        return std::unexpected(result.error());
    }
    return static_cast<float>((*result & BQ25792_IOTG_MASK) * BQ25792_IOTG_STEP_MA) / 1000.0f;
}

bq25792::expected<void> Charger::set_otg_enabled(bool enable) {
    // REG12 bit 6 EN_OTG (Table 8-28, txt:7516); DIS_ACDRV and the other bits are kept.
    return update_bits(BQ25792_REG_CHARGER_CONTROL_3, BQ25792_EN_OTG_MASK, enable ? BQ25792_EN_OTG_MASK : 0);
}

bq25792::expected<bool> Charger::is_otg_enabled() const {
    auto result = read_byte(BQ25792_REG_CHARGER_CONTROL_3);
    if (!result) {
        return std::unexpected(result.error());
    }
    return (*result & BQ25792_EN_OTG_MASK) != 0;
}

// =============================================================================
// Burst Read/Write Implementation
// =============================================================================

bq25792::expected<void> Charger::read_block(uint8_t reg_addr, uint8_t* data, uint8_t size) const {
    // Multi-read auto-increments and may cross register boundaries (datasheet 8.5).
    return read_bytes(reg_addr, data, size);
}

bq25792::expected<void> Charger::write_block(uint8_t reg_addr, const uint8_t* data, uint8_t size) const {
    return write_bytes(reg_addr, data, size);
}

bq25792::expected<void> Charger::read_status_block(uint8_t buffer[13]) const {
    // Registers 0x1B-0x27: CHARGER_STATUS_0 through FAULT_FLAG_1
    return read_block(BQ25792_REG_CHARGER_STATUS_0, buffer, 13);
}

bq25792::expected<void> Charger::read_adc_block(uint8_t buffer[22]) const {
    // Registers 0x31-0x46: IBUS_ADC through DM_ADC
    return read_block(BQ25792_REG_IBUS_ADC, buffer, 22);
}

bq25792::expected<void> Charger::read_config_block(uint8_t buffer[21]) const {
    // Registers 0x00-0x14: MINIMAL_SYSTEM_VOLTAGE through CHARGER_CONTROL_5
    return read_block(BQ25792_REG_MINIMAL_SYSTEM_VOLTAGE, buffer, 21);
}

} // namespace bq25792
