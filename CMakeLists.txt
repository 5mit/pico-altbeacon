cmake_minimum_required(VERSION 3.28)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(pico_sdk_import.cmake)
project(wifi-tag C CXX ASM)

# init SDK
pico_sdk_init()

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

add_compile_options(
        -Wall                # Enable most warning messages
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

# Standalone example that reads from the on board temperature sensor and sends notifications via BLE
# Flashes slowly each second to show it's running
add_executable(picow_ble_beacon
    server.c
    )
target_link_libraries(picow_ble_beacon
    pico_stdlib
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
    )
target_include_directories(picow_ble_beacon PRIVATE
    ${CMAKE_CURRENT_LIST_DIR} # For btstack config
    )

pico_add_extra_outputs(picow_ble_beacon)

# Standalone example that connects to picow_ble_beacon and reads the temperature 
# Flahes once quickly each second when it's running but not connected to another device
# Flashes twice quickly each second when connected to another device and reading it's temperature
add_executable(picow_ble_beacon_receiver
    client.c
    )
target_link_libraries(picow_ble_beacon_receiver
    pico_stdlib
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
    hardware_adc
    )
target_include_directories(picow_ble_beacon_receiver PRIVATE
    ${CMAKE_CURRENT_LIST_DIR} # For btstack config
    )
target_compile_definitions(picow_ble_beacon_receiver PRIVATE
    RUNNING_AS_CLIENT=1
)

pico_add_extra_outputs(picow_ble_beacon_receiver)
