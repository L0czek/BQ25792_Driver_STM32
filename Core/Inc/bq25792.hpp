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

// Charger Status 0 (REG1B, Table 8-36). WD_STAT and VBUS_PRESENT_STAT are defined above.
constexpr uint8_t BQ25792_IINDPM_STAT_MASK             = 0x80;  // bit 7: in IINDPM (IOTG in OTG mode)
constexpr uint8_t BQ25792_VINDPM_STAT_MASK             = 0x40;  // bit 6: in VINDPM (VOTG in OTG mode)
constexpr uint8_t BQ25792_POORSRC_STAT_MASK            = 0x10;  // bit 4: poor source detected
constexpr uint8_t BQ25792_PG_STAT_MASK                 = 0x08;  // bit 3: power good
constexpr uint8_t BQ25792_AC2_PRESENT_STAT_MASK        = 0x04;  // bit 2: VAC2 present (> VAC_PRESENT, 3.4 V typ)
constexpr uint8_t BQ25792_AC1_PRESENT_STAT_MASK        = 0x02;  // bit 1: VAC1 present

// Charger Status 4 (REG1F, Table 8-40)
constexpr uint8_t BQ25792_VBATOTG_LOW_STAT_MASK        = 0x10;  // bit 4: VBAT too low to enable OTG
constexpr uint8_t BQ25792_TS_COLD_STAT_MASK            = 0x08;  // bit 3: TS colder than T1
constexpr uint8_t BQ25792_TS_HOT_STAT_MASK             = 0x01;  // bit 0: TS hotter than T5

// Fault Status 0/1 (REG20 Table 8-41, REG21 Table 8-42)
constexpr uint8_t BQ25792_VAC2_OVP_STAT_MASK           = 0x02;  // REG20 bit 1
constexpr uint8_t BQ25792_VAC1_OVP_STAT_MASK           = 0x01;  // REG20 bit 0
constexpr uint8_t BQ25792_OTG_OVP_STAT_MASK            = 0x20;  // REG21 bit 5
constexpr uint8_t BQ25792_OTG_UVP_STAT_MASK            = 0x10;  // REG21 bit 4

// OTG regulation (REG0B Table 8-22, REG0D Table 8-23). Both reset by WATCHDOG and REG_RST.
constexpr uint16_t BQ25792_VOTG_OFFSET_MV              = 2800;    // VOTG = 2800 mV + code * 10 mV
constexpr uint16_t BQ25792_VOTG_STEP_MV                = 10;
constexpr uint16_t BQ25792_VOTG_MASK                   = 0x07FF;  // REG0B bits 10:0 (15:11 reserved)
constexpr uint16_t BQ25792_IOTG_STEP_MA                = 40;      // IOTG = code * 40 mA
constexpr uint8_t  BQ25792_IOTG_MASK                   = 0x7F;    // REG0D bits 6:0
constexpr uint8_t  BQ25792_PRECHG_TMR_MASK             = 0x80;    // REG0D bit 7 (kept by IOTG writes)

// Charger Control 2 (REG11, Table 8-27)
constexpr uint8_t BQ25792_AUTO_INDET_EN_MASK           = 0x40;  // bit 6: D+/D- detection at plug-in (WATCHDOG reset to 1)
constexpr uint8_t BQ25792_HVDCP_EN_MASK                = 0x08;  // bit 3: HVDCP handshake (REG_RST only)

// Charger Control 3 (REG12, Table 8-28). DIS_ACDRV is defined above.
constexpr uint8_t BQ25792_EN_OTG_MASK                  = 0x40;  // bit 6: OTG mode (WATCHDOG reset to 0)

// Charger Control 5 (REG14, Table 8-30)
constexpr uint8_t BQ25792_EN_IBAT_MASK                 = 0x20;  // bit 5: IBAT discharge sensing for the ADC
constexpr uint8_t BQ25792_EN_IINDPM_MASK               = 0x04;  // bit 2: internal IINDPM regulation
constexpr uint8_t BQ25792_EN_EXTILIM_MASK              = 0x02;  // bit 1: ILIM_HIZ pin clamp (REG_RST only)

