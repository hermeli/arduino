#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DS3231.h>
#include <Arduino_HTS221.h>
#include <Arduino_LSM9DS1.h>
#include <Arduino_APDS9960.h>

#define MAX_CMD_LEN    16
char Cmd[MAX_CMD_LEN];
char last_Cmd[MAX_CMD_LEN];
bool Cmd_complete = false;
int serial_cnt = 0;
char print_data[64];

#define MODE_NORMAL 0
#define MODE_CONFIG 1
int mode = MODE_NORMAL; 

#define MPOS_MODULUS 3
int menuPos = 0;

#define CPOS_MODULUS 3
int cursorPos = 0;

int displayCtr = 0;

DS3231 Clock;
byte Year = 20;
byte Month = 8;
byte Date = 2;
byte DoW = 7;
byte Hour = 12;
byte Minute = 0;
byte Second = 0;
bool h12, PM, Century;

char oledBuf[64];

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     2 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//******************************************************************************************************
// loop()
//******************************************************************************************************
void loop() {
  float x, y, z;
  float ax,ay,az;

  int cursorPosStartX[3] = {2,56,104};
  int cursorPosStopX[3] = {34,88,125};
  
  display.clearDisplay();
  display.setCursor(0,0);             // Start at top-left corner 

  // *** Read sensor values ***
  if (IMU.accelerationAvailable()) 
    IMU.readAcceleration(ax, ay, az);
  if (IMU.magneticFieldAvailable()) 
    IMU.readMagneticField(x, y, z);  

  Hour = Clock.getHour(h12, PM);
  Minute = Clock.getMinute();
  Second = Clock.getSecond();
  
  // *** Display menu contents ***
  switch (menuPos)
  {
    case 0:
    default:
      // Display TIME     
      display.setCursor(2,24);
      display.setTextSize(3);
      sprintf(oledBuf,"%02d:%02d",Hour,Minute);
      display.print(oledBuf);
      display.setTextSize(2);
      sprintf(oledBuf,":%02d\n",Second);
      display.print(oledBuf); 
      break;      
    case 1: 
      // Display DATE
      display.setCursor(2,24);
      display.setTextSize(3);
      sprintf(oledBuf,"%02d.%02d",Clock.getDate(),Clock.getMonth(Century));
      display.print(oledBuf);
      display.setCursor(6*5*3,4*9+2);
      display.setTextSize(1);
      sprintf(oledBuf,".20%02d\n",Clock.getYear());
      display.print(oledBuf); 
      break; 
    case 2:
      // Display CONDITIONS
      display.setTextSize(1);
      sprintf(oledBuf,"Tmp: %.1f %cC\n",Clock.getTemperature(),248);
      display.print(oledBuf);
      sprintf(oledBuf,"Hum: %.1f %%\n",HTS.readHumidity());
      display.print(oledBuf); 
      sprintf(oledBuf,"Mag:%.0f,%.0f,%.0fuT\n",x,y,z);
      display.print(oledBuf);   
      sprintf(oledBuf,"Acc:%.1f,%.1f,%.1f g",ax,ay,az);
      display.print(oledBuf);
      break;
  }

  // *** Show cursor (only in config mode) ***
  if (mode == MODE_CONFIG)
  {
    if ((displayCtr++)%2 == 0)
    {
      display.drawLine(cursorPosStartX[cursorPos], 48, cursorPosStopX[cursorPos], 48, SSD1306_WHITE);
      display.drawLine(cursorPosStartX[cursorPos], 49, cursorPosStopX[cursorPos], 49, SSD1306_WHITE);
    }
  }
  // *** Here we show the display ***
  display.display();
  
  // *** Toggle display inversion if Acc>1.5***
  if (ax>1.5 || ay>1.5 || az>1.5)
  {
    mode ^= 1;  // toggle mode
    if (mode == MODE_NORMAL)
      display.invertDisplay(false);
    else
    {
      display.invertDisplay(true);
    }
    display.display();
    delay(500);
  }
   
  // *** Check gesture sensor ***
  if (APDS.gestureAvailable()) {
    int gesture = APDS.readGesture(); 
    switch (gesture) {
      case GESTURE_UP:
        Serial.println("Detected UP gesture");
        if (mode == MODE_CONFIG)
        {
          switch (menuPos*10+cursorPos)
          {
            case 0:
              Clock.setHour(Clock.getHour(h12, PM)+1);
              break; 
            case 1:
              Clock.setMinute(Clock.getMinute()+1);
              break;
            case 2:
              Clock.setSecond((Clock.getSecond()+10)%60);
              break;
            case 10:
              Clock.setDate(Clock.getDate()+1);          
              break;
            case 11:
              Clock.setMonth(Clock.getMonth(Century)+1);
              break;
            case 12:
              Clock.setYear(Clock.getYear()+1);
              break;
            case 20:
            case 21:
            case 22:
            default:
              break;
          }
        }
        break;

      case GESTURE_DOWN:
        Serial.println("Detected DOWN gesture");
        if (mode == MODE_CONFIG)
        {
          switch (menuPos*10+cursorPos)
          {
            case 0:
              Clock.setHour(Clock.getHour(h12, PM)-1);
              break; 
            case 1:
              Clock.setMinute(Clock.getMinute()-1);
              break;
            case 2:
              Clock.setSecond((Clock.getSecond()-10)%60);
              break;
            case 10:
              Clock.setDate(Clock.getDate()-1);          
              break;
            case 11:
              Clock.setMonth(Clock.getMonth(Century)-1);
              break;
            case 12:
              Clock.setYear(Clock.getYear()-1);
              break;
            case 20:
            case 21:
            case 22:
            default:
              break;
          }
        }
        break;

      case GESTURE_RIGHT:
        Serial.println("Detected RIGHT gesture");
        if (mode == MODE_NORMAL)
        {
          menuPos++;
          menuPos = menuPos % MPOS_MODULUS;
        }
        else
        {
          cursorPos++;
          cursorPos = cursorPos % CPOS_MODULUS;
        }
        break;

      case GESTURE_LEFT:
        Serial.println("Detected LEFT gesture");
        if (mode == MODE_NORMAL)
        {
          if (menuPos>0)
            menuPos--;
          else 
            menuPos=MPOS_MODULUS-1;
        }
        else
        {
          if (cursorPos>0)
            cursorPos--;
          else 
            cursorPos=CPOS_MODULUS-1;
        }
        break;
      case GESTURE_NONE:
        Serial.println("Detected NONE gesture");
        break;
      default:
        Serial.println("Default");
        break;
    }
  }  

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
