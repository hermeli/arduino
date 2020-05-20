/*
  Button LED

  This example creates a BLE peripheral with service that contains a
  characteristic to control an LED and another characteristic that
  represents the state of the button.

  The circuit:
  - Arduino Nano 33 BLE
  - Button connected to pin 4

  You can use a generic BLE central app, like LightBlue (iOS and Android) or
  nRF Connect (Android), to interact with the services and characteristics
  created in this sketch.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>

const int ledPin = LED_BUILTIN;     // set ledPin to on-board LED
const int buttonPin = 4;            // set buttonPin to digital pin 4; 7th from bottom on right side

BLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214"); // create a new BLE LED service

                                    // create switch characteristic and allow remote device to read and write
BLEByteCharacteristic ledCharacteristic("19B10011-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
                                    // create button characteristic and allow remote device to get notifications
BLEByteCharacteristic buttonCharacteristic("19B10012-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(ledPin, OUTPUT);          // use the LED as an output
  pinMode(buttonPin, INPUT);        // use button pin as an input

  if (!BLE.begin())                 // initialize the BLE device
  {                                 // 1 on success, 0 on failure
    Serial.println("Failed to start BLE!");
    while (1);                      // only wait out is to kill execution
  }

  BLE.setLocalName("Nano33 ButtonLED");// set the local value used when advertising
  BLE.setAdvertisedService(ledService);// set the advertised service UUID used when advertising to the value of the BLEService provided
  
  ledService.addCharacteristic(ledCharacteristic);// add the characteristics to the service
  ledService.addCharacteristic(buttonCharacteristic);

  BLE.addService(ledService);       // add the service

  ledCharacteristic.writeValue(0);
  buttonCharacteristic.writeValue(0);

  BLE.advertise();                  // start advertising

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {

  BLE.poll();                       // poll for BLE events

  char buttonValue = digitalRead(buttonPin);  // read the current button pin state

  boolean buttonChanged = (buttonCharacteristic.value() != buttonValue);// has the value changed since the last read

  if (buttonChanged)
  {
    ledCharacteristic.writeValue(buttonValue);// button state changed, update characteristics
    buttonCharacteristic.writeValue(buttonValue);
  }

  if (ledCharacteristic.written() || buttonChanged)
  {
    
    if (ledCharacteristic.value())  // update LED, either central has written to characteristic or button state has changed
    {
      Serial.println("LED on");
      digitalWrite(ledPin, HIGH);
    } 
    else
    {
      Serial.println("LED off");
      digitalWrite(ledPin, LOW);
    }
  }
}