// Temperature Control (REG16, Table 8-32): input pull-downs, REG_RST only
constexpr uint8_t BQ25792_VBUS_PD_EN_MASK              = 0x08;  // bit 3: 6 kOhm on VBUS
constexpr uint8_t BQ25792_VAC1_PD_EN_MASK              = 0x04;  // bit 2: ~60 Ohm on VAC1
constexpr uint8_t BQ25792_VAC2_PD_EN_MASK              = 0x02;  // bit 1: ~60 Ohm on VAC2

// ICO Current Limit (REG19/1A, Table 8-35): input current limit in use, read only
constexpr uint16_t BQ25792_ICO_ILIM_MASK               = 0x01FF;  // bits 8:0, 10 mA
constexpr uint16_t BQ25792_ICO_ILIM_STEP_MA            = 10;

// ADC Function Disable 1 (REG30) value written by configure_adc(): D+/D- are not connected
// on the board, so their channels only lengthen the continuous scan (VAC1 refresh).
constexpr uint8_t BQ25792_ADC_DISABLE_1_VALUE          = BQ25792_DP_ADC_DIS_MASK | BQ25792_DM_ADC_DIS_MASK;

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
 * @brief Charger Status 0 (REG1B), decoded. Status bits, not the clear-on-read flags.
 */
struct InputStatus {
    bool vbus_present;   // bit 0 VBUS_PRESENT_STAT
    bool ac1_present;    // bit 1 AC1_PRESENT_STAT: VAC1 above VAC_PRESENT (3.4 V typ rising)
    bool ac2_present;    // bit 2 AC2_PRESENT_STAT
    bool power_good;     // bit 3 PG_STAT
    bool poor_source;    // bit 4 POORSRC_STAT
    bool wd_expired;     // bit 5 WD_STAT: watchdog expired or chip reset (default mode)
    bool vindpm;         // bit 6 VINDPM_STAT (VOTG regulation in OTG mode)
    bool iindpm;         // bit 7 IINDPM_STAT (IOTG regulation in OTG mode)
    uint8_t raw;         // REG1B as read
};

/**
 * @brief VAC1, VAC2 and VBAT ADC results (one burst read, 0x37-0x3C), 1 mV
 *
 * VAC1/VAC2 are measured at the pins, before the input FETs, so they read the
 * connector voltage whether or not that path is selected.
 */
struct InputVoltages {
    uint16_t vac1_mv;
    uint16_t vac2_mv;
    uint16_t vbat_mv;    // BATP remote sense
};

/**
 * @brief Input mux and OTG control bits (REG12..REG13, one burst read)
 */
struct MuxState {
    bool dis_acdrv;      // REG12 bit 7: both ACFET pairs forced off
    bool en_otg;         // REG12 bit 6
    bool en_acdrv1;      // REG13 bit 6: ACFET1-RBFET1 (VAC1) on; the chip updates it on its own swaps
    bool en_acdrv2;      // REG13 bit 7: ACFET2-RBFET2 (VAC2) on
};

/**
 * @brief Battery and OTG related status (REG1D..REG21, one burst read; the
 *        clear-on-read flag registers from REG22 on are not touched)
 */
struct BatteryOtgStatus {
    bool battery_present;   // REG1D bit 0 VBAT_PRESENT_STAT (VBAT > VBAT_UVLOZ)
    bool vbat_otg_low;      // REG1F bit 4 VBATOTG_LOW_STAT
    bool ts_cold;           // REG1F bit 3 TS_COLD_STAT
    bool ts_hot;            // REG1F bit 0 TS_HOT_STAT
    bool vac1_ovp;          // REG20 bit 0 VAC1_OVP_STAT
    bool vac2_ovp;          // REG20 bit 1 VAC2_OVP_STAT
    bool otg_ovp;           // REG21 bit 5 OTG_OVP_STAT
    bool otg_uvp;           // REG21 bit 4 OTG_UVP_STAT
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
     * @brief (Re)configure the ADC: continuous, 14-bit, all main channels enabled,
     *        D+/D- channels disabled (not connected on the board)
     *
     * ADC_EN is reset by the watchdog, so call this again after a watchdog expiry.
     */
    bq25792::expected<void> configure_adc();

    /**
     * @brief Reset the I2C watchdog timer (REG10 WD_RST)
     *
     * Must be called more often than the watchdog period (40 s default). On
     * expiry the chip returns to default mode and every field marked "Reset by
     * WATCHDOG" in section 8.5 reverts, among them ICHG, EN_CHG, IPRECHG, ITERM,
     * VOTG, IOTG, EN_OTG, AUTO_INDET_EN, EN_IBAT, EN_IINDPM, IBAT_REG, the
     * TS/JEITA settings and ADC_EN. VINDPM, IINDPM, VREG, EN_EXTILIM, HVDCP_EN,
     * the input mux bits and the ADC channel/sample settings are kept.
     */
    bq25792::expected<void> kick_watchdog();

