# Known Limitations and Issues

This page records behavior visible in the supplied firmware and dashboard. It is not a list of verified field performance.

1. **Wi-Fi observations are access points, not client phones.** The scan reports SSID/BSSID/RSSI for networks visible to the ESP32. It cannot reliably enumerate every phone or laptop in the room.
2. **BLE visibility is advertisement-dependent.** Devices that do not advertise during the scan window may not appear. BLE MAC randomization can change identifiers.
3. **Authorization is rudimentary.** Only SSID `SIMATS` is authorized; every BLE observation is unauthorized. The code does not authenticate users or device owners.
4. **Dashboard whitelist mismatch.** The supplied HTML stores Wi-Fi and Bluetooth authorization lists in browser local storage, but the ESP32 firmware accepts only mode and clear commands. The dashboard lists are not dynamically synchronized with the firmware's authorization function.
5. **Confirmation counter semantics.** `consecutiveSeen` increments on every observation and is not reset on a missed observation. Two observations are not necessarily two independent complete scans.
6. **Removal event MAC issue.** `removeDevice()` shifts the array and decrements `deviceCount` before reading `devices[index].mac`; the event can report the MAC of the next array entry. Fixing it requires saving the removed MAC before shifting.
7. **Blocking scan behavior.** The BLE scan call takes three seconds. During this call, the main loop cannot service serial commands or run the other loop logic.
8. **Notification is a prototype.** The dashboard displays an alert dialog using saved contact information. It does not send a real remote notification.
9. **RSSI is not distance.** Signal strength is affected by multipath, obstructions, transmit power, antenna orientation, and interference.
10. **No event history backend.** Device records are held in ESP32 RAM and dashboard browser memory/local storage; there is no authenticated server-side audit trail.

These limitations mean the prototype should support human review only. It must not be used as sole evidence of academic misconduct.
