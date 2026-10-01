/**
 * @file main.c
 * @brief BQ25792 Test Application for STM32
 *
 * This application demonstrates the BQ25792 charger driver on an STM32 board.
 * It initializes the charger, configures charging parameters, and monitors status.
 */

#include "main.h"
#include "bq25792.hpp"
#include <cstdio>
#include <cstring>

// Global handles (defined in generated code)
extern I2C_HandleTypeDef hi2c1;

// BQ25792 driver instance
static bq25792::Charger* g_charger = nullptr;

// Function prototypes
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void Charger_Test(void);

int main(void)
{
    // Initialize HAL
    HAL_Init();

    // Configure system clock
    SystemClock_Config();

    // Initialize GPIO
    MX_GPIO_Init();

    // Initialize I2C
    MX_I2C1_Init();

    // Create charger instance
    g_charger = new bq25792::Charger(&hi2c1);

    // Initialize charger
    auto result = g_charger->initialize();
    if (!result) {
        // Initialization failed
        Error_Handler();
    }

    // Configure charging parameters
    result = g_charger->set_charge_voltage_limit(4.2f);
    if (!result) {
        // Handle error
    }

    result = g_charger->set_charge_current_limit(2.0f);
    if (!result) {
        // Handle error
    }

    result = g_charger->set_input_voltage_limit(5.0f);
    if (!result) {
        // Handle error
    }

    result = g_charger->set_input_current_limit(3.0f);
    if (!result) {
        // Handle error
    }

    // Main loop
    while (1) {
        Charger_Test();
        HAL_Delay(1000);
    }
}

static void Charger_Test(void)
{
    // Read and display battery voltage
    auto vbat = g_charger->get_vbat();
    if (vbat) {
        printf("VBAT: %.3f V\n", vbat.value());
    } else {
        printf("Failed to read VBAT: %s\n", vbat.error().message().c_str());
    }

    // Read and display IBUS current
    auto ibus = g_charger->get_ibus();
    if (ibus) {
        printf("IBUS: %.3f A\n", ibus.value());
    } else {
        printf("Failed to read IBUS: %s\n", ibus.error().message().c_str());
    }

    // Check status
    auto charge_status = g_charger->get_charge_status();
    if (charge_status) {
        printf("Charge Status: ");
        switch (charge_status.value()) {
            case bq25792::ChargeStatus::NotCharging:
                printf("Not Charging\n");
                break;
            case bq25792::ChargeStatus::TrickleCharge:
                printf("Trickle Charge\n");
                break;
            case bq25792::ChargeStatus::Precharge:
                printf("Precharge\n");
                break;
            case bq25792::ChargeStatus::FastCharge:
                printf("Fast Charge\n");
                break;
            case bq25792::ChargeStatus::TaperCharge:
                printf("Taper Charge\n");
                break;
            case bq25792::ChargeStatus::TopOff:
                printf("Top Off\n");
                break;
            case bq25792::ChargeStatus::ChargeDone:
                printf("Charge Done\n");
                break;
            default:
                printf("Reserved\n");
                break;
        }
    }

    // Check VBUS
    auto plugged_in = g_charger->is_plugged_in();
    if (plugged_in) {
        printf("VBUS: %s\n", plugged_in.value() ? "Present" : "Not Present");
    }

    // Check battery presence
    auto battery = g_charger->is_battery_present();
    if (battery) {
        printf("Battery: %s\n", battery.value() ? "Present" : "Not Present");
    }

    // Check for faults
    auto fault = g_charger->is_fault_present();
    if (fault) {
        printf("Fault: %s\n", fault.value() ? "Detected" : "None");
    }

    printf("\n");
}

/**
 * @brief System Clock Configuration
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Initializes the CPU, AHB and APB busses clocks
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Initializes the CPU, AHB and APB busses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief GPIO Initialization
 */
static void MX_GPIO_Init(void)
{
    // Configure LED pin for status indication
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable GPIO clock
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configure LED pin
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 * @brief I2C1 Initialization
 */
static void MX_I2C1_Init(void)
{
    // Configure I2C1
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief Error Handler
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(100);
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief Reports the name of the source file and the source line number
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
