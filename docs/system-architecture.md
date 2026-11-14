# System Architecture

## Functional layers

### 1. Wireless sensing layer

The ESP32's integrated Wi-Fi and BLE radios passively observe nearby wireless infrastructure and BLE advertisements. Wi-Fi scanning returns access-point information; BLE scanning returns advertiser information.

### 2. Embedded processing layer

The firmware performs:
- MAC-address-based de-duplication
- RSSI/name/type/last-seen updates
- Simple SSID authorization
- Observation-count confirmation
- 15-second stale-entry expiry
- GPIO alert control
- JSON serialization

### 3. Communication layer

USB serial is the bidirectional link between ESP32 and laptop. Both sides use 115200 baud. The ESP32 emits one JSON object per line; the dashboard sends newline-terminated text commands.

### 4. Dashboard layer

The standalone HTML page provides:
- ESP32 connection through the browser Web Serial API
- Wi-Fi/Bluetooth/combined monitoring mode selection
- Device table and summary counters
- Radar-style visualization
- Authorized SSID and Bluetooth list controls stored in browser local storage
- Invigilator contact form and prototype alert dialog

The firmware currently authorizes only the Wi-Fi SSID `SIMATS` and treats all BLE devices as unauthorized. The dashboard's whitelist UI is not transmitted as dynamic authorization rules by this firmware.

## Data flow

```text
Wi-Fi access points ──┐
                      ├─> ESP32 scan -> MAC table -> classify/confirm -> JSON
BLE advertisers ──────┘                                      |
                                                             v
Laptop browser dashboard <──── USB serial at 115200 baud <───┘
        |
        ├─> Device list / counters / radar
        ├─> Alert presentation
        └─> Contact details + prototype notification dialog

Laptop dashboard ── serial commands ──> ESP32 monitoring mode / clear
```

## Timing parameters

| Parameter | Value | Meaning |
|---|---:|---|
| `WIFI_SCAN_INTERVAL` | 5000 ms | Minimum scheduled interval between Wi-Fi scans |
| `BLE_SCAN_TIME` | 3 s | Duration of one BLE scan call |
| `CONFIRM_SCANS` | 2 | Observation count threshold |
| `DEVICE_TIMEOUT` | 15000 ms | Age after which an entry is removed |
| `MAX_DEVICES` | 60 | Maximum stored entries |
| Serial baud | 115200 | USB serial data rate |

BLE scanning is called repeatedly in the main loop and can occupy the processor for the scan duration. Command handling and Wi-Fi scheduling therefore do not behave like independent parallel tasks.
