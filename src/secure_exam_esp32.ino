#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define LED_PIN     2
#define BUZZER_PIN  15

#define WIFI_SCAN_INTERVAL 5000
#define BLE_SCAN_TIME 3

#define CONFIRM_SCANS 2
#define DEVICE_TIMEOUT 15000

BLEScan *pBLEScan;

// Monitoring modes
enum MonitorMode {
  MODE_BOTH,
  MODE_WIFI,
  MODE_BLUETOOTH
};

MonitorMode monitorMode = MODE_BOTH;


// --------------------------------------------------
// DEVICE STRUCTURE
// --------------------------------------------------

struct DeviceInfo {

  String mac;
  String name;
  String type;

  int rssi;

  unsigned long lastSeen;

  int consecutiveSeen;

  bool authorized;
};


// Maximum devices stored
#define MAX_DEVICES 60

DeviceInfo devices[MAX_DEVICES];

int deviceCount = 0;

unsigned long lastWiFiScan = 0;


// --------------------------------------------------
// SETUP
// --------------------------------------------------

void setup() {

  Serial.begin(115200);

  delay(1000);

  // LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);


  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true, true);

  delay(500);


  // BLE
  BLEDevice::init("");

  pBLEScan = BLEDevice::getScan();

  pBLEScan->setActiveScan(true);

  pBLEScan->setInterval(100);

  pBLEScan->setWindow(80);


  Serial.println(
    "{\"status\":\"ESP32_READY\"}"
  );

  Serial.println(
    "{\"status\":\"MODE_BOTH\"}"
  );
}


// --------------------------------------------------
// MAIN LOOP
// --------------------------------------------------

void loop() {

  readCommands();

  if (
    monitorMode == MODE_BOTH ||
    monitorMode == MODE_WIFI
  ) {

    if (
      millis() - lastWiFiScan >=
      WIFI_SCAN_INTERVAL
    ) {

      lastWiFiScan = millis();

      scanWiFi();
    }
  }


  if (
    monitorMode == MODE_BOTH ||
    monitorMode == MODE_BLUETOOTH
  ) {

    scanBluetooth();
  }


  removeOldDevices();

  updateAlert();

  readCommands();

  delay(200);
}


// --------------------------------------------------
// WIFI SCAN
// --------------------------------------------------

void scanWiFi() {

  int count =
    WiFi.scanNetworks(
      false,
      true
    );


  for (int i = 0; i < count; i++) {

    String ssid =
      WiFi.SSID(i);

    String mac =
      WiFi.BSSIDstr(i);

    int rssi =
      WiFi.RSSI(i);


    if (ssid.length() == 0) {

      ssid = "Hidden Network";
    }


    updateDevice(
      mac,
      ssid,
      "wifi",
      rssi
    );
  }


  WiFi.scanDelete();
}


// --------------------------------------------------
// BLUETOOTH SCAN
// --------------------------------------------------

void scanBluetooth() {

  BLEScanResults *results =
    pBLEScan->start(
      BLE_SCAN_TIME,
      false
    );


  int count =
    results->getCount();


  for (int i = 0; i < count; i++) {

    BLEAdvertisedDevice device =
      results->getDevice(i);


    String mac =
      device
        .getAddress()
        .toString()
        .c_str();


    String name =
      "Unknown Bluetooth";


    if (device.haveName()) {

      name =
        device
          .getName()
          .c_str();
    }


    int rssi =
      device.getRSSI();


    updateDevice(
      mac,
      name,
      "bluetooth",
      rssi
    );
  }


  pBLEScan->clearResults();
}


// --------------------------------------------------
// UPDATE / ADD UNIQUE DEVICE
// --------------------------------------------------

void updateDevice(
  String mac,
  String name,
  String type,
  int rssi
) {

  int index =
    findDevice(mac);


  // -----------------------------------------------
  // Existing device
  // -----------------------------------------------

  if (index >= 0) {

    devices[index].name =
      name;

    devices[index].type =
      type;

    devices[index].rssi =
      rssi;

    devices[index].lastSeen =
      millis();

    devices[index].consecutiveSeen++;

    devices[index].authorized =
      checkAuthorized(
        name,
        type
      );


    sendDeviceJSON(
      devices[index]
    );

    return;
  }


  // -----------------------------------------------
  // New device
  // -----------------------------------------------

  if (deviceCount >= MAX_DEVICES) {

    return;
  }


  devices[deviceCount].mac =
    mac;

  devices[deviceCount].name =
    name;

  devices[deviceCount].type =
    type;

  devices[deviceCount].rssi =
    rssi;

  devices[deviceCount].lastSeen =
    millis();

  devices[deviceCount].consecutiveSeen =
    1;

  devices[deviceCount].authorized =
    checkAuthorized(
      name,
      type
    );


  sendDeviceJSON(
    devices[deviceCount]
  );


  deviceCount++;
}


// --------------------------------------------------
// FIND DEVICE BY MAC
// --------------------------------------------------

int findDevice(String mac) {

  for (int i = 0; i < deviceCount; i++) {

    if (
      devices[i].mac.equalsIgnoreCase(mac)
    ) {

      return i;
    }
  }


  return -1;
}


