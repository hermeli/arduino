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
    publish("\tdate DDMMYYYY\t: setup date, e.g. 29.08.2020\r\n");
    publish("\ttime HHMMSS\t: setup time, e.g. 16:35:20\r\n");
    publish(print_data);                                        
  }
  else if ((strncmp(Cmd, "date", 4)==0))
  {
    for (int i=5; i<13; i++)
    {
      if (!isDigit(Cmd[i]))
      {
        publish("Argument error!\r\n"); 
        return;
      }
    }
    Date = (byte)(10*(int)(Cmd[5]-'0')+(int)(Cmd[6]-'0'));
    Month = (byte)(10*(int)(Cmd[7]-'0')+(int)(Cmd[8]-'0'));
    Year = (byte)(10*(int)(Cmd[11]-'0')+(int)(Cmd[12]-'0'));

    Clock.setYear(Year);
    Clock.setMonth(Month);
    Clock.setDate(Date);
    publish("OK\r\n");
  }
  else if ((strncmp(Cmd, "time", 4)==0))
  {
    for (int i=5; i<11; i++)
    {
      if (!isDigit(Cmd[i]))
      {
        publish("Argument error!\r\n"); 
        return;
      }
    }
    Hour = (byte)(10*(int)(Cmd[5]-'0')+(int)(Cmd[6]-'0'));
    Minute = (byte)(10*(int)(Cmd[7]-'0')+(int)(Cmd[8]-'0'));
    Second = (byte)(10*(int)(Cmd[9]-'0')+(int)(Cmd[10]-'0'));

    Clock.setHour(Hour);
    Clock.setMinute(Minute);
    Clock.setSecond(Second);
    publish("OK\r\n");
  }
  else
    publish("Input error!\r\n");   
}
