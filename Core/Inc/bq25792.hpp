#ifndef __BQ25792_HPP
#define __BQ25792_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include <string>
#include <cstring>
#include <system_error>
#include <utility>

// std::expected alias (driver is built as C++23)
#include "bq25792_expected.hpp"

// HAL includes (skip in unit test mode)
#ifndef BQ25792_UNIT_TEST
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_i2c.h"
#else
// Unit test mode: include mock HAL for type definitions
#include "mock_hal.h"
#endif

// BQ25792 configuration
#ifndef BQ25792_I2C_ADDRESS
#define BQ25792_I2C_ADDRESS 0x6B
#endif

#ifndef BQ25792_TIMEOUT_MS
#define BQ25792_TIMEOUT_MS 20
#endif

// ADC configuration
#ifndef BQ25792_IBUS_ADC_RESOLUTION
#define BQ25792_IBUS_ADC_RESOLUTION 16
#endif

#ifndef BQ25792_VBAT_ADC_RESOLUTION
#define BQ25792_VBAT_ADC_RESOLUTION 16
#endif

#ifndef BQ25792_VSYS_ADC_RESOLUTION
#define BQ25792_VSYS_ADC_RESOLUTION 16
#endif

// Register addresses
constexpr uint8_t BQ25792_REG_MINIMAL_SYSTEM_VOLTAGE   = 0x00;  // VSYS_MIN
constexpr uint8_t BQ25792_REG_CHARGE_VOLTAGE_LIMIT     = 0x01;  // Charge Voltage Limit
constexpr uint8_t BQ25792_REG_CHARGE_CURRENT_LIMIT     = 0x03;  // Charge Current Limit
constexpr uint8_t BQ25792_REG_INPUT_VOLTAGE_LIMIT      = 0x05;  // Input Voltage Limit
constexpr uint8_t BQ25792_REG_INPUT_CURRENT_LIMIT      = 0x06;  // Input Current Limit
constexpr uint8_t BQ25792_REG_PRECHARGE_CONTROL        = 0x08;  // Precharge Control
constexpr uint8_t BQ25792_REG_TERMINATION_CONTROL      = 0x09;  // Termination Control
constexpr uint8_t BQ25792_REG_RECHARGE_CONTROL         = 0x0A;  // Recharge Control
constexpr uint8_t BQ25792_REG_VOTG_REGULATION          = 0x0B;  // VOtg regulation
constexpr uint8_t BQ25792_REG_IOTG_REGULATION          = 0x0D;  // IOtg regulation
constexpr uint8_t BQ25792_REG_TIMER_CONTROL            = 0x0E;  // Timer Control
constexpr uint8_t BQ25792_REG_CHARGER_CONTROL_0        = 0x0F;  // Charger Control 0
constexpr uint8_t BQ25792_REG_CHARGER_CONTROL_1        = 0x10;  // Charger Control 1
constexpr uint8_t BQ25792_REG_CHARGER_CONTROL_2        = 0x11;  // Charger Control 2
constexpr uint8_t BQ25792_REG_CHARGER_CONTROL_3        = 0x12;  // Charger Control 3
constexpr uint8_t BQ25792_REG_CHARGER_CONTROL_4        = 0x13;  // Charger Control 4
constexpr uint8_t BQ25792_REG_CHARGER_CONTROL_5        = 0x14;  // Charger Control 5
constexpr uint8_t BQ25792_REG_RESERVED_15              = 0x15;  // Reserved
constexpr uint8_t BQ25792_REG_TEMPERATURE_CONTROL      = 0x16;  // Temperature Control
constexpr uint8_t BQ25792_REG_NTC_CONTROL_0            = 0x17;  // NTC Control 0
constexpr uint8_t BQ25792_REG_NTC_CONTROL_1            = 0x18;  // NTC Control 1
constexpr uint8_t BQ25792_REG_ICO_CURRENT_LIMIT        = 0x19;  // ICO Current Limit
constexpr uint8_t BQ25792_REG_CHARGER_STATUS_0         = 0x1B;  // Charger Status 0
constexpr uint8_t BQ25792_REG_CHARGER_STATUS_1         = 0x1C;  // Charger Status 1
constexpr uint8_t BQ25792_REG_CHARGER_STATUS_2         = 0x1D;  // Charger Status 2
constexpr uint8_t BQ25792_REG_CHARGER_STATUS_3         = 0x1E;  // Charger Status 3
constexpr uint8_t BQ25792_REG_CHARGER_STATUS_4         = 0x1F;  // Charger Status 4
constexpr uint8_t BQ25792_REG_FAULT_STATUS_0           = 0x20;  // Fault Status 0
constexpr uint8_t BQ25792_REG_FAULT_STATUS_1           = 0x21;  // Fault Status 1
constexpr uint8_t BQ25792_REG_CHARGER_FLAG_0           = 0x22;  // Charger Flag 0
constexpr uint8_t BQ25792_REG_CHARGER_FLAG_1           = 0x23;  // Charger Flag 1
constexpr uint8_t BQ25792_REG_CHARGER_FLAG_2           = 0x24;  // Charger Flag 2
constexpr uint8_t BQ25792_REG_CHARGER_FLAG_3           = 0x25;  // Charger Flag 3
constexpr uint8_t BQ25792_REG_FAULT_FLAG_0             = 0x26;  // Fault Flag 0
constexpr uint8_t BQ25792_REG_FAULT_FLAG_1             = 0x27;  // Fault Flag 1
constexpr uint8_t BQ25792_REG_CHARGER_MASK_0           = 0x28;  // Charger Mask 0
constexpr uint8_t BQ25792_REG_CHARGER_MASK_1           = 0x29;  // Charger Mask 1
constexpr uint8_t BQ25792_REG_CHARGER_MASK_2           = 0x2A;  // Charger Mask 2
constexpr uint8_t BQ25792_REG_CHARGER_MASK_3           = 0x2B;  // Charger Mask 3
constexpr uint8_t BQ25792_REG_FAULT_MASK_0             = 0x2C;  // Fault Mask 0
constexpr uint8_t BQ25792_REG_FAULT_MASK_1             = 0x2D;  // Fault Mask 1
constexpr uint8_t BQ25792_REG_ADC_CONTROL              = 0x2E;  // ADC Control
constexpr uint8_t BQ25792_REG_ADC_DISABLE_0            = 0x2F;  // ADC Function Disable 0
constexpr uint8_t BQ25792_REG_ADC_DISABLE_1            = 0x30;  // ADC Function Disable 1
constexpr uint8_t BQ25792_REG_IBUS_ADC                 = 0x31;  // IBUS ADC
constexpr uint8_t BQ25792_REG_IBAT_ADC                 = 0x33;  // IBAT ADC
constexpr uint8_t BQ25792_REG_VBUS_ADC                 = 0x35;  // VBUS ADC
constexpr uint8_t BQ25792_REG_VAC1_ADC                 = 0x37;  // VAC1 ADC
constexpr uint8_t BQ25792_REG_VAC2_ADC                 = 0x39;  // VAC2 ADC
constexpr uint8_t BQ25792_REG_VBAT_ADC                 = 0x3B;  // VBAT ADC
constexpr uint8_t BQ25792_REG_VSYS_ADC                 = 0x3D;  // VSYS ADC
constexpr uint8_t BQ25792_REG_TS_ADC                   = 0x3F;  // TS ADC
constexpr uint8_t BQ25792_REG_TDIE_ADC                 = 0x41;  // TDIE ADC
constexpr uint8_t BQ25792_REG_DP_ADC                   = 0x43;  // D+ ADC
constexpr uint8_t BQ25792_REG_DM_ADC                   = 0x45;  // D- ADC
constexpr uint8_t BQ25792_REG_DPDM_DRIVER              = 0x47;  // DPDM Driver
constexpr uint8_t BQ25792_REG_PART_INFORMATION         = 0x48;  // Part Information