// --------------------------------------------------
// AUTHORIZATION
// --------------------------------------------------

bool checkAuthorized(
  String name,
  String type
) {

  /*
     SIMATS is the default authorized
     Wi-Fi network.

     Additional SSIDs can be sent from
     the HTML dashboard later.
  */

  if (type == "wifi") {

    if (
      name.equalsIgnoreCase("SIMATS")
    ) {

      return true;
    }


    return false;
  }


  /*
     Bluetooth devices are currently
     considered unauthorized.

     We can add Bluetooth MAC whitelist
     support in the next version.
  */

  return false;
}


// --------------------------------------------------
// SEND DEVICE JSON
// --------------------------------------------------

void sendDeviceJSON(
  DeviceInfo &device
) {

  Serial.print("{");

  Serial.print("\"event\":\"device\",");

  Serial.print("\"type\":\"");
  printJSONString(device.type);
  Serial.print("\",");

  Serial.print("\"name\":\"");
  printJSONString(device.name);
  Serial.print("\",");

  Serial.print("\"mac\":\"");
  printJSONString(device.mac);
  Serial.print("\",");

  Serial.print("\"rssi\":");
  Serial.print(device.rssi);

  Serial.print(",");

  Serial.print("\"authorized\":");
  Serial.print(
    device.authorized
      ? "true"
      : "false"
  );

  Serial.print(",");

  Serial.print("\"seen\":");
  Serial.print(
    device.consecutiveSeen
  );

  Serial.println("}");
}


// --------------------------------------------------
// REMOVE OLD DEVICES
// --------------------------------------------------

void removeOldDevices() {

  unsigned long now =
    millis();


  for (int i = 0; i < deviceCount; i++) {

    if (
      now - devices[i].lastSeen
      > DEVICE_TIMEOUT
    ) {

      removeDevice(i);

      i--;
    }
  }
}


// --------------------------------------------------
// REMOVE DEVICE FROM ARRAY
// --------------------------------------------------

void removeDevice(int index) {

  for (
    int i = index;
    i < deviceCount - 1;
    i++
  ) {

    devices[i] =
      devices[i + 1];
  }


  deviceCount--;


  Serial.print(
    "{\"event\":\"removed\",\"mac\":\""
  );

  printJSONString(
    devices[index].mac
  );

  Serial.println("\"}");
}


// --------------------------------------------------
// ALERT LOGIC
// --------------------------------------------------

void updateAlert() {

  bool unauthorized =
    false;


  for (int i = 0; i < deviceCount; i++) {

    if (
      !devices[i].authorized &&
      devices[i].consecutiveSeen
      >= CONFIRM_SCANS
    ) {

      unauthorized = true;

      break;
    }
  }


  if (unauthorized) {

    digitalWrite(
      LED_PIN,
      HIGH
    );

    digitalWrite(
      BUZZER_PIN,
      HIGH
    );

  }
  else {

    digitalWrite(
      LED_PIN,
      LOW
    );

    digitalWrite(
      BUZZER_PIN,
      LOW
    );
  }
}


// --------------------------------------------------
// RECEIVE COMMANDS FROM HTML
// --------------------------------------------------

void readCommands() {

  if (!Serial.available()) {

    return;
  }


  String command =
    Serial.readStringUntil('\n');

  command.trim();


  // -----------------------------------------------
  // WIFI ONLY
  // -----------------------------------------------

  if (
    command == "MODE_WIFI"
  ) {

    monitorMode =
      MODE_WIFI;

    clearDevices();

    Serial.println(
      "{\"status\":\"MODE_WIFI\"}"
    );

    return;
  }


  // -----------------------------------------------
  // BLUETOOTH ONLY
  // -----------------------------------------------

  if (
    command == "MODE_BLUETOOTH"
  ) {

    monitorMode =
      MODE_BLUETOOTH;

    clearDevices();

    Serial.println(
      "{\"status\":\"MODE_BLUETOOTH\"}"
    );

    return;
  }


  // -----------------------------------------------
  // BOTH
  // -----------------------------------------------

  if (
    command == "MODE_BOTH"
  ) {

    monitorMode =
      MODE_BOTH;

    clearDevices();

    Serial.println(
      "{\"status\":\"MODE_BOTH\"}"
    );

    return;
  }


  // -----------------------------------------------
  // CLEAR
  // -----------------------------------------------

  if (
    command == "CLEAR"
  ) {

    clearDevices();

    return;
  }
}


// --------------------------------------------------
// CLEAR DEVICE LIST
// --------------------------------------------------

void clearDevices() {

  deviceCount = 0;

  digitalWrite(
    LED_PIN,
    LOW
  );

  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  Serial.println(
    "{\"event\":\"clear\"}"
  );
}


// --------------------------------------------------
// JSON STRING HELPER
// --------------------------------------------------

void printJSONString(
  String text
) {

  for (
    int i = 0;
    i < text.length();
    i++
  ) {

    char c =
      text.charAt(i);


    if (
      c == '"' ||
      c == '\\'
    ) {

      Serial.print('\\');
    }


    Serial.print(c);
  }
}
