#include "expression_parser_string.h"

extern char* _parser_error_msg;

String evaluate(String expr)
{
  int status;
  //  expr = GetRidOfurlCharacters(expr);
  //  Serial.print("eval function ");
  //  Serial.print(expr);
  //  Serial.print("---");
  // check if the expression is empty; in that case returns an empty string
  if (expr == "")
  {
    return "";
  }
  delay(0);
  status = parse_expression_with_callbacks( expr.c_str(), variable_callback, function_callback, NULL, &numeric_value, string_value  );
  if (_parser_error_msg != NULL & _parser_error_msg != "Comparaison between string and number!")
  {
    PrintAndWebOut(String(_parser_error_msg));
    return F("error");
  }
  if (status == PARSER_STRING)
    return string_value;
  else
    return FloatToString(numeric_value);

}





/**
 @brief user-defined variable callback function. see expression_parser.h for more details.
 @param[in] user_data pointer to any user-defined state data that is required, none in this case
 @param[in] name name of the variable to look up the value of
 @param[out] value output point to double that holds the variable value on completion
 @param[out] value output point to string that holds the variable value on completion
 @return PARSER_TRUE if the variable exists and value was set by the callback with the double-prevision result stored in value or
         PARSER_STRING if the variable exists and value was set by the callback with the string result stored in value_str or
         PARSER_FALSE otherwise.
*/
int variable_callback( void *user_data, const char *name, float *value, String *value_str  ) {
  // look up the variables by name

  VariableLocated = 0;




  String Name = String(name);
  delay(0);
  for (byte i = 0; i < 50; i++)
  {
    if (AllMyVaribles[i][0] == Name)
    {
      delay(0);
      // we need to put some intelligence in order to understand if the variable is a string or a number
      // we make a fast check, needs to be improved!
      *value =  atof(AllMyVaribles[i][1].c_str());
      *value_str = AllMyVaribles[i][1];
      if (AllMyVaribles[i][1] == "")
        return PARSER_STRING;
      //we can define phases for the identification of the number; this doesn't works for the exponential numbers (ex 1.5e15)
      //phase 0 : space or sign '+' or sign '-' or digit
      //phase 1 : any digits or '.'
      //phase 2 : any digits until the end of the line
      //phase 9 : error! number not valid
      int phase = 0;
      char c;
      for (int j = 0; j < AllMyVaribles[i][1].length(); j++)
      {
        LastVarNumberLookedUp = i;
        VariableLocated = 1;
        c = AllMyVaribles[i][1][j];
        switch (phase)
        {
          case 0:
            if (c == ' ')
              break;
            if (c == '-' || c == '+' || isdigit(c))
              phase = 1;
            else
              phase = 9;
            break;
          case 1:
            if (isdigit(c))
              break;
            if (c == '.')
              phase = 2;
            else
              phase = 9;
            break;
          case 2:
            if (!isdigit(c))
              phase = 9;
            break;
        }
        if (phase == 9)
          break;
      }
      //if ( (*value == 0) &&  (AllMyVaribles[i][1].length() > 1)) // if the converted value is zero but the string is not "0"
      if (phase == 9)
        return PARSER_STRING;
      else
        return PARSER_TRUE;
      break;
    }
  }
  // failed to find variable, return false
  *value = 0;
  *value_str = name;
  // against my will, I have been obliged to put a PARSER_STRING here instead of PARSER_FALSE; this will inhibit the error message if the
  // variable is not existing permitting to use any variable not initialised; :-)
  return PARSER_STRING;
}

