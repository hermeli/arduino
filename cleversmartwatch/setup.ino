//******************************************************************************************************
// setup()
//******************************************************************************************************
void setup() {
  Wire.begin();
  Serial.begin(115200);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
     while(1); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960 sensor!");
  }

  if (!HTS.begin()) {
    Serial.println("Error initializing humidity temperature sensor!");
    while(1);
  }

  if (!IMU.begin()) {
    Serial.println("Error initializing IMU!");
    while(1);
  }
}
