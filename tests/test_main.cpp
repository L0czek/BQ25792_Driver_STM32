/**
 * @file test_main.cpp
 * @brief Main entry point for BQ25792 unit tests
 */

#include <iostream>

int main(int argc, char** argv) {
    std::cout << "BQ25792 Charger Driver - Host Unit Tests" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;

    // Run tests from test_bq25792.cpp
    extern int main_bq25792_tests();
    return main_bq25792_tests();
}