// VSYS_MIN configuration
constexpr uint16_t BQ25792_VSYS_MIN_FIXED_OFFSET       = 2500;
constexpr uint16_t BQ25792_VSYS_MIN_STEP_SIZE          = 250;
constexpr uint16_t BQ25792_VSYS_MIN_MASK               = 0x3F;

// Charge Voltage Limit configuration
constexpr uint16_t BQ25792_CHARGE_VOLTAGE_LIMIT_STEP   = 10;  // 10mV steps
constexpr uint16_t BQ25792_CHARGE_VOLTAGE_LIMIT_MASK   = 0x07FF;  // 11 bits

// Charge Current Limit configuration
constexpr uint16_t BQ25792_CHARGE_CURRENT_LIMIT_STEP   = 10;  // 10mA steps
constexpr uint16_t BQ25792_CHARGE_CURRENT_LIMIT_MASK   = 0x01FF;  // 9 bits

// Input Voltage Limit configuration
constexpr uint16_t BQ25792_INPUT_VOLTAGE_LIMIT_STEP    = 100;  // 100mV steps
constexpr uint8_t  BQ25792_INPUT_VOLTAGE_LIMIT_MASK    = 0xFF;  // 8 bits

// Input Current Limit configuration
constexpr uint16_t BQ25792_INPUT_CURRENT_LIMIT_STEP    = 10;  // 10mA steps
constexpr uint16_t BQ25792_INPUT_CURRENT_LIMIT_MASK    = 0x01FF;  // 9 bits