/**
 @brief user-defined function callback. see expression_parser.h for more details.
 @param[in] user_data input pointer to any user-defined state variables needed.  in this case, this pointer is the maximum number of arguments allowed to the functions (as a contrived example usage).
 @param[in] name name of the function to evaluate
 @param[in] num_args number of arguments that were parsed in the function call
 @param[in] args list of parsed arguments (double precision numbers) - if the arg is not valid, the value is nan
 @param[out] value output evaluated result of the function call (double precision number)
 @param[in] args list of parsed arguments (strings) - if the arg is not valid, the value is '\0'
 @param[out] value output evaluated result of the function call (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
         PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
         PARSER_FALSE otherwise.
*/
int function_callback( void *user_data, const char *name, const int num_args, const float *args, float *value, String **args_str, String *value_str ) {
  int i, max_args;
  float tmp;
  String fname = String(name);
  delay(0);

  // example to show the user-data parameter, sets the maximum number of
  // arguments allowed for the following functions from the user-data function
  max_args = *((int*)user_data);


  // this is in first position as it is required to be as fast as possible
  if ( fname == F("udpread") && num_args == 0 ) {
    // function udpread()
    //Serial.println(UdpBuffer);
    *value_str = String(UdpBuffer);
    //UdpBuffer = ""; // clear the variable after the read
    return PARSER_STRING;
  }
  else if ( fname == F("udpremote") && num_args == 0 ) {
    // function udpremote()
    *value_str = UdpRemoteIP.toString() + String(F(":")) + String(UdpRemotePort);
    return PARSER_STRING;
  }
  else if ( fname == F("millis") && num_args == 0 ) // the function is millis()
  {
    // set return value and return true
    *value = millis();
    return PARSER_TRUE;
  }
  else if ( fname == F("ramfree") && num_args == 0 ) // the function is ramfree()
  {
    // set return value and return true
    *value = ESP.getFreeHeap();
    return PARSER_TRUE;
  }
  else if ( fname == F("rnd") && num_args == 1 ) // the function is rnd(seed)
  {
    // set return value and return true
    randomSeed(millis());
    *value = random(args[0]);
    return PARSER_TRUE;
  }
  else if ( fname == F("flashfree") && num_args == 0 ) // the function is flashfree()
  {
    // set return value and return true
    struct FSInfo {
      size_t totalBytes;
      size_t usedBytes;
      size_t blockSize;
      size_t pageSize;
      size_t maxOpenFiles;
      size_t maxPathLength;
    };
    SPIFFS.info(fs_info);
    int Flashfree = fs_info.totalBytes - fs_info.usedBytes;
    *value = (float) Flashfree;
    return PARSER_TRUE;
  }
  else if ( fname == F("id") && num_args == 0 ) {
    // example of the id()
    // set return value
    *value_str = String(ESP.getChipId());
    return PARSER_STRING;
  }

  else if ( fname == F("htmlid") && num_args == 0 ) {
    // example of the htmlid()
    // set return value
    *value_str = String(LastElimentIdTag);
    return PARSER_STRING;
  }

  else if ( fname == F("timesetup") && num_args > 0 ) {
    String bla;
    SaveDataToFile("TimeZone", *args_str[0]);
    SaveDataToFile("DaylightSavings", *args_str[1]);
    configTime(LoadDataFromFile("TimeZone").toFloat() * 3600, LoadDataFromFile("DaylightSavings").toInt(), "pool.ntp.org", "time.nist.gov");
    return PARSER_STRING;
  }
  else if ( fname == F("mid") && num_args == 2 ) {
    // example of the mid(string, start)
    // set return value
    *value_str = args_str[0]->substring((int) args[1] - 1);
    *value = -1;
    return PARSER_STRING;
  }
  else if ( fname == F("mid") && num_args == 3 ) {
    // example of the mid(string, start, end)
    // set return value
    *value_str = args_str[0]->substring((int) args[1] - 1, (int) (args[1] + args[2]) - 1 );
    *value = -1;
    return PARSER_STRING;
  }
  else if ( fname == F("right") && num_args == 2 ) {
    // example of the right(string, length)
    // set return value
    *value_str = args_str[0]->substring(args_str[0]->length() - (int) args[1]);
    *value = -1;
    return PARSER_STRING;
  }
  else if ( fname == F("left") && num_args == 2 ) {
    // example of the left(string, length)
    // set return value
    *value_str = args_str[0]->substring(0, (int) args[1]);
    *value = -1;
    return PARSER_STRING;
  }
  else if ( fname == F("word") && num_args > 1 ) {
    // example of the word(string, length)
    // set return value
    char bla;
    if (num_args == 2) bla = ' ';
    if (num_args == 3) bla = *args_str[2]->c_str();

    *value_str = getValue(*args_str[0], bla  , (int) args[1] - 1 );
    *value = -1;
    return PARSER_STRING;
  }



  else if ( fname == F("len") && num_args == 1 ) {
    // example of the len(string)
    // set return value
    *value  = args_str[0]->length();
    return PARSER_TRUE;
  }
  else if ( fname == F("upper") && num_args == 1 ) {
    // example of the upper(string)
    // set return value
    args_str[0]->toUpperCase();
    *value_str  =  *args_str[0];
    return PARSER_STRING;
  }
  else if ( fname == F("lower") && num_args == 1 ) {
    // example of the lower(string)
    // set return value
    args_str[0]->toLowerCase();
    *value_str  =  *args_str[0];
    return PARSER_STRING;
  }
  else if ( fname == F("instr") && num_args == 2 ) {
    // example of the instr(string, string)
    // set return value
    *value  = args_str[0]->indexOf(*args_str[1]) + 1;
    return PARSER_TRUE;
  }
  else if ( fname == F("hex") && num_args == 1 ) {
    // example of the hex(value)
    // set return value
    *value_str  = String((int) args[0], HEX);
    return PARSER_STRING;
  }
  else if ( fname == F("oct") && num_args == 1 ) {
    // example of the oct(value)
    // set return value
    *value_str  = String((int) args[0], OCT);
    return PARSER_STRING;
  }
  else if ( fname == F("int") && num_args == 1 ) {
    // example of the int(value)
    // set return value
    *value  = (int) args[0];
    return PARSER_TRUE;
  }
  else if ( fname == F("chr") | fname == F("chr$") && num_args == 1 ) {
    // example of the chr(value)
    // set return value
    *value_str  = String((char) args[0]);
    return PARSER_STRING;
  }
  else if ( fname == F("asc") && num_args == 1 ) {
    // example of the asc(string) -> return the ascii code of the 1st char
    // set return value
    *value = (*args_str[0])[0];
    return PARSER_TRUE;
  }
  else if ( fname == F("replace") && num_args == 3 ) {
    // example of the replace(string, string to search for, string to replacement for}
    // set return value
    args_str[0]->replace(*args_str[1], *args_str[2]);
    *value_str = *args_str[0];
    return PARSER_STRING;
  }
  else if ( fname == F("str") && num_args == 1 ) {
    // example of str(number) converts the number to string
    // set return value
    *value_str = FloatToString(args[0]);
    return PARSER_STRING;
  }
  else if ( fname == F("wifi.scan") && num_args == 0 ) {
    // function wifi.scan() -> no arguments
    // set return value
    *value = WiFi.scanNetworks() + 1;
    return PARSER_TRUE;
  }
  else if ( fname == F("wifi.ssid") && num_args == 1 ) {
    // function wifi.ssid(number)
    // set return value
    *value_str =  String(WiFi.SSID(args[0] - 1));
    return PARSER_STRING;
  }
  else if ( fname == F("wifi.rssi") && num_args == 1 ) {
    // function wifi.rssi(number)
    // set return value
    *value_str =  String(WiFi.RSSI(args[0] - 1));
    return PARSER_STRING;
  }
  else if ( fname == F("ip") && num_args == 0 ) {
    // function wifi.scan() -> no arguments
    // set return value
    *value_str =  String(WiFi.localIP().toString());
    return PARSER_STRING;
  }
  else if ( fname == F("wget") && num_args > 0 ) {
    // function wget(url) or wget (url, port)
    // set return value
    if (num_args == 1)  *value_str  =  FetchWebUrl(*args_str[0], 80);
    else if (num_args == 2 )   *value_str  =  FetchWebUrl(*args_str[0], args[1]);
    return PARSER_STRING;
  }

  else if ( fname == F("sendts") && num_args == 2 ) {
    // function sendts(url, field)
    // set return value
    FetchWebUrl(String(F("api.thingspeak.com/update?key=")) + *args_str[0] + "&field" + *args_str[1] + "=" + *args_str[2], 80);
    return PARSER_STRING;
  }

  else if ( fname == F("readts") && num_args == 2 ) {
    // function readts(url, field)
    // set return value

    String MyOut =  FetchWebUrl(String(F("api.thingspeak.com/channels/")) + *args_str[1] + "/field/" + *args_str[2] + "/last.xml?api_key=" + *args_str[0], 80);
    MyOut = MyOut.substring(MyOut.indexOf(String("<field" + *args_str[2] + ">") ) + 8, MyOut.indexOf(String("</field" + *args_str[2] + ">")));
    *value_str = MyOut;
    return PARSER_STRING;
  }
  else if ( fname == F("readopenweather") && num_args == 2 ) {
    // function readopenweather(url, index)
    // set return value
    *value_str  =  FetchOpenWeatherMapApi(*args_str[0], String(args[1]));
    return PARSER_STRING;
  }
  else if ( fname == F("json") && num_args == 2 ) {
    // function json(buffer, key)
    // set return value
    *value_str  =  Parsifal(*args_str[0], *args_str[1]);
    return PARSER_STRING;
  }
  else if ( fname == F("io") && num_args > 0 ) {
    // function json(buffer, key)
    // set return value
    *value_str  =  UniversalPinIO(*args_str[0], *args_str[1],  args[2]);
    return PARSER_STRING;
  }
  else if ( fname == F("neo") && num_args > 0 ) {
    // function neo(led no, r, g, b)
    // set return value
    pixels.setPixelColor(args[0], pixels.Color(args[1], args[2], args[3]));
    pixels.show();
    return PARSER_STRING;
  }
  else if ( fname == F("neostripcolor") && num_args > 0 ) {
    // function json(buffer, key)
    // set return value
    for (int LedNo = args[0]; LedNo <= args[1]& LedNo < 255  ; LedNo++) {
      pixels.setPixelColor(LedNo, pixels.Color(args[2], args[3], args[4]));
      delay(0);
    }
    pixels.show();
    return PARSER_STRING;
  }
  else if ( fname == F("temp") && num_args > 0 ) {
    // function temp(sensor #)
    // set return value
    *value  = sensors.getTempCByIndex(args[0]);
    return PARSER_TRUE;
  }

  else if ( fname == F("dht.temp") ) {
    // function dht.temp()
    // set return value
    *value  = dht.readTemperature();
    return PARSER_TRUE;
  }
  else if ( fname == F("dht.hum") ) {
    // function dht.hum()
    // set return value
    *value  = dht.readHumidity();
    return PARSER_TRUE;
  }
  else if ( fname == F("dht.heatindex") ) {
    // function dht.heatindex()
    // set return value
    *value  = dht.computeHeatIndex(dht.readTemperature(), dht.readHumidity(), false);
    return PARSER_TRUE;
  }
  
  else if ( fname == F("neocls") && num_args == 0 ) {
    // function json(buffer, key)
    // set return value
    for (int LedNo = 0; LedNo < 255  ; LedNo++) {
      pixels.setPixelColor(LedNo, pixels.Color(0, 0, 0));
      delay(0);
    }
    pixels.show();
    return PARSER_STRING;
  }
  else if ( fname == F("i2c.write") && num_args == 1 ) {
    // function json(buffer, key)
    // set return value
    *value_str  =  String(Wire.write((int)args[0]));
    return PARSER_STRING;
  }
  else if ( fname == F("i2c.end") && num_args == 0 ) {
    // function json(buffer, key)
    // set return value
    *value_str  =  String(Wire.endTransmission());
    return PARSER_STRING;
  }
  else if ( fname == F("i2c.requestfrom") && num_args > 0 ) {
    // function i2c.requestfrom(device id, key)
    // set return value
    *value_str  =  String(Wire.requestFrom((byte)args[0], (byte)args[1]));
    return PARSER_STRING;
  }

  else if ( fname == F("i2c.available") && num_args == 0 ) {
    // function i2c.requestfrom(device id, key)
    // set return value
    *value_str  =  String(Wire.available());
    return PARSER_STRING;
  }

  else if ( fname == F("i2c.read") && num_args == 0 ) {
    // function i2c.requestfrom(device id, key)
    // set return value
    *value_str  =  String(Wire.read());
    return PARSER_STRING;
  }
  else if ( fname == F("hextoint") && num_args == 1 ) {
    // function i2c.requestfrom(device id, key)
    // set return value
    *value_str  =  String(StrToHex(*args_str[0] ));
    return PARSER_STRING;
  }





  else if ( fname == F("htmlvar") && num_args > 0 ) {
    // function json(buffer, key)
    // set return value
    *value_str  = String(String(F("VARS|")) + String(LastVarNumberLookedUp));
    return PARSER_STRING;
  }
  else if ( fname == F("unixtime") || fname == F("time") ) {
    // function time(format) or unixtime(value, format)
    time_t now;
    String *ar =  args_str[0];
    if (fname == F("time"))
    {
      now = time(nullptr);
    }
    else
    {
      now = args[0];
      ar =  args_str[1];
    }
    // set return value
    *value_str = String(ctime(&now));

    if (ar != NULL)
    {
      ar->toUpperCase();
      ar->trim();
      ar->replace(F("TIME"),  String(Mid(*value_str, 11, 8)));
      ar->replace(F("DOW"),   String(Mid(*value_str, 0, 3)));
      ar->replace(F("MONTH"), String(Mid(*value_str, 4, 3)));
      ar->replace(F("DATE"),  String(Mid(*value_str, 8, 2)));
      ar->replace(F("HOUR"),  String(Mid(*value_str, 11, 2)));
      ar->replace(F("MIN"),   String(Mid(*value_str, 14, 2)));
      ar->replace(F("SEC"),   String(Mid(*value_str, 17, 2)));
      ar->replace(F("YEAR"),  String(Mid(*value_str, 20, 4)));
      // set return value
      *value_str = *ar;
    }


    return PARSER_STRING;
  }



  // failed to evaluate function, return false
  return PARSER_FALSE;
}


String makeMeAString(String StringToReturnFromThisFunction)
{
  return StringToReturnFromThisFunction;
}

