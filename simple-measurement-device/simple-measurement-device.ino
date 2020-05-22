//******************************************************************************************************
// Simple Measurement Device 
// Provides analog (6x 10bit) and digital ports (2..13) with an Arduino Board
// (c)2020, stefan.wyss@mt.com
//******************************************************************************************************
#define MAX_CMD_LEN    16

char command[MAX_CMD_LEN];
char last_command[MAX_CMD_LEN];
char unit[2] = {'p','t'};
int mapping = 1023;
bool command_complete = false;
int serial_cnt = 0;
char print_data[64];
int PIN_AIN[6] = {A0,A1,A2,A3,A4,A5};
//******************************************************************************************************
// setup()
//******************************************************************************************************
void setup() {
  Serial.begin(9600);                             
  Serial.print("Simple Measurement Device V1.0\r\n");                                      
  Serial.print(">> ");
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
    Serial.print("Help:\r\n");    
    Serial.print("\tainX\t: read analog port X(0..5)\r\n");  
    Serial.print("\tunit=X\t: setup 2 digit unit (e.g. unit=mV, default unit=pt)\r\n");  
    Serial.print("\tmap=X\t: setup analog mapping 0..X (e.g. map=5000, default map=1023)\r\n"); 
    Serial.print("\tdinX\t: read digital port X(2..13)\r\n");
    Serial.print("\tdoutX=Y\t: write value Y(0 or 1) on digital port X(2..13)\r\n");
    sprintf(print_data, "(Config: unit=%c%c, map=%d)\r\n",unit[0],unit[1],mapping); 
    Serial.print(print_data);                                        
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
      Serial.print("Input error!\r\n"); 
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
      Serial.print("Input error!\r\n"); 
      return; 
    }  
    pinMode(pin,INPUT);
    sprintf(print_data,"DIN%d: %d\r\n",pin,digitalRead(pin));
    Serial.print(print_data);
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
      Serial.print("Input error!\r\n"); 
      return; 
    }  
    int value = analogRead(PIN_AIN[channel]); 
    int mapped_val = map(value, 0, 1023, 0, mapping);
    sprintf(print_data, "AIN%d: %d %c%c\r\n",channel,mapped_val,unit[0],unit[1]); 
    Serial.print(print_data);          
  }
  else
    Serial.print("Input error!\r\n");   
}
//******************************************************************************************************
// loop()
//******************************************************************************************************
void loop() {
  while (Serial.available() > 0)                                   // if serial data is received
  {
    char serial_input = (char)Serial.read();                    // read serial data
    Serial.print(serial_input);                                 // echo character

    // say what you got:
    // Serial.print("I received: ");
    // Serial.println(serial_input, HEX);
    
    if (serial_cnt < MAX_CMD_LEN)
      command[serial_cnt++] = serial_input;                         // store serial data in array
    if (serial_input == '\r')                                   // if serial command in finished (\r received)
      command_complete = true;                                      // command complete, leave serial event
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

//******************************************************************************************************
// serialEvent()
//******************************************************************************************************
/*void serialEvent()
{
  while (Serial.available() > 0)                                   // if serial data is received
  {
    char serial_input = (char)Serial.read();                    // read serial data
    Serial.print(serial_input);                                 // echo character

    // say what you got:
    Serial.print("I received: ");
    Serial.println(serial_input, HEX);
    
    if (serial_cnt < MAX_CMD_LEN)
      command[serial_cnt++] = serial_input;                         // store serial data in array
    if (serial_input == '\r')                                   // if serial command in finished (\r received)
      command_complete = true;                                      // command complete, leave serial event
  }
}
*/
