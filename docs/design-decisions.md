# Design Decisions

## Implemented decisions

1. **Single ESP32 controller:** The integrated Wi-Fi and BLE radios reduce external hardware requirements.
2. **MAC-based table key:** Repeated observations of the same reported MAC update one entry instead of always adding a new row.
3. **Combined monitoring as default:** The firmware starts in `MODE_BOTH`.
4. **Selectable monitoring modes:** The dashboard can send Wi-Fi-only, Bluetooth-only, or combined mode commands.
5. **Simple baseline authorization:** The SSID `SIMATS` is the only authorized Wi-Fi network in the supplied firmware. BLE observations are currently unauthorized.
6. **Observation threshold:** The hardware alert waits until the observation counter reaches two.
7. **Stale-entry expiry:** Entries not observed for more than 15 seconds are removed.
8. **JSON over USB serial:** A newline-delimited protocol keeps the ESP32/dashboard interface simple.
9. **Standalone dashboard:** The supplied UI is a single HTML file and does not require a separate frontend build system.

## Trade-offs and constraints

- SSID authorization is not a person/device identity system. A campus may have multiple access points using the same SSID, and phones may not expose their presence as Wi-Fi access points.
- BLE address randomization can make one physical device appear under changing addresses.
- RSSI varies with obstacles, orientation, antenna characteristics, and interference; it is not a precise distance estimate.
- The observation counter does not enforce two independent scan cycles.
- The current dashboard's local whitelist controls do not alter the ESP32's firmware-side authorization policy.
- The dashboard notification action displays a prepared alert; it is not a remote SMS/email/call service.
- The current firmware has a removal-event MAC reporting issue documented in [known-limitations.md](known-limitations.md).

## Future engineering improvements (not implemented in this source)

- Correct removal-event reporting and add a regression test.
- Separate scan scheduling from serial command handling using a non-blocking/task-based design.
- Add a deliberate scan-cycle confirmation state with reset/decay rules.
- Implement a validated, synchronized whitelist protocol if dynamic authorization is required.
- Add privacy-aware event retention and explicit operator controls.
- Integrate a secure backend for actual remote notifications, with authentication and delivery status.
- Validate performance and false-positive behavior in a controlled test environment.