// Precharge current configuration
constexpr uint16_t BQ25792_PRECHARGE_CURRENT_STEP      = 40;  // 40mA steps
constexpr uint8_t  BQ25792_PRECHARGE_CURRENT_MASK      = 0x3F;  // 6 bits
constexpr uint8_t  BQ25792_PRECHARGE_VBAT_LOW_MASK     = 0xC0;  // 2 bits (bits 6-7)

// Charge Status 1 - CHG_STAT (bits 7-5)
constexpr uint8_t BQ25792_CHG_STAT_MASK                = 0xE0;  // bits 7-5
constexpr uint8_t BQ25792_CHG_STAT_SHIFT               = 5;

// Charge Status 1 - VBUS_STAT (bits 4-1)
constexpr uint8_t BQ25792_VBUS_STAT_MASK               = 0x1E;  // bits 4-1
constexpr uint8_t BQ25792_VBUS_STAT_SHIFT              = 1;

// Charger Status 0 - VBUS_PRESENT_STAT (bit 0)
constexpr uint8_t BQ25792_VBUS_PRESENT_STAT_MASK       = 0x01;

// Charger Status 2 - Battery Present (bit 0)
constexpr uint8_t BQ25792_BATTERY_PRESENT_MASK         = 0x01;

// Part Information
constexpr uint8_t BQ25792_PART_NUMBER_MASK            = 0x38;  // REG48 bits 5-3: PN_2:0
constexpr uint8_t BQ25792_PART_NUMBER_SHIFT           = 3;
constexpr uint8_t BQ25792_PART_NUMBER                 = 0x01;  // PN = 001b -> BQ25792 (REG48, Table 8-70)

// ============================================================================= 
// Register Bit Masks and Constants
// =============================================================================

// Charger Control 0 (REG0F) - EN_CHG bit
constexpr uint8_t BQ25792_EN_CHG_MASK                  = 0x20;  // Bit 5: EN_CHG (bit 0 is reserved)

