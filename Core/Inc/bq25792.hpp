#ifndef __BQ25792_HPP
#define __BQ25792_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include <string>
#include <cstring>
#include <stdexcept>
#include <system_error>

// Include custom expected for C++17 compatibility
// This is namespace-specific to avoid duplicate definitions
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
#define BQ25792_TIMEOUT_MS 1000
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
constexpr uint8_t BQ25792_PART_ID                      = 0x0A;  // Expected part ID

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
    FaultDetected
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
 * @brief RAII wrapper for aligned buffers (for DMA if needed)
 */
class Buffer {
public:
    /**
     * @brief Create an aligned buffer
     * @param size Buffer size in bytes
     * @param alignment Alignment in bytes (default: 8 for I2C)
     */
    explicit Buffer(size_t size, size_t alignment = 8)
        : buffer_(nullptr), size_(size), alignment_(alignment)
    {
        if (size == 0) {
            throw std::invalid_argument("Buffer size cannot be zero");
        }
        buffer_ = static_cast<uint8_t*>(aligned_alloc(alignment, size));
        if (!buffer_) {
            throw std::bad_alloc();
        }
        std::fill(buffer_, buffer_ + size, 0);
    }

    /**
     * @brief Destructor
     */
    ~Buffer() {
        if (buffer_) {
            free(buffer_);
        }
    }

    // Disable copy
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    // Enable move
    Buffer(Buffer&& other) noexcept
        : buffer_(other.buffer_), size_(other.size_), alignment_(other.alignment_)
    {
        other.buffer_ = nullptr;
        other.size_ = 0;
        other.alignment_ = 8;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            if (buffer_) {
                free(buffer_);
            }
            buffer_ = other.buffer_;
            size_ = other.size_;
            alignment_ = other.alignment_;
            other.buffer_ = nullptr;
            other.size_ = 0;
            other.alignment_ = 8;
        }
        return *this;
    }

    /**
     * @brief Get pointer to buffer data
     */
    uint8_t* data() { return buffer_; }

    const uint8_t* data() const { return buffer_; }

    /**
     * @brief Get buffer size
     */
    size_t size() const { return size_; }

    /**
     * @brief Get alignment
     */
    size_t alignment() const { return alignment_; }

    /**
     * @brief Clear buffer to zero
     */
    void clear() {
        if (buffer_) {
            std::fill(buffer_, buffer_ + size_, 0);
        }
    }

    /**
     * @brief Fill buffer with value
     */
    void fill(uint8_t value) {
        if (buffer_) {
            std::fill(buffer_, buffer_ + size_, value);
        }
    }

private:
    uint8_t* buffer_;
    size_t size_;
    size_t alignment_;
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
     * @param voltage Voltage in volts (3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5)
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
     * @param voltage Voltage in volts (range: 3.5V to 19.2V)
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
     * @param current Current in amps (range: 0A to 12.79A)
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
     * @param voltage Voltage in volts (range: 3.9V to 25.5V)
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
     * @param current Current in amps (range: 0A to 12.79A)
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> set_input_current_limit(float current);

    /**
     * @brief Get precharge control settings
     * @return Precharge control struct with Vbat_low and Iprechrg
     */
    bq25792::expected<std::pair<float, float>> get_precharge_control() const;

    /**
     * @brief Set precharge control settings
     * @param vbat_low Vbat low threshold voltage in volts
     * @param iprechrg Precharge current in amps
     * @return bq25792::expected<void> - error on failure
     */
    bq25792::expected<void> set_precharge_control(float vbat_low, float iprechrg);

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

private:
    I2C_HandleTypeDef* hi2c_;
    bool initialized_;

    /**
     * @brief Convert HAL status to expected error code
     */
    static std::error_code convert_hal_status(HAL_StatusTypeDef status);

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
};

} // namespace bq25792

#endif // __BQ25792_HPP
