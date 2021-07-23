//******************************************************************************************************
// publish()
//******************************************************************************************************
void publish(char* data)
{
  Serial.print(data); 
}
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
    publish("Help:\r\n");    
    publish("\ 0g\t: calibrate with 0g (offset)\r\n");
    publish("\ 500g\t: calibrate with 500g (scale)\r\n");
    publish(print_data);                                        
  }
  else if (strncmp(Cmd, "0g", 2) == 0)
  {
    scale.tare();
    publish("0g calibration done!\r\n");
  }   
  else if (strncmp(Cmd, "500g", 4) == 0)
  {
    scale.calibrate_scale(500, 5);
    publish("500g calibration done!\r\n");
  }   
  else
    publish("Input error!\r\n");   
}
