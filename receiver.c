#include <stdio.h>
#include <math.h>
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "common.h"

#define SCAN_INTERVAL 0x0030
#define SCAN_WINDOW 0x0030

#define LED_QUICK_FLASH_DELAY_MS 100
#define LED_SLOW_FLASH_DELAY_MS 1000

typedef enum {
    OFF,
    SCAN_RESULT
} gc_state_t;

static btstack_packet_callback_registration_t hci_event_callback_registration;
static gc_state_t state = OFF;
static bd_addr_t server_addr;
static bd_addr_type_t server_addr_type;
static bool listener_registered;
static btstack_timer_source_t heartbeat;



static void client_start(void){
    printf("Start scanning!\n");
    state = SCAN_RESULT;
    gap_set_scan_parameters(0, SCAN_INTERVAL, SCAN_WINDOW);
    gap_start_scan();
}

static bool advertisement_report_is_beacon(uint8_t *advertisement_report){
    // get advertisement from report event
    const uint8_t * recv_data = gap_event_advertising_report_get_data(advertisement_report);
    uint8_t recv_len  = gap_event_advertising_report_get_data_length(advertisement_report);
    if (beacon_id_len > recv_len) {
        return false;
    }
    for (int i = 0; i < beacon_id_len; ++i) {
        if (beacon_id[i] != recv_data[i]) {
            return false;
        }
    }   
    return true;
}

static void hci_event_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;
    if (packet_type != HCI_EVENT_PACKET) return;
    uint8_t event_type = hci_event_packet_get_type(packet);
    switch(event_type){
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                gap_local_bd_addr(local_addr);
                printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
                client_start();
            } else {
                state = OFF;
            }
            break;
        case GAP_EVENT_ADVERTISING_REPORT:
            if (state != SCAN_RESULT) return;
            if (!advertisement_report_is_beacon(packet)) return;
            printf("Beacon Found.\n");
            const uint8_t * beacon_data = gap_event_advertising_report_get_data(packet) + beacon_id_len;
            uint8_t beacon_data_len  = gap_event_advertising_report_get_data_length(packet) - beacon_id_len;

            for (int i = 0 ; i < beacon_data_len; ++i) {
                printf("%2X ", beacon_data[i]);
            }
            printf("\n");
            for (int i = 0 ; i < size; ++i) {
                printf("%2X ", packet[i]);
            }
            printf("\n");

            // store address and type
            gap_event_advertising_report_get_address(packet, server_addr);
            server_addr_type = gap_event_advertising_report_get_address_type(packet);
            printf("Found device with addr %s.\n", bd_addr_to_str(server_addr));

            // Get rssi
            printf("RSSI: %d dBm\n", (int8_t)gap_event_advertising_report_get_rssi(packet));

            // Calc distance:
            printf("Distance: %lfm\n", pow(10, ((double)((int8_t)beacon_data[19] - (int8_t)gap_event_advertising_report_get_rssi(packet)) / (10 * PATH_LOSS))));
        default:
            break;
    }
}

static void heartbeat_handler(struct btstack_timer_source *ts) {
    // Invert the led
    static bool quick_flash;
    static bool led_on = true;

    led_on = !led_on;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    if (listener_registered && led_on) {
        quick_flash = !quick_flash;
    } else if (!listener_registered) {
        quick_flash = false;
    }

    // Restart timer
    btstack_run_loop_set_timer(ts, (led_on || quick_flash) ? LED_QUICK_FLASH_DELAY_MS : LED_SLOW_FLASH_DELAY_MS);
    btstack_run_loop_add_timer(ts);
}

int main() {
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        return -1;
    }

    hci_event_callback_registration.callback = &hci_event_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    heartbeat.process = &heartbeat_handler;
    btstack_run_loop_set_timer(&heartbeat, LED_SLOW_FLASH_DELAY_MS);
    btstack_run_loop_add_timer(&heartbeat);

    // turn on!
    hci_power_control(HCI_POWER_ON);


    while(true) {      
        sleep_ms(1000);
    }
    return 0;
}
