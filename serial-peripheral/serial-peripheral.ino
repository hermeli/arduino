/*
  Serial Peripheral, 21.05.2020, wyss@superspider.net

  This example creates a BLE peripheral with service that contains a
  characteristic to receive some characters and sends them out in the
  serial port.

  The circuit:
  - Arduino Nano 33 BLE

  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.
*/

#include <ArduinoBLE.h>
#define BUFSIZE 64

BLEService comService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create a new BLE COM service

// create com characteristic and allow remote device to write & get notifications
BLECharacteristic comCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLEWrite | BLENotify, BUFSIZE);
//******************************************************************************************************
// setup()
//******************************************************************************************************
void setup() {
  Serial.begin(115200);
  //while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);          // use the LED as an output

  if (!BLE.begin())                 // initialize the BLE device
  { // 1 on success, 0 on failure
    Serial.println("Failed to start BLE!");
    while (1);                      // only wait out is to kill execution
  }

  BLE.setLocalName("BLECOM"); // set the local value used when advertising
  BLE.setAdvertisedService(comService);// set the advertised service UUID used when advertising to the value of the BLEService provided
  comService.addCharacteristic(comCharacteristic);// add the characteristics to the service
  BLE.addService(comService);       // add the service
  BLE.advertise();                  // start advertising
  Serial.println("Bluetooth device active, waiting for connections...");
}
//******************************************************************************************************
// loop()
//******************************************************************************************************
void loop() {
  int len = 0;
  byte rx[BUFSIZE];

  BLE.poll();                       // poll for BLE events
  if (comCharacteristic.written())
  {
    // central has written to com characteristic
    len = comCharacteristic.readValue(rx, BUFSIZE);
    for (int i = 0; i < len; i++)
      Serial.print((char)rx[i]);
  }

  // get serial characters and notify BLE central
  len = Serial.available();
  if (len > 0)
  {
    for (int i = 0; i < len; i++)
      rx[i] = Serial.read();

    comCharacteristic.writeValue(rx, len);
  }
}
