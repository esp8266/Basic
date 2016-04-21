#include "expression_parser_string.h"

extern char* _parser_error_msg;

String evaluate(String expr)
{
  //int status;
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
  parser_result = parse_expression_with_callbacks( expr.c_str(), variable_callback, function_callback, NULL, &numeric_value, string_value  );
  if (_parser_error_msg != NULL)
  {
    PrintAndWebOut(String(_parser_error_msg));
    return F("error");
  }
  if (parser_result == PARSER_STRING)
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
  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    if (AllMyVariables[i].getName() == Name)
    {
      *value =  atof(AllMyVariables[i].getVar().c_str());

      *value_str = AllMyVariables[i].getVar();
      //Serial.print("Variable "); Serial.print(Name); Serial.print(AllMyVaribles_format[i]);
      VariableLocated = 1;
      return AllMyVariables[i].Format; // returns the format of the variable : PARSER_TRUE if numeric, PARSER_STRING if string
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
  fname.toLowerCase();
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
    SaveDataToFile("TimeZone", String(args[0]));
    SaveDataToFile("DaylightSavings", String(args[1]));
    configTime(LoadDataFromFile("TimeZone").toFloat() * 3600, LoadDataFromFile("DaylightSavings").toInt(), "pool.ntp.org", "time.nist.gov");
    *value_str = "";
    return PARSER_STRING;
  }
  else if ( fname == F("mid") && num_args == 2 ) {
    // example of the mid(string, start)
    // set return value
    if (args_str[0] != NULL)  // we should trigger an error if the argument is not a string
    {
      *value_str = args_str[0]->substring((int) args[1] - 1);
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("MID() : The first argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("mid") && num_args == 3 ) {
    // example of the mid(string, start, end)
    // set return value
    if (args_str[0] != NULL)
    {
      *value_str = args_str[0]->substring((int) args[1] - 1, (int) (args[1] + args[2]) - 1 );
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("MID() : The first argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("right") && num_args == 2 ) {
    // example of the right(string, length)
    // set return value
    if (args_str[0] != NULL)
    {
      *value_str = args_str[0]->substring(args_str[0]->length() - (int) args[1]);
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("RIGHT() : The first argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("left") && num_args == 2 ) {
    // example of the left(string, length)
    // set return value
    if (args_str[0] != NULL)
    {
      *value_str = args_str[0]->substring(0, (int) args[1]);
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("LEFT() : The first argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("word") && num_args > 1 ) {
    // example of the word(string, length)
    // set return value
    char bla;
    if ( (args_str[0] == NULL) || ( (num_args == 3) && (args_str[2] == NULL)) )
    {
      PrintAndWebOut(F("WORD() : The first and 3rd argument must be string!"));
      return PARSER_FALSE;
    }
    if (num_args == 2) bla = ' ';
    if (num_args == 3) bla = *args_str[2]->c_str();
    *value_str = getValue(*args_str[0], bla  , (int) args[1] - 1 );
    *value = -1;
    return PARSER_STRING;
  }
  else if ( fname == F("len") && num_args == 1 ) {
    // example of the len(string)
    // set return value
    if (args_str[0] != NULL)  // we should trigger an error if the argument is not a string
    {
      *value  = args_str[0]->length();
      return PARSER_TRUE;
    }
    else {
      PrintAndWebOut(F("LEN() : The argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("upper") && num_args == 1 ) {
    // example of the upper(string)
    // set return value
    if (args_str[0] != NULL)  // we should trigger an error if the argument is not a string
    {
      args_str[0]->toUpperCase();
      *value_str  =  *args_str[0];
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("UPPER() : The argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("lower") && num_args == 1 ) {
    // example of the lower(string)
    // set return value
    if (args_str[0] != NULL)  // we should trigger an error if the argument is not a string
    {
      args_str[0]->toLowerCase();
      *value_str  =  *args_str[0];
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("LOWER() : The argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( (fname == F("instr") || fname == F("instrrev")) && ( (num_args == 2) || (num_args == 3) ) ) {
    // example of the instr(string, string)
    // set return value
    if ( (args_str[0] != NULL) && (args_str[1] != NULL))
    {
      if (fname == F("instr"))
      {
        i = (num_args == 3) ? args[2] - 1 : 0;
        *value  = args_str[0]->indexOf(*args_str[1], i) + 1;
      }
      else
      {
        i = (num_args == 3) ? args[2] - 1 : - 1;
        *value  = args_str[0]->lastIndexOf(*args_str[1], i) + 1;
      }
      return PARSER_TRUE;
    }
    else {
      PrintAndWebOut(F("INSTR() : Both arguments must be a string!"));
      return PARSER_FALSE;
    }
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
    if (args_str[0] != NULL)
    {
      *value = (*args_str[0])[0];
      return PARSER_TRUE;
    }
    else {
      PrintAndWebOut(F("ASC() : The argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("val") && num_args == 1 ) {
    // function val(string) -> return the numeric value of the string
    // set return value
    if (args_str[0] != NULL)
    {
      *value = (int) strtol( args_str[0]->c_str(), 0, 10);
      return PARSER_TRUE;
    }
    else {
      PrintAndWebOut(F("VAL() : The argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("hextoint") && num_args == 1 ) {
    // function hextoint(string) -> return the numeric value of the hex string
    // set return value
    if (args_str[0] != NULL)
    {
      *value = (int) strtol( args_str[0]->c_str(), 0, 16);
      return PARSER_TRUE;
    }
    else {
      PrintAndWebOut(F("HEXTOINT() : The argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("replace") && num_args == 3 ) {
    // example of the replace(string, string to search for, string to replacement for}
    // set return value
    if ( (args_str[0] != NULL) && (args_str[1] != NULL) && (args_str[2] != NULL) )
    {
      args_str[0]->replace(*args_str[1], *args_str[2]);
      *value_str = *args_str[0];
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("REPLACE() : All the arguments must be a string!"));
      return PARSER_FALSE;
    }
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
    if (WiFi.localIP() == INADDR_NONE)
      *value_str =  String(WiFi.softAPIP().toString());
    else
      *value_str =  String(WiFi.localIP().toString());
    return PARSER_STRING;
  }
  else if ( fname == F("wget") && num_args > 0 ) {
    // function wget(url) or wget (url, port)
    // set return value
    if (args_str[0] != NULL)
    {
      if (num_args == 1)  *value_str  =  FetchWebUrl(*args_str[0], 80);
      else if (num_args == 2 )   *value_str  =  FetchWebUrl(*args_str[0], args[1]);
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("WGET() : The first arguments must be a string!"));
      return PARSER_FALSE;
    }
  }

  else if ( fname == F("sendts") && num_args == 3 ) {
    // function sendts(url, field)
    // set return value
    if ( (args_str[0] != NULL) && (args_str[1] != NULL) && (args_str[2] != NULL) )
    {
      FetchWebUrl(String(F("api.thingspeak.com/update?key=")) + *args_str[0] + "&field" + *args_str[1] + "=" + *args_str[2], 80);
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("SENDTS() : All the arguments must be a string!"));
      return PARSER_FALSE;
    }
  }

  else if ( fname == F("readts") && num_args == 3 ) {
    // function readts(url, field)
    // set return value
    if ( (args_str[0] != NULL) && (args_str[1] != NULL) && (args_str[2] != NULL) )
    {
      String MyOut =  FetchWebUrl(String(F("api.thingspeak.com/channels/")) + *args_str[1] + "/field/" + *args_str[2] + "/last.xml?api_key=" + *args_str[0], 80);
      MyOut = MyOut.substring(MyOut.indexOf(String("<field" + *args_str[2] + ">") ) + 8, MyOut.indexOf(String("</field" + *args_str[2] + ">")));
      *value_str = MyOut;
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("READTS() : All the arguments must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("readopenweather") && num_args == 2 ) {
    // function readopenweather(url, index)
    // set return value
    if (args_str[0] != NULL)
    {
      *value_str  =  FetchOpenWeatherMapApi(*args_str[0], String(args[1]));
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("ReadOpenWeather() : The first argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("json") && num_args == 2 ) {
    // function json(buffer, key)
    // set return value
    if ( (args_str[0] != NULL) && (args_str[1] != NULL) )
    {
      *value_str  =  Parsifal(*args_str[0], *args_str[1]);
      return PARSER_STRING;
    }
    else {
      PrintAndWebOut(F("JSON() : Both arguments must be a string!"));
      return PARSER_FALSE;
    }
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
    for (int LedNo = args[0]; LedNo <= args[1]& LedNo < 512  ; LedNo++) {
      pixels.setPixelColor(LedNo, pixels.Color(args[2], args[3], args[4]));
      delay(0);
    }
    pixels.show();
    return PARSER_STRING;
  }
  else if ( fname == F("neocls") && num_args == 0 ) {
    // function json(buffer, key)
    // set return value
    for (int LedNo = 0; LedNo < 512  ; LedNo++) {
      pixels.setPixelColor(LedNo, pixels.Color(0, 0, 0));
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


  else if ( fname == F("i2c.begin") && num_args == 1 ) {
    // function i2c.begin(address)
    // set return value
    Wire.beginTransmission((byte)args[0]);
    *value_str  =  String(F(""));
    return PARSER_STRING;
  }
  else if ( fname == F("i2c.write") && num_args == 1 ) {
    // function i2c.write(byte to be written)
    // set return value
    *value_str  =  String(Wire.write((byte)args[0]));
    return PARSER_STRING;
  }
  else if ( fname == F("i2c.end") && num_args == 0 ) {
    // function i2c.end()
    // set return value
    *value_str  =  String(Wire.endTransmission());
    return PARSER_STRING;
  }
  else if ( fname == F("i2c.requestfrom") && num_args > 0 ) {
    // function i2c.requestfrom(address, qty)
    // set return value
    *value_str  =  String(Wire.requestFrom((byte)args[0], (byte)args[1]));
    return PARSER_STRING;
  }

  else if ( fname == F("i2c.available") && num_args == 0 ) {
    // function i2c.available()
    // set return value
    *value_str  =  String(Wire.available());
    return PARSER_STRING;
  }

  else if ( fname == F("i2c.read") && num_args == 0 ) {
    // function i2c.read()
    // set return value
    *value_str  =  String(Wire.read());
    return PARSER_STRING;
  }
  else if ( fname == F("htmlvar") && num_args > 0 ) {
    // function json(buffer, key)
    // set return value
    *value_str  = String(String(F("VARS|")) + String(LastVarNumberLookedUp));
    return PARSER_STRING;
  }
  else if ( fname == F("unixtime") || fname == F("time") ) {
    // function time(format) or unixtime(value, format)  - value can be string or number
    time_t now;
    String *ar =  args_str[0];
    if (fname == F("time"))
    {
      now = time(nullptr);
    }
    else
    {
      if (ar != NULL) // the input value is a string; we convert it to number before
        now = strtol( args_str[0]->c_str(), 0, 10);
      else
        now = args[0];
      ar =  args_str[1];
    }
    // set return value
    *value_str = String(ctime(&now));

    if (ar != NULL)
    {
      ar->toUpperCase();
      ar->trim();
      ar->replace(F("TIME"),  value_str->substring( 11, 19));
      ar->replace(F("DOW"),   value_str->substring(  0, 3));
      ar->replace(F("MONTH"), value_str->substring(  4, 7));
      ar->replace(F("DATE"),  value_str->substring(  8, 10));
      ar->replace(F("HOUR"),  value_str->substring( 11, 13));
      ar->replace(F("MIN"),   value_str->substring( 14, 16));
      ar->replace(F("SEC"),   value_str->substring( 17, 19));
      ar->replace(F("YEAR"),  value_str->substring( 20, 24));
      // set return value
      *value_str = *ar;
    }
    return PARSER_STRING;
  }
  else if ( (i = Search_Array(fname)) != -1) // check if is an array
  {
    if (basic_arrays[i].Format == PARSER_STRING) // string format
    {
      *value_str = basic_arrays[i].getString(args[0]);
      return PARSER_STRING;
    }
    else
    {
      *value = basic_arrays[i].getFloat(args[0]);
      return PARSER_TRUE;
    }
  }


  // failed to evaluate function, return false
  return PARSER_FALSE;
}


String makeMeAString(String StringToReturnFromThisFunction)
{
  return StringToReturnFromThisFunction;
}

