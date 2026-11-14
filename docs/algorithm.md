# Detection Algorithm

## 1. Initialization

1. Start serial communication at 115200 baud.
2. Configure GPIO 2 as the LED output and GPIO 15 as the buzzer output; set both LOW.
3. Put Wi-Fi into station mode and disconnect from any associated network.
4. Initialize BLE and obtain the BLE scanner.
5. Enable active BLE scanning and configure scan interval/window.
6. Emit `ESP32_READY` and initial `MODE_BOTH` status messages.

## 2. Monitoring loop

The main loop reads serial commands, performs scans allowed by the selected monitoring mode, removes stale records, updates the hardware alert, reads commands again, and delays briefly.

### Wi-Fi path

- Runs when mode is Wi-Fi-only or combined.
- Starts a scan when at least 5000 ms has elapsed since the previous scheduled Wi-Fi scan.
- Reads SSID, BSSID/MAC, and RSSI for each result.
- Replaces an empty SSID with `Hidden Network`.
- Sends the observation to the common device-table update routine.
- Deletes Wi-Fi scan results after processing.

### BLE path

- Runs when mode is Bluetooth-only or combined.
- Performs a 3-second active scan.
- Reads each advertised device's MAC address, advertised name when present (otherwise `Unknown Bluetooth`), and RSSI.
- Sends the observation to the common device-table update routine.
- Clears BLE scan results after processing.

## 3. Device table

The table holds up to 60 entries. The MAC address is the lookup key.

- If the MAC already exists, update name, type, RSSI, last-seen time, authorization state, and increment `consecutiveSeen`.
- If the MAC is new and the table has capacity, create an entry with `consecutiveSeen = 1`.
- If the table is full, new devices are ignored until an existing entry expires.

## 4. Authorization rule

| Device category | Current rule |
|---|---|
| Wi-Fi | Authorized only when the SSID equals `SIMATS`, case-insensitive |
| BLE | Always unauthorized |

The current sketch does not receive dynamic whitelist entries from the dashboard. The HTML page's stored authorization lists should not be represented as a firmware-side whitelist.

## 5. Confirmation and alert

The firmware activates the GPIO 2 LED and GPIO 15 buzzer if at least one stored device is unauthorized and its `consecutiveSeen` value is at least `CONFIRM_SCANS = 2`. Otherwise both outputs are LOW.

Important interpretation: `consecutiveSeen` increments on each call to `updateDevice()`. It is not reset when an observation is missed and does not necessarily represent two separate complete scan cycles. It is therefore an observation count, despite the variable name.

## 6. Expiry

An entry is removed when `millis() - lastSeen > 15000`. The sketch emits a `removed` event after shifting the array.

Known issue: the current `removeDevice()` implementation shifts entries and decrements the count before reading `devices[index].mac`. The emitted MAC can consequently refer to the next entry after the shift. The source is preserved as supplied; see [known-limitations.md](known-limitations.md).

## 7. Serial interface

Device records are sent as newline-delimited JSON. The dashboard can request `MODE_WIFI`, `MODE_BLUETOOTH`, `MODE_BOTH`, or `CLEAR`. A mode change clears the current device table before reporting the new mode.