// ADC Function Disable 0 (REG2F) - Individual ADC disable bits
constexpr uint8_t BQ25792_IBUS_ADC_DIS_MASK            = 0x80;  // Bit 7: IBUS ADC Disable
constexpr uint8_t BQ25792_IBAT_ADC_DIS_MASK            = 0x40;  // Bit 6: IBAT ADC Disable
constexpr uint8_t BQ25792_VBUS_ADC_DIS_MASK            = 0x20;  // Bit 5: VBUS ADC Disable
constexpr uint8_t BQ25792_VBAT_ADC_DIS_MASK            = 0x10;  // Bit 4: VBAT ADC Disable
constexpr uint8_t BQ25792_VSYS_ADC_DIS_MASK            = 0x08;  // Bit 3: VSYS ADC Disable
constexpr uint8_t BQ25792_TS_ADC_DIS_MASK              = 0x04;  // Bit 2: TS ADC Disable
constexpr uint8_t BQ25792_TDIE_ADC_DIS_MASK            = 0x02;  // Bit 1: TDIE ADC Disable

// ADC Function Disable 1 (REG30) - Individual ADC disable bits
constexpr uint8_t BQ25792_DP_ADC_DIS_MASK              = 0x80;  // Bit 7: D+ ADC Disable
constexpr uint8_t BQ25792_DM_ADC_DIS_MASK              = 0x40;  // Bit 6: D- ADC Disable
constexpr uint8_t BQ25792_VAC2_ADC_DIS_MASK            = 0x20;  // Bit 5: VAC2 ADC Disable
constexpr uint8_t BQ25792_VAC1_ADC_DIS_MASK            = 0x10;  // Bit 4: VAC1 ADC Disable

// ADC Control (REG2E) - ADC configuration bits
constexpr uint8_t BQ25792_ADC_EN_MASK                  = 0x80;  // Bit 7: ADC Enable
constexpr uint8_t BQ25792_ADC_RATE_MASK                = 0x40;  // Bit 6: ADC Conversion Rate
constexpr uint8_t BQ25792_ADC_SAMPLE_MASK              = 0x30;  // Bits 5-4: ADC Sample Speed
constexpr uint8_t BQ25792_ADC_AVG_MASK                 = 0x08;  // Bit 3: ADC Average
constexpr uint8_t BQ25792_ADC_AVG_INIT_MASK            = 0x04;  // Bit 2: ADC Average Initial

// Input power mux (dual ACFET-RBFET, datasheet 8.3.5.4)
constexpr uint8_t BQ25792_EN_ACDRV2_MASK               = 0x80;  // REG13 bit 7: EN_ACDRV2
constexpr uint8_t BQ25792_EN_ACDRV1_MASK               = 0x40;  // REG13 bit 6: EN_ACDRV1
constexpr uint8_t BQ25792_DIS_ACDRV_MASK               = 0x80;  // REG12 bit 7: DIS_ACDRV (force both off)
constexpr uint8_t BQ25792_ACRB2_STAT_MASK              = 0x80;  // REG1E bit 7: ACFET2-RBFET2 present
constexpr uint8_t BQ25792_ACRB1_STAT_MASK              = 0x40;  // REG1E bit 6: ACFET1-RBFET1 present

// Watchdog (REG10)
constexpr uint8_t BQ25792_WD_RST_MASK                  = 0x08;  // REG10 bit 3: WD_RST (self clearing)
constexpr uint8_t BQ25792_WATCHDOG_MASK                = 0x07;  // REG10 bits 2-0: WATCHDOG_2:0
constexpr uint8_t BQ25792_WD_STAT_MASK                 = 0x20;  // REG1B bit 5: WD_STAT (1 = expired, default mode)

// Termination control (REG09)
constexpr uint8_t BQ25792_REG_RST_MASK                 = 0x40;  // REG09 bit 6: REG_RST

// ADC control (REG2E) value used by the driver:
// ADC_EN=1, ADC_RATE=0 (continuous), ADC_SAMPLE=01 (14-bit, 12 ms/channel), ADC_AVG=0
constexpr uint8_t BQ25792_ADC_SAMPLE_14BIT             = 0x10;
constexpr uint8_t BQ25792_ADC_CONTROL_VALUE            = BQ25792_ADC_EN_MASK | BQ25792_ADC_SAMPLE_14BIT;

