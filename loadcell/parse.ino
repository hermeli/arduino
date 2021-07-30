//******************************************************************************************************
// parseCmd()
//******************************************************************************************************
void parseCmd()
{ 
  // check if last Cmd needs to be restored (carriage return)
  if (Cmd[0] == '\r')
  {
    for (int i=0; i<MAX_CMD_LEN; i++)
      Cmd[i] = last_Cmd[i];
  }
  if (strncmp(Cmd, "help", 4) == 0)                                
  {                                                        
    Serial.print("Help:\r\n");    
    Serial.print("  0g\t: calibrate with 0g (offset)\r\n");
    Serial.print("  500g\t: calibrate with 500g (scale)\r\n");
    Serial.print("  get\t: get the current weight\r\n");                                        
  }
  else if (strncmp(Cmd, "0g", 2) == 0)
  {
    scale.tare();
    Serial.print("0g calibration done!\r\n");
  }   
  else if (strncmp(Cmd, "500g", 4) == 0)
  {
    scale.calibrate_scale(500, 5);
    Serial.print("500g calibration done!\r\n");
  }   
  else if (strncmp(Cmd, "get", 3) == 0)
  {
    sprintf(print_data,"%dg\r\n",weight);
    Serial.print(print_data);
  }   
  else
    Serial.print("Input error!\r\n");   
}
