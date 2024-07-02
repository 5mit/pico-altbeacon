# Pico W - AltBeacon

An implementation of a BLE beacon using the AltBeacon pseudo standard.

This project contains two programs: picow_ble_beacon (beacon.c) and picow_ble_receiver (receiver.c)

# picow_ble_beacon (beacon.c)

Constantly emits unconnectable and non-directed advertisements with data formatted according to the AltBeacon standard that contains the Pico's Bluetooth address.

# picow_ble_receiver (receiver.c)

Constantly scans for bluetooth devices and checks to see if any are AltBeacons. 
If so, it prints the raw advertisement data, Bluetooth address of the Beacon, RSSI, and an estimate of the distance between the receiver and the beacon to serial.