// Fault status bits that are real faults (REG20 bit 7 IBAT_REG_STAT is a regulation status)
constexpr uint8_t BQ25792_FAULT0_MASK                  = 0x7F;  // VBUS/VBAT OVP, IBUS/IBAT/CONV OCP, VAC2/VAC1 OVP
constexpr uint8_t BQ25792_FAULT1_MASK                  = 0xF4;  // VSYS_SHORT, VSYS_OVP, OTG_OVP, OTG_UVP, TSHUT

namespace bq25792 {

/**
 * @brief Error codes for BQ25792 operations
 */
enum class ErrorCode : uint8_t {
    None = 0,
    HALError,
    Timeout,
    I2CError,
    InvalidAddress,
    InvalidParameter,
    NotInitialized,
    BatteryNotPresent,
    FaultDetected,
    // I2C failure classes (single-master bus, so ARLO also means a bus fault):
    Nack,             // device did not acknowledge (absent, busy, or rejected the command)
    BusError
};

/**
 * @brief Custom error_category implementation for std::expected
 */
class BQ25792ErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "bq25792";
    }

    std::string message(int ev) const override {
        switch (static_cast<ErrorCode>(ev)) {
            case ErrorCode::None: return "No error";
            case ErrorCode::HALError: return "HAL error";
            case ErrorCode::Timeout: return "Operation timeout";
            case ErrorCode::I2CError: return "I2C communication error";
            case ErrorCode::InvalidAddress: return "Invalid register address";
            case ErrorCode::InvalidParameter: return "Invalid parameter";
            case ErrorCode::NotInitialized: return "BQ25792 not initialized";
            case ErrorCode::BatteryNotPresent: return "Battery not present";
            case ErrorCode::FaultDetected: return "Fault condition detected";
            case ErrorCode::Nack: return "I2C NACK (device not responding)";
            case ErrorCode::BusError: return "I2C bus error";
            default: return "Unknown error";
        }
    }
};

/**
 * @brief Get the BQ25792 error category instance
 */
inline const BQ25792ErrorCategory& get_error_category() {
    static BQ25792ErrorCategory instance;
    return instance;
}

/**
 * @brief Create an error code from ErrorCode enum
 */
inline std::error_code make_error_code(ErrorCode ec) {
    return std::error_code(static_cast<int>(ec), get_error_category());
}

/**
 * @brief Battery Charge Status
 */
enum class ChargeStatus : uint8_t {
    NotCharging = 0x0,
    TrickleCharge = 0x1,
    Precharge = 0x2,
    FastCharge = 0x3,
    TaperCharge = 0x4,
    Reserved = 0x5,
    TopOff = 0x6,
    ChargeDone = 0x7
};

/**
 * @brief VBUS Status
 */
enum class VBUSStatus : uint8_t {
    NoInput = 0x0,
    USBSDP500mA = 0x1,
    USBCDP1500mA = 0x2,
    USBDCP3250mA = 0x3,
    AdjustableHigh = 0x4,
    UnknownAdapter = 0x5,
    NonStandardAdapter = 0x6,
    OTGMode = 0x7,
    NotQualifiedAdapter = 0x8,
    PoweredFromVBUS = 0xB
};

/**
 * @brief Charger input power-mux selection (dual ACFET-RBFET, datasheet 8.3.5.4)
 *
 * AC1  - only the VAC1 path (EN_ACDRV1=1, EN_ACDRV2=0)
 * AC2  - only the VAC2 path (EN_ACDRV1=0, EN_ACDRV2=1)
 * AUTO - DIS_ACDRV=0, charger turns on whichever path got a valid input first
 *        and fails over automatically
 * USB_DPDM - not a power-mux setting on this part; rejected by set_input_source()
 */
enum class ChargerInputSource : uint8_t {
    USB_DPDM = 0x0,
    AC1 = 0x1,
    AC2 = 0x2,
    AUTO = 0x3
};

