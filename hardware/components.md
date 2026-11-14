# Hardware Components and Wiring

## Bill of materials

| Item | Quantity | Purpose |
|---|---:|---|
| ESP32 development board | 1 | Wi-Fi/BLE scanning, processing, serial communication |
| USB data cable | 1 | Programming and USB serial connection to laptop |
| LED | 1 (optional external) | Visual alert on GPIO 2 |
| 220–330 Ω resistor | 1 | Current limiting for an external LED |
| Active buzzer | 1 | Audible alert on GPIO 15 |
| NPN transistor / logic-level driver | 1 (recommended where required) | Drives a buzzer that exceeds safe GPIO current |
| Base resistor | 1 | Limits transistor base current; choose for the selected transistor |
| Breadboard and jumper wires | as required | Prototyping connections |

The ESP32 contains the Wi-Fi and Bluetooth Low Energy radios. No external RF module is required.

## Pin connections

| ESP32 pin | Connection | Notes |
|---|---|---|
| GPIO 2 | LED indicator output | Use a series resistor for an external LED. The board may also have an onboard LED on this pin. |
| GPIO 15 | Buzzer control output | Firmware drives this pin HIGH/LOW. Use a suitable driver stage if required by the buzzer current/voltage. |
| GND | Common ground | Connect the ESP32 and external driver circuit grounds. |
| USB | Laptop | Carries firmware upload and serial JSON/commands at 115200 baud. |

See [pin-connections.csv](../hardware/pin-connections.csv) for a compact pin map.

## Wiring guidance

### External LED

Connect GPIO 2 through a 220–330 Ω resistor to the LED anode; connect the LED cathode to GND. Confirm the board's onboard LED arrangement before adding an external LED in parallel.

### Active buzzer

The firmware uses `digitalWrite(BUZZER_PIN, HIGH/LOW)`, so it is intended for an active buzzer or a buzzer module that produces a tone from a logic-level enable. Do not assume an arbitrary buzzer can be driven directly from an ESP32 pin. If its current exceeds the GPIO's safe output capability, use a transistor driver and a suitable supply. Share ground with the ESP32. Check the selected module's voltage and polarity.

### USB serial

Connect the ESP32 to the laptop using a data-capable USB cable. The firmware uses 115200 baud. Close Arduino Serial Monitor before opening the browser's Web Serial connection.

> **Verification status:** These are logical pin assignments from the supplied sketch plus recommended protection guidance. The exact physical wiring should be checked against the board and buzzer module actually used.
