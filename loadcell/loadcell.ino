//******************************************************************************************************
// Load Cell (Sparkfun HX711, SSD1306 OLED with I2C mode and Arduino Nano 33 BLE 
// (https://simple-circuit.com/arduino-ssd1306-oled-i2c-spi-example/)
//
// PIN MAPPING
// SSD1306    | Arduino Nano 33 BLE
// 1 GND      | GND
// 2 VCC      | +3V3
// 3 D0       | I2C_SCL
// 4 D1       | I2C_SDA
// 5 RES      | D2
// 6 DC       | +3V3
// 7 CS       | open
//
// HX711      | Arduino Nano 33 BLE
// 1 VCC/VDD  | +3V3
// 2 DAT      | D3
// 3 CLK      | D4
// 4 GND      | GND
//
// PROGRAM DOWNLOAD PROCEDURE
// Especially when using the Arduino Serial Monitor, the download of new code is a bit tricky:
// 1) Close the Serial Monitor (and hide other Arduino sketch windows!)
// 2) Double click the Arduino Nano 33 BLE reset button (-> Arduino LED starts fading)
// 3) Connect the "other" COM Port (than the one from Serial monitor) under Tools > Port
// 4) Upload the sketch
//******************************************************************************************************

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HX711.h>

// Serial Port stuff
#define MAX_CMD_LEN    16
char Cmd[MAX_CMD_LEN];
char last_Cmd[MAX_CMD_LEN];
bool Cmd_complete = false;
int serial_cnt = 0;
char print_data[64];

// Scale stuff
HX711 scale;
uint8_t dataPin = 3;
uint8_t clockPin = 4;

// Display stuff
char oledBuf[64];

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     2 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define FILT_LEN 5
float weights[FILT_LEN];
int wIndex=0;
float wFiltered;

int weight=0;

//******************************************************************************************************
// loop()
//******************************************************************************************************
void loop() {
 
  display.clearDisplay();
  display.setTextSize(4);
  display.setCursor(20,25);
  
  weight = scale.get_units();
  sprintf(oledBuf,"%dg\r\n",weight);
  display.print(oledBuf);
  display.display();  // [29ms]
 
  // *** Handle console input ***
  while (Serial.available() > 0)                                  
  {
    char serial_input = (char)Serial.read();                      
    Serial.print(serial_input);                                  
        
    if (serial_cnt < MAX_CMD_LEN)
      Cmd[serial_cnt++] = serial_input;                      
    if (serial_input == '\r')                                   
    {
      Serial.print('\n');
      Cmd_complete = true;                                   
    }      
  }
  
  if (Cmd_complete)
  {
    parseCmd();

    // reset serial communication
    for (int i = 0; i < MAX_CMD_LEN; i++)  
    { 
      last_Cmd[i] = Cmd[i];     
      Cmd[i] = 0;                                     
    }                                                             
    serial_cnt = 0;                                                             
    Cmd_complete = false;                                                    
    Serial.print("\r\n>> ");
  }
}