/**
 * @brief Precharge-to-fastcharge threshold (REG08 VBAT_LOWV_1:0), ratio of VREG
 */
enum class VbatLowV : uint8_t {
    Pct15   = 0x0,  // 15% * VREG
    Pct62_2 = 0x1,  // 62.2% * VREG
    Pct66_7 = 0x2,  // 66.7% * VREG
    Pct71_4 = 0x3   // 71.4% * VREG (POR default)
};

/**
 * @brief Snapshot of the ADC result registers (one burst read, 0x31-0x3E)
 */
struct AdcMeasurements {
    float ibus_a;   // signed, + = into charger
    float ibat_a;   // signed, + = charging, - = discharging
    float vbus_v;
    float vac1_v;
    float vac2_v;
    float vbat_v;
    float vsys_v;
};

/**
 * @brief BQ25792 Charger Driver Class
 *
 * This class provides a modern C++ interface for the TI BQ25792 Smart Battery Manager.
 * It manages I2C handle internally and provides methods for all charger operations
 * with proper error handling via std::expected.
 */
class Charger {
public:
    /**
     * @brief Construct BQ25792 charger driver
     * @param hi2c Pointer to I2C handle
     */
    explicit Charger(I2C_HandleTypeDef* hi2c);

    /**
     * @brief Destroy BQ25792 charger driver
     */
    ~Charger();

    /**
     * @brief Initialize the charger driver
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> initialize();

    /**
     * @brief Reset the charger
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> reset();

    /**
     * @brief Get minimal system voltage limit
     * @return Expected voltage in volts (e.g., 3.5, 7.0, 9.0, 12.0)
     */
    bq25792::expected<float> get_vsys_min() const;

    /**
     * @brief Set minimal system voltage limit
     * @param voltage Voltage in volts (range: 2.5V to 16.0V in 250mV steps)
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> set_vsys_min(float voltage);

    /**
     * @brief Get charge voltage limit
     * @return Expected voltage in volts
     */
    bq25792::expected<float> get_charge_voltage_limit() const;

    /**
     * @brief Set charge voltage limit
     * @param voltage Voltage in volts (range: 3.0V to 18.8V)
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> set_charge_voltage_limit(float voltage);

    /**
     * @brief Get charge current limit
     * @return Expected current in amps
     */
    bq25792::expected<float> get_charge_current_limit() const;

    /**
     * @brief Set charge current limit
     * @param current Current in amps (range: 0.05A to 5.0A)
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> set_charge_current_limit(float current);

    /**
     * @brief Get input voltage limit
     * @return Expected voltage in volts
     */
    bq25792::expected<float> get_input_voltage_limit() const;

    /**
     * @brief Set input voltage limit
     * @param voltage Voltage in volts (range: 3.6V to 22.0V)
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> set_input_voltage_limit(float voltage);

    /**
     * @brief Get input current limit
     * @return Expected current in amps
     */
    bq25792::expected<float> get_input_current_limit() const;

    /**
     * @brief Set input current limit
     * @param current Current in amps (range: 0.1A to 3.3A)
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> set_input_current_limit(float current);

    /**
     * @brief Get precharge control settings (REG08)
     * @return pair {VBAT_LOWV threshold (ratio of VREG), precharge current in amps}
     */
    bq25792::expected<std::pair<VbatLowV, float>> get_precharge_control() const;

    /**
     * @brief Set precharge control settings (REG08)
     * @param vbat_low Precharge-to-fastcharge threshold as ratio of VREG
     * @param iprechrg Precharge current in amps (0.04 - 2.0 A, 40 mA steps)
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> set_precharge_control(VbatLowV vbat_low, float iprechrg);

    /**
     * @brief Check if input is plugged in (VBUS present)
     * @return Expected boolean - true if VBUS present
     */
    bq25792::expected<bool> is_plugged_in() const;

