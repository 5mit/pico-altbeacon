/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "pico/stdlib.h"
#include "common.h"


static uint8_t adv_data[] = {
    // Flags general discoverable
    0x02, 0x01, 0x06, 0x1B, 0xFF, BEACON_MFG_ID, 0xBE, 0xAC, 
    '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0',0x00,0x00,
    RSSI_AT_1m, 0x00
};
static const uint8_t adv_data_len = sizeof(adv_data);

int main() {
    stdio_init_all();

    // initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    sm_init();

    // setup advertisements
    printf("Beacon Data:\n");
    for (int i = 0; i < adv_data_len; ++i) {
        printf("%X ", adv_data[i]);
    }
    printf("\n");
    uint16_t adv_int_min = 800;
    uint16_t adv_int_max = 800;
    uint8_t adv_type = 2;
    bd_addr_t null_addr;
    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    assert(adv_data_len <= 31); // ble limitation
    gap_advertisements_set_data(adv_data_len, (uint8_t*) adv_data);
    gap_advertisements_enable(1);
    //gap_discoverable_control(0);
    //gap_connectable_control(0);

    // turn on bluetooth!
    hci_power_control(HCI_POWER_ON);
    
    while(true) {      
        sleep_ms(1000);
    }

    return 0;
}
