//******************************************************************************************************
// Simple Measurement Device 
// Provides analog (6x 10bit) and digital ports (2..13) with an Arduino Board
// Builds for NANO 33 BLE (#define BLE_SUPPORTED) or UNO R3 (#undef BLE_SUPPORTED)
// (c)2020, stefan.wyss@mt.com
//******************************************************************************************************
#define BLE_SUPPORTED 
#define MAX_CMD_LEN    16

char command[MAX_CMD_LEN];
char last_command[MAX_CMD_LEN];
char unit[2] = {'p','t'};
int mapping = 1023;
bool command_complete = false;
int serial_cnt = 0;
char print_data[64];
int PIN_AIN[6] = {A0,A1,A2,A3,A4,A5};

#ifdef BLE_SUPPORTED
#define BUFSIZE 64
#include <ArduinoBLE.h>

BLEService uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"); // create a new BLE UART service
BLECharacteristic txCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLENotify, BUFSIZE);
BLECharacteristic rxCharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite, BUFSIZE);  // write w/o response
#endif

//******************************************************************************************************
// publish()
//******************************************************************************************************
void publish(char* data)
{
  Serial.print(data); 
#ifdef BLE_SUPPORTED
  for (int len=0; len<BUFSIZE; len++)
  {
    if (data[len]=='\0')
    {
      txCharacteristic.writeValue(data, len);
      break;
    }
  }
#endif
}
//******************************************************************************************************
// setup()
//******************************************************************************************************
void setup() {
  Serial.begin(115200);                             
  Serial.print("Simple Measurement Device V1.0\r\n");                                      
  Serial.print(">> ");

#ifdef BLE_SUPPORTED
  if (!BLE.begin())                 
  { 
    Serial.println("Failed to start BLE!");
    while (1);                     
  }
  BLE.setLocalName("BLECOM"); 
  BLE.setAdvertisedService(uartService);
  uartService.addCharacteristic(txCharacteristic);
  uartService.addCharacteristic(rxCharacteristic);
  BLE.addService(uartService);       
  BLE.advertise();                 
  Serial.println("Bluetooth device active, waiting for connections...");
#endif
}
//******************************************************************************************************
// parseCmd()
//******************************************************************************************************
void parseCmd()
{ 
  // check if last command need to be restored (carriage return)
  if (command[0] == '\r')
  {
    for (int i=0; i<MAX_CMD_LEN; i++)
      command[i] = last_command[i];
  }
  if (strncmp(command, "help", 4) == 0)                                
  {                                                        
    publish("Help:\r\n");    
    publish("\tainX\t: read analog port X(0..5)\r\n");  
    publish("\tunit=X\t: setup 2 digit unit (e.g. unit=mV, default unit=pt)\r\n");  
    publish("\tmap=X\t: setup analog mapping 0..X (e.g. map=5000, default map=1023)\r\n"); 
    publish("\tdinX\t: read digital port X(2..13)\r\n");
    publish("\tdoutX=Y\t: write value Y(0 or 1) on digital port X(2..13)\r\n");
    sprintf(print_data, "(Config: unit=%c%c, map=%d)\r\n",unit[0],unit[1],mapping); 
    publish(print_data);                                        
  }
  else if (isDigit(command[4]) && (strncmp(command, "dout", 4) == 0))
  {
    int value = 0;
    int pin = 0;
    
    if (command[5] == '=') 
    {
      pin = (int)(command[4] - '0');
      value = (int)(command[6] - '0');
    }
    if (command[6] == '=')
    {
      pin = 10*(int)(command[4] - '0')+(int)(command[5] - '0'); 
      value = (int)(command[7] - '0');
    }
    if (pin<2 || pin>13 || value<0 || value>1)
    {
      publish("Input error!\r\n"); 
      return;
    } 
    pinMode(pin,OUTPUT);
    digitalWrite(pin,value); 
  }
  else if (isDigit(command[3]) && (strncmp(command, "din", 3) == 0))
  {
    char command_copy[MAX_CMD_LEN];
    for (int i = 0; i < MAX_CMD_LEN; i++)  
      command_copy[i] = command[i];                                          
    command_copy[serial_cnt]='\0';
    String str(&command_copy[3]);
    int pin = str.toInt();
    
    if (pin<2 || pin>13)
    {
      publish("Input error!\r\n"); 
      return; 
    }  
    pinMode(pin,INPUT);
    sprintf(print_data,"DIN%d: %d\r\n",pin,digitalRead(pin));
    publish(print_data);
  }
  else if (strncmp(command, "unit=", 5) == 0)
  {
    unit[0]=command[5];
    unit[1]=command[6];
  }
  else if (strncmp(command, "map=", 4) == 0)
  {
    command[serial_cnt]='\0';
    String str(&command[4]);
    mapping=str.toInt();
  }
  else if (isDigit(command[3]) && (strncmp(command, "ain", 3) == 0))
  {
    int channel = (int)(command[3] - '0');
    if (channel < 0 || channel > 6)
    {
      publish("Input error!\r\n"); 
      return; 
    }  
    int value = analogRead(PIN_AIN[channel]); 
    int mapped_val = map(value, 0, 1023, 0, mapping);
    sprintf(print_data, "AIN%d: %d %c%c\r\n",channel,mapped_val,unit[0],unit[1]); 
    publish(print_data);          
  }
  else
    publish("Input error!\r\n");   
}
//******************************************************************************************************
// loop()
//******************************************************************************************************
void loop() {
#ifdef BLE_SUPPORTED
  int ble_count=0;
  BLE.poll();                       
  if (rxCharacteristic.written())
  {
    ble_count = rxCharacteristic.readValue(command, MAX_CMD_LEN);
    command_complete = true;
  }
#endif 
  while (Serial.available() > 0)                                  // if serial data is received
  {
    char serial_input = (char)Serial.read();                      // read serial data
    Serial.print(serial_input);                                   // echo character
        
    if (serial_cnt < MAX_CMD_LEN)
      command[serial_cnt++] = serial_input;                       // store serial data in array
    if (serial_input == '\r')                                     // if serial command in finished (\r received)
    {
      Serial.print('\n');
      command_complete = true;                                   
    }      
  }
  
  if (command_complete)
  {
    parseCmd();

    // reset serial communication
    for (int i = 0; i < MAX_CMD_LEN; i++)  
    { 
      last_command[i] = command[i];     
      command[i] = 0;                                     
    }                                                             
    serial_cnt = 0;                                                             
    command_complete = false;                                                    
    Serial.print("\r\n>> ");
  }
}