    /**
     * @brief Get charge status
     * @return Expected ChargeStatus enum
     */
    bq25792::expected<ChargeStatus> get_charge_status() const;

    /**
     * @brief Get VBUS status
     * @return Expected VBUSStatus enum
     */
    bq25792::expected<VBUSStatus> get_vbus_status() const;

    /**
     * @brief Check if battery is present
     * @return Expected boolean - true if battery present
     */
    bq25792::expected<bool> is_battery_present() const;

    /**
     * @brief Check if any fault is present
     * @return Expected boolean - true if fault detected
     */
    bq25792::expected<bool> is_fault_present() const;

    /**
     * @brief Check if charger is enabled
     * @return Expected boolean - true if enabled
     */
    bq25792::expected<bool> is_enabled() const;

    /**
     * @brief Enable/disable the charger
     * @param enable true to enable, false to disable
     * @return Expected void on success
     */
    bq25792::expected<void> set_enabled(bool enable);

    /**
     * @brief Get battery voltage
     * @return Expected voltage in volts
     */
    bq25792::expected<float> get_vbat() const;

    /**
     * @brief Get IBUS current
     * @return Expected current in amps (positive for charge, negative for discharge)
     */
    bq25792::expected<float> get_ibus() const;

    /**
     * @brief Get VBUS voltage
     * @return Expected voltage in volts
     */
    bq25792::expected<float> get_vbus() const;

    /**
     * @brief Get VSYS voltage
     * @return Expected voltage in volts
     */
    bq25792::expected<float> get_vsys() const;

    /**
     * @brief Get IBAT current
     * @return Expected current in amps
     */
    bq25792::expected<float> get_ibat() const;

    /**
     * @brief Read all main ADC channels in one burst (consistent snapshot)
     *
     * The ADC is configured for continuous conversion by initialize() /
     * configure_adc(); this only reads the result registers.
     */
    bq25792::expected<AdcMeasurements> get_adc_measurements() const;

    /**
     * @brief (Re)configure the ADC: continuous, 14-bit, all main channels enabled
     *
     * ADC_EN is reset by the watchdog, so call this again after a watchdog expiry.
     */
    bq25792::expected<void> configure_adc();

    /**
     * @brief Reset the I2C watchdog timer (REG10 WD_RST)
     *
     * Must be called more often than the watchdog period (40 s default),
     * otherwise ICHG, EN_CHG, IPRECHG, ITERM and ADC_EN revert to defaults.
     */
    bq25792::expected<void> kick_watchdog();

    /**
     * @brief Check whether the watchdog has expired (REG1B WD_STAT)
     *
     * When true the charger is in default mode and the host settings that are
     * "reset by WATCHDOG" have been lost and must be re-applied.
     */
    bq25792::expected<bool> is_watchdog_expired() const;

    /**
     * @brief Get device info (Part ID)
     * @return Expected part ID
     */
    bq25792::expected<uint8_t> get_device_info() const;

    /**
     * @brief Get I2C address
     */
    static constexpr uint8_t get_i2c_address() { return BQ25792_I2C_ADDRESS; }

    /**
     * @brief Get timeout in milliseconds
     */
    static constexpr uint32_t get_timeout_ms() { return BQ25792_TIMEOUT_MS; }

    /**
     * @brief Check if driver is initialized
     */
    bool is_initialized() const { return initialized_; }

    /**
     * @brief Get input source selection
     * @return Expected ChargerInputSource enum
     */
    bq25792::expected<ChargerInputSource> get_input_source() const;

    /**
     * @brief Set input source selection
     * @param source Charger input source
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> set_input_source(ChargerInputSource source) const;

    /**
     * @brief Get VSYS_MIN fixed offset
     */
    static constexpr uint16_t get_vsys_min_offset() { return BQ25792_VSYS_MIN_FIXED_OFFSET; }

