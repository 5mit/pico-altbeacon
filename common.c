#include "common.h"

uint8_t adv_data[] = {
    BEACON_ID,
    CYW43_SN_PLACEHOLDER, 0x00,0x00,
    RSSI_AT_1m, 0x00
};
const uint8_t adv_data_len = sizeof(adv_data);
uint8_t *beacon_id = adv_data;
const uint8_t beacon_id_len = 9;