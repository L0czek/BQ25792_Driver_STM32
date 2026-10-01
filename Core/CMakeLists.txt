# Core library for BQ25792
# This CMakeLists.txt is included by the parent CMakeLists.txt

# Add source files to the main library
target_sources(bq25792_driver
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/bq25792.cpp
)