    /**
     * @brief Get VSYS_MIN step size
     */
    static constexpr uint16_t get_vsys_min_step() { return BQ25792_VSYS_MIN_STEP_SIZE; }

    /**
     * @brief Get charge voltage step size (in mV)
     */
    static constexpr uint16_t get_charge_voltage_step() { return BQ25792_CHARGE_VOLTAGE_LIMIT_STEP; }

    /**
     * @brief Get charge current step size (in mA)
     */
    static constexpr uint16_t get_charge_current_step() { return BQ25792_CHARGE_CURRENT_LIMIT_STEP; }

    /**
     * @brief Get input voltage step size (in mV)
     */
    static constexpr uint16_t get_input_voltage_step() { return BQ25792_INPUT_VOLTAGE_LIMIT_STEP; }

    /**
     * @brief Get input current step size (in mA)
     */
    static constexpr uint16_t get_input_current_step() { return BQ25792_INPUT_CURRENT_LIMIT_STEP; }

    /**
     * @brief Get precharge current step size (in mA)
     */
    static constexpr uint16_t get_precharge_current_step() { return BQ25792_PRECHARGE_CURRENT_STEP; }

    // =============================================================================
    // Burst Read/Write Methods for Multiple Registers
    // =============================================================================

    /**
     * @brief Read multiple consecutive registers starting from a given address
     * @param reg_addr Starting register address
     * @param data Buffer to store read data
     * @param size Number of bytes to read (max 16 to avoid address wrap)
     * @return Expected void on success
     */
    bq25792::expected<void> read_block(uint8_t reg_addr, uint8_t* data, uint8_t size) const;

    /**
     * @brief Write multiple consecutive registers starting from a given address
     * @param reg_addr Starting register address
     * @param data Data to write
     * @param size Number of bytes to write (max 16 to avoid address wrap)
     * @return Expected void on success
     */
    bq25792::expected<void> write_block(uint8_t reg_addr, const uint8_t* data, uint8_t size) const;

    /**
     * @brief Read all status and flag registers in one burst (registers 0x1B-0x27)
     * @param buffer Buffer of at least 13 bytes to store status registers
     * @return Expected void on success
     */
    bq25792::expected<void> read_status_block(uint8_t buffer[13]) const;

    /**
     * @brief Read all ADC registers in one burst (registers 0x31-0x45)
     * @param buffer Buffer of at least 22 bytes to store ADC values
     * @return Expected void on success
     */
    bq25792::expected<void> read_adc_block(uint8_t buffer[22]) const;

    /**
     * @brief Read all configuration registers in one burst (registers 0x00-0x14)
     * @param buffer Buffer of at least 21 bytes to store configuration
     * @return Expected void on success
     */
    bq25792::expected<void> read_config_block(uint8_t buffer[21]) const;

private:
    I2C_HandleTypeDef* hi2c_;
    bool initialized_;

    /**
     * @brief Convert HAL status to expected error code
     */
    std::error_code convert_hal_status(HAL_StatusTypeDef status) const;

    /**
     * @brief Read a single byte from a register
     */
    bq25792::expected<uint8_t> read_byte(uint8_t reg_addr) const;

    /**
     * @brief Read multiple bytes from registers
     */
    bq25792::expected<void> read_bytes(uint8_t reg_addr, uint8_t* data, uint8_t size) const;

    /**
     * @brief Write a single byte to a register
     */
    bq25792::expected<void> write_byte(uint8_t reg_addr, uint8_t data) const;

    /**
     * @brief Write multiple bytes to registers
     */
    bq25792::expected<void> write_bytes(uint8_t reg_addr, const uint8_t* data, uint8_t size) const;

    /**
     * @brief Write a 16-bit word to registers (big-endian)
     */
    bq25792::expected<void> write_word(uint8_t reg_addr, uint16_t data) const;

    bq25792::expected<void> update_bits(uint8_t reg_addr, uint8_t mask, uint8_t value) const;
};

} // namespace bq25792

#endif // __BQ25792_HPP
