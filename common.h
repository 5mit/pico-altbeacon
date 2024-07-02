#ifndef COMMON_H_
#define COMMON_H_

#include "pico/cyw43_arch.h"

#define RSSI_AT_1m (-50)
#define PATH_LOSS 2

#define BEACON_MFG_ID 0x81, 0x11
#define BEACON_ID 0x02, 0x01, 0x06, 0x1B, 0xFF, BEACON_MFG_ID, 0xBE, 0xAC // Identifier for beacon (follows AltBeacon pseudo-standard)
//This string is automatically replaced by Pico's BSSID + 1 when adv data is set
#define CYW43_SN_PLACEHOLDER '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0' 

extern uint8_t adv_data[];
extern const uint8_t adv_data_len;
extern uint8_t *beacon_id;
extern const uint8_t beacon_id_len;
#endif