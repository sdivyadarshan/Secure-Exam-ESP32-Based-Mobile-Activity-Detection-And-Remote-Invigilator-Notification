# Testing and Validation Checklist

> Do not mark a test as passed until it has been run on the actual hardware/software combination and the result has been recorded.

## Development environment

- [ ] Record ESP32 board model and board revision.
- [ ] Record Arduino IDE version.
- [ ] Record Espressif ESP32 Arduino core version.
- [ ] Record operating system and browser version.
- [ ] Confirm the selected serial port and 115200-baud connection.

## Firmware checks

- [ ] Compile the supplied sketch without errors.
- [ ] Upload the firmware to the ESP32.
- [ ] Confirm `ESP32_READY` appears in Serial Monitor.
- [ ] Confirm initial `MODE_BOTH` status appears.
- [ ] Confirm Wi-Fi scan events are emitted.
- [ ] Confirm BLE scan events are emitted for devices advertising during the scan window.
- [ ] Confirm repeated observations update an existing MAC entry.
- [ ] Confirm the 60-entry capacity behavior.
- [ ] Confirm entries expire after the configured timeout.
- [ ] Verify LED behavior on GPIO 2.
- [ ] Verify buzzer behavior on GPIO 15 using a suitable driver circuit.

## Dashboard checks

- [ ] Open `web/dashboard.html` in a supported Chrome/Edge context.
- [ ] Connect to the ESP32 through Web Serial.
- [ ] Confirm incoming JSON lines are parsed.
- [ ] Confirm Wi-Fi-only mode sends `MODE_WIFI`.
- [ ] Confirm Bluetooth-only mode sends `MODE_BLUETOOTH`.
- [ ] Confirm combined mode sends `MODE_BOTH`.
- [ ] Confirm clear action sends `CLEAR`.
- [ ] Confirm device list, counters, and radar update.
- [ ] Confirm browser local-storage settings persist after reload.
- [ ] Confirm the invigilator contact form saves and reloads the entered details.
- [ ] Confirm Notify Invigilator displays the prototype alert dialog.

## Edge cases and false-positive checks

- [ ] Test a Wi-Fi environment with multiple access points sharing one SSID.
- [ ] Test hidden SSIDs.
- [ ] Test BLE devices with and without advertised names.
- [ ] Test devices that stop advertising or leave the area.
- [ ] Test BLE address randomization behavior.
- [ ] Test weak and fluctuating RSSI.
- [ ] Test serial disconnect/reconnect.
- [ ] Check removal events when multiple entries exist and compare the emitted MAC with the actual expired entry.

## Results log

No measured test results are asserted by this documentation. Add date, board/core versions, setup, expected result, actual result, and pass/fail status after each test run.