    /**
     * @brief Check whether the watchdog has expired (REG1B WD_STAT)
     *
     * When true the charger is in default mode (expiry or power-on reset) and the
     * host settings that are "reset by WATCHDOG" have been lost and must be
     * re-applied. Any register write restarts an expired watchdog and clears it.
     */
    bq25792::expected<bool> is_watchdog_expired() const;

    // =========================================================================
    // Input / power-path status
    // =========================================================================

    /**
     * @brief Read REG1B (input presence, power good, watchdog, DPM status)
     */
    bq25792::expected<InputStatus> get_input_status() const;

    /**
     * @brief Decode a REG1B value (for callers that read it in a burst)
     */
    static InputStatus decode_input_status(uint8_t reg1b);

    /**
     * @brief VAC1, VAC2 and VBAT ADC results in one 6-byte burst (0x37-0x3C)
     */
    bq25792::expected<InputVoltages> get_input_voltages() const;

    /**
     * @brief DIS_ACDRV, EN_OTG, EN_ACDRV1/2 in one 2-byte burst (REG12-REG13)
     */
    bq25792::expected<MuxState> get_mux_state() const;

    /**
     * @brief Battery present, OTG/TS and VAC/OTG fault status (REG1D-REG21 burst)
     */
    bq25792::expected<BatteryOtgStatus> get_battery_otg_status() const;

    /**
     * @brief Input current limit in use (REG19 ICO_ILIM, 10 mA), in amps
     *
     * The lower of IINDPM and the ILIM_HIZ clamp (while EN_EXTILIM=1), or the
     * ICO result. Reads 0 before an input has been qualified.
     */
    bq25792::expected<float> get_effective_input_current_limit() const;

    // =========================================================================
    // Input current / detection policy bits
    // =========================================================================

    /**
     * @brief REG14 EN_EXTILIM: let the ILIM_HIZ pin clamp IINDPM (POR: on, REG_RST only)
     */
    bq25792::expected<void> set_ext_ilim_enabled(bool enable);

    /**
     * @brief REG14 EN_IBAT: IBAT ADC also measures discharge current (watchdog resets it)
     */
    bq25792::expected<void> set_ibat_discharge_sense(bool enable);

    /**
     * @brief REG11 AUTO_INDET_EN: D+/D- source detection at VAC1 plug-in, which
     *        rewrites IINDPM (watchdog resets it to 1)
     *
     * Disabling also clears HVDCP_EN in the same write, so no HVDCP handshake runs.
     */
    bq25792::expected<void> set_auto_input_detection(bool enable);

    /**
     * @brief REG16 VAC1_PD_EN: VAC1 pull-down (~60 Ohm) to discharge the input
     *
     * About 6.7 W at 20 V: only briefly, never while a source drives VAC1.
     */
    bq25792::expected<void> set_vac1_pulldown(bool enable);

    // =========================================================================
    // OTG (reverse mode from the battery onto VBUS)
    // =========================================================================

    /**
     * @brief OTG regulation voltage (REG0B, 10 mV steps, 2.8-22 V)
     */
    bq25792::expected<void> set_otg_voltage(float voltage);
    bq25792::expected<float> get_otg_voltage() const;

    /**
     * @brief OTG current limit (REG0D IOTG, 40 mA steps, 0.12-3.32 A); keeps PRECHG_TMR
     */
    bq25792::expected<void> set_otg_current_limit(float current);
    bq25792::expected<float> get_otg_current_limit() const;

    /**
     * @brief REG12 EN_OTG. With an ACFET pair fitted the converter only starts
     *        once the matching EN_ACDRV bit is 1 (Table 8-8).
     */
    bq25792::expected<void> set_otg_enabled(bool enable);
    bq25792::expected<bool> is_otg_enabled() const;

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
     *
     * Clears DIS_ACDRV, then for AC1/AC2 writes EN_ACDRV1/EN_ACDRV2 in one REG13
     * write (the chip ignores both set together). AUTO only clears DIS_ACDRV.
     *
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
