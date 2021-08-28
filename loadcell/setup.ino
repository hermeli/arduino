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
  display.setTextColor(SSD1306_WHITE); // Draw white text

  scale.begin(dataPin, clockPin);
  scale.set_scale(487);
  scale.set_offset(-75400);   // -75400 (empty) to 168150 (500g) = 243550/500g => 487/g

  pinMode(BlackButton, INPUT_PULLUP);
  pinMode(GreenButton, INPUT_PULLUP);
}
