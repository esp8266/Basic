#define TFT_DEMO
#include "expression_parser_string.h"


char bmp_buff[320 * 4];
extern char* _parser_error_msg;
extern String args_var[PARSER_MAX_ARGUMENT_COUNT];

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
    SendErrorMsg(String(_parser_error_msg));
    return F("error");
  }
  if (parser_result == PARSER_STRING)
    return string_value;
  else
    return FloatToString(numeric_value);

}

 #if defined(BASIC_TFT)
#include "Fonts/FreeSerifBold9pt7b.h"
#include "Fonts/FreeSerifBold12pt7b.h"
#include "Fonts/FreeSerifBold18pt7b.h"
#include "Fonts/FreeSerifBold24pt7b.h"
#endif





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
  LastVarNumberLookedUp = 0;
  for (int i = 0; i < TotalNumberOfVariables; i++)
  {
    if (AllMyVariables[i].getName() == Name)
    {
      LastVarNumberLookedUp = i;
      *value =  atof(AllMyVariables[i].getVar().c_str());

      *value_str = AllMyVariables[i].getVar();
      //Serial.print("Variable "); Serial.print(Name); Serial.print(AllMyVaribles_format[i]);
      VariableLocated = 1;
      return AllMyVariables[i].Format; // returns the format of the variable : PARSER_TRUE if numeric, PARSER_STRING if string
    }
  }
  Name.toUpperCase();
  // now look for the constants
  if (Name == F("D0")) { *value = 16; return PARSER_TRUE;}
  if (Name == F("D1")) { *value =  5; return PARSER_TRUE;}
  if (Name == F("D2")) { *value =  4; return PARSER_TRUE;}
  if (Name == F("D3")) { *value =  0; return PARSER_TRUE;}
  if (Name == F("D4")) { *value =  2; return PARSER_TRUE;}
  if (Name == F("D5")) { *value = 14; return PARSER_TRUE;}
  if (Name == F("D6")) { *value = 12; return PARSER_TRUE;}
  if (Name == F("D7")) { *value = 13; return PARSER_TRUE;}
  if (Name == F("D8")) { *value = 15; return PARSER_TRUE;}
  if (Name == F("RX")) { *value =  3; return PARSER_TRUE;}
  if (Name == F("TX")) { *value =  1; return PARSER_TRUE;}


  // failed to find variable, return false
  *value = 0;
  *value_str = name;
  // against my will, I have been obliged to put a PARSER_STRING here instead of PARSER_FALSE; this will inhibit the error message if the
  // variable is not existing permitting to use any variable not initialized; :-)
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
  else if ( fname == F("io") && num_args > 0 ) {
    // function json(buffer, key)
    // set return value
    *value  =  UniversalPinIO(*args_str[0], String(args[1]),  args[2]);
    return PARSER_TRUE;
  }
  
  
  
  else if ( fname == F("telnet.client.connect") && num_args >= 1 ) {
    // Tellnet client connection setup.
	if (args_str[0] != NULL & args[1] != NULL) 
	{
		*value  = clientTelnet.connect( String(*args_str[0]).c_str(), (int)args[1]);
		 return PARSER_TRUE;
	}
	else if (args_str[0] != NULL )
	{
		*value  = clientTelnet.connect(  String(*args_str[0]).c_str(), 23);
		 return PARSER_TRUE;
	}
	SendErrorMsg(F("Missing arguments!"));
    return PARSER_FALSE;
  }
  else if ( fname == F("telnet.client.available") && num_args == 0 ) {
    // Tellnet client connection setup.
	*value  = clientTelnet.available();
	 return PARSER_TRUE;
  }
  else if ( fname == F("telnet.client.read.chr") && num_args == 0 ) {
    // Tellnet client connection setup.
	*value_str  = "";
	if (clientTelnet.available()) *value_str = (char) clientTelnet.read();
    return PARSER_STRING;
  }
  else if ( fname == F("telnet.client.read.str") && num_args == 0 ) {
    // Tellnet client connection setup.
	*value_str  = "";
	while (clientTelnet.available())
	{
		*value_str += (char) clientTelnet.read();
		delay(0);
	}
    return PARSER_STRING;
  }
  else if ( fname == F("telnet.client.write") && num_args == 1 ) {
    // Tellnet client connection setup.
	if (args_str[0] != NULL) 
	{
		*value  = clientTelnet.write( String(*args_str[0]).c_str(),  String(*args_str[0]).length());
		return PARSER_TRUE;
	}
	
  }  
  else if ( fname == F("telnet.client.stop") && num_args == 0 ) {
    // Tellnet client connection setup.
	*value  = 0;
	clientTelnet.stop();
	 return PARSER_TRUE;
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
  else if ( fname == F("version") && num_args == 0 ) {
    // example of the id()
    // set return value
    *value_str = String(BasicVersion);
    return PARSER_STRING;
  }
  else if ( fname == F("htmlid") && num_args == 0 ) {
    // example of the htmlid()
    // set return value
    *value_str = String(LastElimentIdTag);
    return PARSER_STRING;
  }

  else if ( (fname == F("timesetup") | fname == F("time.setup")) && num_args > 0 ) {
    String bla;
    SaveDataToFile("TimeZone", String(args[0]));
	if (num_args >= 2) {SaveDataToFile("DaylightSavings", String(args[1])); }else {SaveDataToFile("DaylightSavings", String(F("0")));}
	if (num_args == 3)
	{
		SaveDataToFile("TimeServer", String(args[2]));
		configTime(LoadDataFromFile("TimeZone").toFloat() * 3600, LoadDataFromFile("DaylightSavings").toInt(), LoadDataFromFile("TimeServer").c_str(), "pool.ntp.org"); 		
	}
	else
	{
		configTime(LoadDataFromFile("TimeZone").toFloat() * 3600, LoadDataFromFile("DaylightSavings").toInt(), "pool.ntp.org", "time.nist.gov");
	}
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
      SendErrorMsg(F("MID() : The first argument must be a string!"));
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
      SendErrorMsg(F("MID() : The first argument must be a string!"));
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
      SendErrorMsg(F("RIGHT() : The first argument must be a string!"));
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
      SendErrorMsg(F("LEFT() : The first argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("word") && num_args > 1 ) {
    // example of the word(string, length)
    // set return value
    char bla;
    if ( (args_str[0] == NULL) || ( (num_args == 3) && (args_str[2] == NULL)) )
    {
      SendErrorMsg(F("WORD() : The first and 3rd argument must be string!"));
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
      SendErrorMsg(F("LEN() : The argument must be a string!"));
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
      SendErrorMsg(F("UPPER() : The argument must be a string!"));
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
      SendErrorMsg(F("LOWER() : The argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("trim") && num_args == 1 ) {
    // example of the lower(string)
    // set return value
    if (args_str[0] != NULL)  // we should trigger an error if the argument is not a string
    {
      args_str[0]->trim();
      *value_str  =  *args_str[0];
      return PARSER_STRING;
    }
    else {
      SendErrorMsg(F("LOWER() : The argument must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("msgget") && num_args == 1 ) {
    // example of the lower(string)
    // set return value


    if (args_str[0] != NULL)  // we should trigger an error if the argument is not a string
    {
      int str_len = args_str[0]->length() + 1;
      char MgetToTest[str_len];
      args_str[0]->toCharArray(MgetToTest, str_len);
      *value_str  =  GetRidOfurlCharacters(server->arg( MgetToTest  ));
      return PARSER_STRING;
    }
    else {
      SendErrorMsg(F("msgget() : The argument must be a string!"));
      return PARSER_FALSE;
    }
  }

  else if ( fname == F("serial.read.int") && num_args == 0 ) {
    if ( Serial.available() )
    {
      *value  = Serial.read();
      return PARSER_TRUE;
    }
    else {
	  *value = -1;
      return PARSER_TRUE;
    }
  }

    else if ( fname == F("serial.read.chr") && num_args == 0 ) {
    if ( Serial.available() )
    {
      *value_str  = (char) Serial.read();
      return PARSER_STRING;
    }
    else {
	  *value_str  = "-1";
      return PARSER_STRING;
    }
  }

    else if ( fname == F("serial.available") && num_args == 0 ) {

	*value  =  Serial.available();
    return PARSER_TRUE;
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
      SendErrorMsg(F("INSTR() : Both arguments must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("hex") ) {
    if ( num_args == 1 ) {
      // example of the hex(value)
      // set return value
      *value_str  = String((int) args[0], HEX);
      return PARSER_STRING;
    }
    else if ( num_args == 2 ) {  // example of the hex(value, nb_digits)
      String tmp = "0000000" + String((int) args[0], HEX);
      *value_str = tmp.substring(tmp.length() - args[1]);
      return PARSER_STRING;
    }
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
    *value_str = String(" ");
    char *p = (char*) value_str->c_str();
    *p = (char) args[0];
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
      SendErrorMsg(F("ASC() : The argument must be a string!"));
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
      SendErrorMsg(F("VAL() : The argument must be a string!"));
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
      SendErrorMsg(F("HEXTOINT() : The argument must be a string!"));
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
      SendErrorMsg(F("REPLACE() : All the arguments must be a string!"));
      return PARSER_FALSE;
    }
  }

  else if ( fname == F("write") && num_args == 2 ) {
	  delay(0);
	  *value_str = *args_str[0];
	  
	  Serial.println("Done writing data");
	  if (args_str[1] != NULL)  
	  {
		  SaveDataToFile(*args_str[0], *args_str[1]);
		  return PARSER_STRING;
	  }
	  
	  
	  if (args[1] != NULL) 
	  {
		  SaveDataToFile(*args_str[0], FloatToString(args[1]));
		  return PARSER_STRING;
	  }	  
	  


  }
  else if ( fname == F("mqtt.setup") && num_args >= 1 ) {
	int MQTTport = 1883;
	
	if (num_args == 2) MQTTport = args[1];
	
	//MQTTclient.setServer(String(*args_str[0]).c_str(), MQTTport);
	MQTTclient.setServer("broker.mqtt-dashboard.com", MQTTport);
    
	MQTTActivated = 1;
	Serial.println(String("Connected to " + String(*args_str[0])));
    *value_str = *args_str[0];
    return PARSER_STRING;
  }
  else if ( fname == F("mqtt.subscribe") && num_args >= 1 ) {
    MQTTSubscribeTopic = *args_str[0];
    //MQTTclient.subscribe(String(*args_str[0]).c_str());
    *value_str = *args_str[0];
    return PARSER_STRING;
  }  
  else if ( fname == F("mqtt.msg")) {
    *value_str = MQTTlatestMsg;
    return PARSER_STRING;
  }     
  else if ( fname == F("mqtt.publish")  && num_args >= 1 ) {
	MQTTPublishTopic = *args_str[0];
    MQTTPublishMSG = *args_str[1];
    *value_str = MQTTlatestMsg;
    return PARSER_STRING;
  }    
  
  
  
  

  else if ( fname == F("read") && num_args == 1 ) {
	  *value_str = "";
    *value_str = LoadDataFromFile(*args_str[0]);
    return PARSER_STRING;
  }
  else if ( fname == F("read.val") && num_args == 1 ) {
    *value = LoadDataFromFile(*args_str[0]).toInt();
    return PARSER_TRUE;
  }
  else if ( fname == F("del.dat") && num_args == 1 ) {
	  *value_str = "";
	  SPIFFS.remove(String("/data/" + *args_str[0] + ".dat"));
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
	numberOfWifiScanResults = WiFi.scanNetworks() ;
    *value = numberOfWifiScanResults;
    return PARSER_TRUE;
  }
  else if ( fname == F("wifi.ssid") && num_args == 1 ) {
    // function wifi.ssid(number)
    // set return value
	if (numberOfWifiScanResults < args[0])return PARSER_STRING;
    *value_str =  String(WiFi.SSID(args[0] - 1));
    return PARSER_STRING;
  }
  else if ( fname == F("wifi.rssi") && num_args == 1 ) {
    // function wifi.rssi(number)
    // set return value
	if (numberOfWifiScanResults < args[0])return PARSER_STRING;
    *value_str =  String(WiFi.RSSI(args[0] - 1));
    return PARSER_STRING;
  }
  else if ( fname == F("wifi.bssid") && num_args == 1 ) {
    // function wifi.rssi(number)
    // set return value
	if (numberOfWifiScanResults < args[0])return PARSER_STRING;
    *value_str =  String(WiFi.BSSIDstr(args[0] - 1));
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
  else if ( fname == F("wifi.connect") && num_args >= 0 ) {
    // function wifi.connect(ap name, ap pass, ip, gateway, subnet)
    // set return value
	if ( (args_str[0] != NULL) && (args_str[1] != NULL) && (args_str[2] != NULL) && (args_str[3] != NULL) && (args_str[4] != NULL) ) 
	{
		*value =  ConnectToTheWIFI(*args_str[0],*args_str[1],*args_str[2],*args_str[3],*args_str[4] );
		return PARSER_TRUE;
	}	
	if ( (args_str[0] != NULL) && (args_str[1] != NULL) ) 
	{
		*value =  ConnectToTheWIFI(*args_str[0],*args_str[1], "", "", "");
        return PARSER_TRUE;
	}
	if ( (args_str[0] != NULL)) 
	{
		*value =  ConnectToTheWIFI(*args_str[0],"", "", "", "");
        return PARSER_TRUE;
	}
	*value =  ConnectToTheWIFI("","", "", "", "");
	return PARSER_TRUE;
  }  
  else if ( fname == F("wifi.ap") && num_args >= 0 ) {
    // function wifi.ap(ap name, ap pass, ip, gateway, subnet)
    // set return value
	if ( (args_str[0] != NULL) && (args_str[1] != NULL) && (args_str[2] != NULL) && (args_str[3] != NULL) && (args_str[4] != NULL) ) 
	{
		*value =  CreateAP(*args_str[0],*args_str[1],*args_str[2],*args_str[3],*args_str[4] );
		return PARSER_TRUE;
	}	
	if ( (args_str[0] != NULL) && (args_str[1] != NULL) ) 
	{
		*value =  CreateAP(*args_str[0],*args_str[1], "", "", "");
        return PARSER_TRUE;
	}
	if ( (args_str[0] != NULL) ) 
	{
		*value =  CreateAP(*args_str[0],"", "", "", "");
        return PARSER_TRUE;
	}
	*value =  CreateAP("","", "", "", "");
	return PARSER_TRUE;	
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
      SendErrorMsg(F("WGET() : The first arguments must be a string!"));
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
      SendErrorMsg(F("SENDTS() : All the arguments must be a string!"));
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
      SendErrorMsg(F("READTS() : All the arguments must be a string!"));
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
      SendErrorMsg(F("ReadOpenWeather() : The first argument must be a string!"));
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
      SendErrorMsg(F("JSON() : Both arguments must be a string!"));
      return PARSER_FALSE;
    }
  }
  else if ( fname == F("neo.setup") && num_args == 1 ) {
    // function neosetup(pin)
    if (pixels != NULL)
      delete pixels;
    pixels = new Adafruit_NeoPixel(512, args[0], NEO_GRB + NEO_KHZ800);
    pixels->begin();
    return PARSER_STRING;
  }
 else if ( fname == F("neo") && num_args > 0 ) {
    // function neo(led no, r, g, b ,flag)
    // if flag == 1 then dont display neopixel, just set it in memory
    if (pixels == NULL)
    {
      pixels = new Adafruit_NeoPixel(512, 15, NEO_GRB + NEO_KHZ800);
      pixels->begin();
    }
    pixels->setPixelColor(args[0], pixels->Color(args[1], args[2], args[3]));
    if (args[4] != 1)
    {
    pixels->show();
    }
    return PARSER_STRING;
  }
    
   else if ( fname == F("neo.shift") && num_args > 0 ) {
    // ****
    // neoshift(first pixel 0 -511,last pixel 0 to 511 ,direction -1 or +1)
    // *****

    //test if args[] out of range
    if (args[0] > args[1] || args[0]>511 || args[1]>511)
    {
      SendErrorMsg(F("Neoshift error"));
      return PARSER_FALSE;
    }
    if (pixels == NULL)
    {
      pixels = new Adafruit_NeoPixel(512, 15, NEO_GRB + NEO_KHZ800);
      pixels->begin();
    }
    // move pixels 
    
    if (args[2] > 0){
    //move pixels up one
    for (int zz = args[1]; zz>= args[0] ; zz--){
      uint32_t oldp = pixels->getPixelColor(zz);
      pixels->setPixelColor(zz+1,oldp);
      delay(0);
    }
    }
    else if (args[2]<0){
      // move down one
      for ( int zz = args[0]; zz <= args[1] ; zz++){
        uint32_t oldp = pixels->getPixelColor(zz);
      pixels->setPixelColor(zz-1,oldp);
      delay(0);
      }
    }
    pixels->show();
    return PARSER_STRING;
  } 
  else if ( fname == F("neostripcolor")| fname == F("neo.stripcolor") && num_args > 0 ) {
    // function neostripcolor(start, end, r, g, b)
    if (pixels == NULL)
    {
      pixels = new Adafruit_NeoPixel(512, 15, NEO_GRB + NEO_KHZ800);
      pixels->begin();
    }
    for (int LedNo = args[0]; (LedNo <= args[1]) && (LedNo < 512); LedNo++) {
      pixels->setPixelColor(LedNo, pixels->Color(args[2], args[3], args[4]));
      delay(0);
    }
    pixels->show();
    return PARSER_STRING;
  }
  else if ( fname == F("neocls") |  fname == F("neo.cls") && num_args == 0 ) {
    // function neocls()
    // set return value
    if (pixels == NULL)
    {
      pixels = new Adafruit_NeoPixel(512, 15, NEO_GRB + NEO_KHZ800);
      pixels->begin();
    }
    for (int LedNo = 0; LedNo < 512  ; LedNo++) {
      pixels->setPixelColor(LedNo, pixels->Color(0, 0, 0));
      delay(0);
    }
    pixels->show();
    return PARSER_STRING;
  }
  else if (fname == F("neo.hex") && num_args > 0){
  // NEO HEX       (string of pixel info,first pixel,string start ie 0,1,2 will be multiplied by 3,pcount number pixels,percentage brightness 0-100)
  // pixels are 3 hex digits ie FFF will be white 000 will be black (coded as RGB)
    int hexlen ; //= args_str[0]->length();
    hexlen = args[3];
    int r,g,b;
    int zpix; // calculated pixel value
    String hz = "";
    for (int npix = 0;npix < hexlen;npix ++){
      zpix = (npix + args[2]) *3;
    hz = args_str[0]->substring(zpix,zpix+1);
    r = strtol(hz.c_str(),0,16);
    r = (r<<4) *args[4] /100;
        hz = args_str[0]->substring(zpix+1,zpix+2);
    g = strtol(hz.c_str(),0,16);
    g = (g<<4) * args[4] /100;
        hz = args_str[0]->substring(zpix+2,zpix+3);
    b = strtol(hz.c_str(),0,16);
    b = (b<<4) * args[4] /100;
     pixels->setPixelColor(npix + args[1], pixels->Color(r, g, b));
    //pixels->setPixelColor(LedNo, pixels->Color(r, g, b));
    }
    pixels ->show();
  return PARSER_TRUE;
  }
  
  
  
	else if ( fname == F("temp") && num_args < 2 ) {
		byte deviceAddr[8]; // create 8 byte storage area for device code
		String tmp="";
		if (num_args==0) {
			byte xx;
			// Return all connected sensor ROM codes as space-separated string (16 characters each)
			for (i=0; sensors.getAddress(deviceAddr,i); i++) // Loop while devices are found
			{
				delay(0);
				if (i>0) tmp.concat(" "); // If not the first device, add a separator space
				for (xx=0; xx<8; xx++) {
				if (deviceAddr[xx]<16) tmp.concat("0");
				tmp.concat(String(deviceAddr[xx], HEX));
				}
			}
				*value_str = tmp;
				return PARSER_STRING;
			}
			else 
			{
			sensors.requestTemperatures(); // Tell all sensors to convert analog to digital REGARDLESS of how we are addressing them
			if (args_str[0] != NULL) 
			{
				// Query a sensor by ROM code. Rom code is a 16 character hexadecimal string
				uint32_t longx;
				for (i=0; i<16; i+=2) 
				{
					delay(0);
					tmp = args_str[0]->substring(i,i+2);
					longx = strtoul(tmp.c_str(), NULL, 16);
					deviceAddr[i>>1] = longx;
				}
				// Read by device rom code and set the return value
				*value = sensors.getTempC(deviceAddr); // Perform the temperature read by device code
			}
			else 
			{
			// function temp(sensor #) - Read by sensor number and set the return value
			*value = sensors.getTempCByIndex(args[0]);
			}
		return PARSER_TRUE;
		}
	}
  else if (fname.startsWith(F("dht.")) )      // block DHT functions; this reduces the number of compares
  {
    fname = fname.substring(4); // skip the term dht.
    if ( fname == F("setup") ) // dht.setup(model, pin) -> model can be 11, 21, 22
    {
      // function dht.setup( model, pin)
      uint8_t pin;
      uint8_t model;
      switch ( num_args )
      {
        case 1:
          model = args[0];
          pin = 5;
          break;
        case 2:
          model = args[0];
          pin = args[1];
          break;
        default:
          model = 21; // default DHT21
          pin = 5; // default pin
          break;
      }
      if (dht != NULL)
        delete dht;
      dht = new DHT(pin, model);
      dht->begin();
      // set return value
      return PARSER_TRUE;
    }
    else if ( fname == F("temp") ) {
      // function dht.temp()
      // set return value
      if (dht == NULL)
      {
        dht = new DHT(5, 21);
        dht->begin();
      }
      *value  = dht->readTemperature();
      return PARSER_TRUE;
    }
    else if ( fname == F("hum") ) {
      // function dht.hum()
      // set return value
      if (dht == NULL)
      {
        dht = new DHT(5, 21);
        dht->begin();
      }
      *value  = dht->readHumidity();
      return PARSER_TRUE;
    }
    else if ( fname == F("heatindex") ) {
      // function dht.heatindex()
      // set return value
      if (dht == NULL)
      {
        dht = new DHT(5, 21);
        dht->begin();
      }
      *value  = dht->computeHeatIndex(dht->readTemperature(), dht->readHumidity(), false);
      return PARSER_TRUE;
    }
  }
  else if (fname.startsWith(F("i2c.")) )      // block I2C functions; this reduces the number of compares
  {
    fname = fname.substring(4); // skip the term i2c.
    if ( fname == F("begin") && num_args == 1 ) {
      // function i2c.begin(address)
      // set return value
      Wire.beginTransmission((byte)args[0]);
      *value_str  =  String(F(""));
      return PARSER_STRING;
    }
    else if ( fname == F("write") && num_args == 1 ) {
      // function i2c.write(byte to be written)
      // set return value
      *value_str  =  String(Wire.write((byte)args[0]));
      return PARSER_STRING;
    }
    else if ( fname == F("end") && num_args == 0 ) {
      // function i2c.end()
      // set return value
      *value_str  =  String(Wire.endTransmission());
      return PARSER_STRING;
    }
    else if ( fname == F("requestfrom") && num_args > 0 ) {
      // function i2c.requestfrom(address, qty)
      // set return value
      *value_str  =  String(Wire.requestFrom((byte)args[0], (byte)args[1]));
      return PARSER_STRING;
    }

    else if ( fname == F("available") && num_args == 0 ) {
      // function i2c.available()
      // set return value
      *value_str  =  String(Wire.available());
      return PARSER_STRING;
    }

    else if ( fname == F("read") && num_args == 0 ) {
      // function i2c.read()
      // set return value
      *value_str  =  String(Wire.read());
      return PARSER_STRING;
    }
    else if ( fname == F("setup") && num_args == 2 ) {
      // function i2c.read()
      // set return value
	  Wire.begin(args[0], args[1]);
	   #if defined(BASIC_TFT)
	  StartUp_OLED_AFTER_I2C_REMAP();
	  lcd.begin(16, 2);
	  #endif
      *value  =  1;
      return PARSER_TRUE;;
    }
	
	
	
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
      ar->replace(F("DAY"),   value_str->substring(  8, 10));
      ar->replace(F("HOUR"),  value_str->substring( 11, 13));
      ar->replace(F("MIN"),   value_str->substring( 14, 16));
      ar->replace(F("SEC"),   value_str->substring( 17, 19));
      ar->replace(F("YEAR"),  value_str->substring( 20, 24));
      // set return value
      *value_str = *ar;
    }
    return PARSER_STRING;
  }
  else if ( fname == F("gpio1reset") && num_args == 0 )
  {
    // function gpi01reset()  -> restore the normal functionality of the pin GPIO1 (TX)
    pinMode(1, SPECIAL);
    return PARSER_STRING;
  }
  else if (fname.startsWith(F("spi.")) )      // block SPI functions; this reduces the number of compares
  {
    fname = fname.substring(4); // skip the term spi.
    if ( fname == F("setup") && num_args > 0 ) {
      SPI.begin();
      switch(num_args)
      {
        case 1:  //spi.setup(speed)
          SPI.beginTransaction( SPISettings(args[0], MSBFIRST, SPI_MODE0));
          break;
        case 2:  //spi.setup(speed, SPI_MODE)
          SPI.beginTransaction( SPISettings(args[0], MSBFIRST, args[1]));
          break;
        case 3:  //spi.setup(speed, SPI_MODE, MSBFIRST)
          SPI.beginTransaction( SPISettings(args[0], args[2], args[1]));
          break;
      }
      // set return value
      *value = 0;
      return PARSER_TRUE;
    }
	
    else if ( fname == F("setfrequency") && num_args == 1 ) {    // let a = spi.write(value_number) -> send and receive a  single byte
	SPI.setFrequency(args[0]);
      *value = -1;
      return PARSER_TRUE;
    }	
    else if ( fname == F("setmode") && num_args == 1 ) {    // let a = spi.write(value_number) -> send and receive a  single byte
	SPI.setDataMode(args[0]);
      *value = -1;
      return PARSER_TRUE;
    }		
	
	
	
    else if ( fname == F("byte") && num_args == 1 ) {    // let a = spi.write(value_number) -> send and receive a  single byte
      *value = SPI.transfer(args[0]);
      return PARSER_TRUE;
    }
    else if ( fname == F("string") && num_args == 2 ) { // let a$ = spi.string(value_string, length) -> send and receive a buffer of 'length' bytes
      String ret;
      char c;
      ret.reserve(args[1]);
      for (i = 0; i < args[1]; i++)
      {
        c = SPI.transfer(args_str[0]->c_str()[i]);
        ret.concat(c);
      }
      *value_str = ret;
      return PARSER_STRING;
    }
    else if ( fname == F("hex") && num_args == 2 ) { // let a$ = spi.hex(value_string, length) -> send and receive a buffer of 'length' bytes; each byte is a 2 char hex string
      if (args_str[0] == NULL)  {
        SendErrorMsg(F("spi.hex() : First argument must be a string!"));
        return PARSER_FALSE;
      }
      String ret;
      char c, t;
      String s;
      ret.reserve(args[1] * 2); // each received byte is 2 chars
      for (i = 0; i < (args[1] * 2); i += 2)
      {
        s = args_str[0]->substring(i , i + 2);
        t = (char) strtol( s.c_str(), 0, 16);
        c = SPI.transfer(t);
        s = String( c, HEX);
        if (s.length() == 1) s = "0" + s;
        ret.concat(s);
      }
      *value_str = ret;
      return PARSER_STRING;
    }
  }
  else if ( fname == F("eval") && num_args == 1) {
    if (args_str[0] == NULL)  {
      SendErrorMsg(F("eval() : The argument must be a string!"));
      return PARSER_FALSE;
    }
    evaluate(*args_str[0]);
    *value_str = string_value;
    *value = numeric_value;
    return parser_result;
  }
  else if (fname.startsWith(F("ir.")) )      // block Infrared functions; this reduces the number of compares
  {
    fname = fname.substring(3); // skip the term ir.
    if ( fname == F("send.setup") && num_args == 1 ) {
      // function ir.send.init(pin)   define the pin to be used for the irsend
      // set return value
      if (irsend != NULL)
        delete irsend;
      irsend = new IRsend(args[0]);
      irsend->begin();
      return PARSER_STRING;
    }
    else if ( fname == F("send") && num_args == 1 ) {
      // function ir.send.nec(code, len)   send a NEC code with the first argument and the lenght on the 2nd
      if (args_str[0] == NULL)  {
        SendErrorMsg(F("ir.send() : Argument must be a string!"));
        return PARSER_FALSE;
      }
	  String IRhexCodeForOutput = getValue(*args_str[0], ':' ,0);
	  String IRmfgCodeForOutput = getValue(*args_str[0], ':' ,1);
	  byte IRbytesCodeForOutput = getValue(*args_str[0], ':' ,2).toInt();
	  delay(0);
	  
	  if (IRmfgCodeForOutput == "NEC") irsend->sendNEC(HexToLongInt(IRhexCodeForOutput), (int) IRbytesCodeForOutput);
	  if (IRmfgCodeForOutput == "SONY") irsend->sendSony(HexToLongInt(IRhexCodeForOutput), (int) IRbytesCodeForOutput);
      return PARSER_STRING;
    }
    else if ( fname == F("send.nec") && num_args == 2 ) {
      // function ir.send.nec(code, len)   send a NEC code with the first argument and the lenght on the 2nd
      if (args_str[0] == NULL)  {
        SendErrorMsg(F("ir.send.nec() : The first argument must be a string!"));
        return PARSER_FALSE;
      }
      irsend->sendNEC(HexToLongInt(*args_str[0]), (int) args[1]);
      return PARSER_STRING;
    }
    else if ( fname == F("send.sony") && num_args == 2 ) {
      // function ir.send.nec(code, len)   send a SONY code with the first argument and the lenght on the 2nd
      if (args_str[0] == NULL)  {
        SendErrorMsg(F("ir.send.sony() : The first argument must be a string!"));
        return PARSER_FALSE;
      }
      irsend->sendSony(HexToLongInt(*args_str[0]), (int) args[1]);
      return PARSER_STRING;
    }
    else if ( fname == F("recv.setup") && num_args == 1 ) {
      // function ir.recv.init(pin)   define the pin to be used for the irsend
      // set return value
      if (irrecv != NULL)
        delete irrecv;
      irrecv = new IRrecv(args[0]);
      irrecv->enableIRIn(); // Start the receiver
      return PARSER_STRING;
    }
    else if ( fname == F("recv.get") && num_args == 0 ) {
      // function ir.recv.get()   gets the code received
      // set return value
      *value_str = String(IRresults.value, HEX);
      irrecv->resume(); // Receive the next value
      return PARSER_STRING;
    }
    else if ( fname == F("recv.full") && num_args == 0 ) {
      // function ir.recv.type()   gets the type of RC received
      // set return value
      *value_str =   String(IRresults.value, HEX) + ":" + IRtype(IRresults.decode_type) + ":" + String(IRresults.bits);
      irrecv->resume(); // Receive the next value
      return PARSER_STRING;
    }
    else if ( fname == F("recv.dump") && num_args == 0 ) {
      // function ir.recv.dump()   verbose the code received permitting to identify the remote controller
      // set return value
      IRdump(&IRresults);
      *value_str = String(IRresults.value, HEX);
      irrecv->resume(); // Receive the next value
      return PARSER_STRING;
    }
  }
  
  
#if defined(BASIC_TFT)
    else if (fname.startsWith(F("oled.")) )      // block TFT functions; this reduces the number of compares
  {
    fname = fname.substring(5); // skip the term tft.
    if ( fname == F("color") && num_args == 1 ) {
      // function tft.fill(color)   fill the screen with the color (565)
	  if (args[0] == 0) display.setColor(BLACK);
	  if (args[0] == 1) display.setColor(WHITE);
      return PARSER_TRUE;
    }
    else if ( fname == F("cls") && num_args == 0 ) {
      // function tft.fill(color)   fill the screen with the color (565)
      display.clear();
	  display.display();
      return PARSER_TRUE;
    }
    else if ( fname == F("line") && num_args == 4) {
      // function tft.rect(x, y, width, height, color)
      display.drawLine(args[0], args[1], args[2], args[3]);
	  display.display();
      return PARSER_TRUE;
    }	
    else if ( fname == F("rect") && num_args == 4) {
      // function tft.rect(x, y, width, height, color)
      display.drawRect(args[0], args[1], args[2], args[3]);
	  display.display();
      return PARSER_TRUE;
    }
    else if ( fname == F("rect.fill") && num_args == 4 ) {
      // function tft.rect.fill(x, y, width, height, color)
      display.fillRect(args[0], args[1], args[2], args[3]);
	  display.display();
      return PARSER_TRUE;
    }
    else if ( fname == F("circle") && num_args == 3 ) {
      // function tft.circle(x, y, radius, color)
      display.drawCircle(args[0], args[1], args[2]);
	  display.display();
      return PARSER_TRUE;
    }
    else if ( fname == F("circle.fill") && num_args == 3 ) {
      // function tft.circle.fill(x, y, radius, color)
      display.fillCircle(args[0], args[1], args[2]);
	  display.display();
      return PARSER_TRUE;
    }

    else if ( fname == F("font") && num_args == 1 ) {
      // function tft.circle.fill(x, y, radius, color)
	  if (args[0] == 10) display.setFont(ArialMT_Plain_10);
	  if (args[0] == 16) display.setFont(ArialMT_Plain_16);
	  if (args[0] == 24) display.setFont(ArialMT_Plain_24);
      return PARSER_TRUE;
    }
	
    else if ( fname == F("print") && num_args >= 1) {
      // function tft.text.size(size)
      if (args[1] != NULL & args[2] !=NULL & args_str[0] != NULL) {
		  display.drawString(args[1], args[2], *args_str[0]);
		  display.display();
		  }
      else if ( args_str[1] != NULL) {
		  display.drawString(0, 0, *args_str[0]);
		  display.display();
		  }	  
	display.display();
      return PARSER_STRING;
    }  
  }
  

  else if (fname.startsWith(F("tft.")) )      // block TFT functions; this reduces the number of compares
  {
    fname = fname.substring(4); // skip the term tft.
      if ( fname == F("setup") && num_args >= 2 ) {
      // function tft.init(TFT_CS, TFT_DC, Rotation)   init the ILI9341 display; Rotation can be from 0 to 3
      // create the class if not defined
      if (tft == NULL)
        {
        tft = new Adafruit_ILI9341(args[0], args[1]);
        }
        TFT_CS_pin = args[0];
        TFT_DC_pin = args[1];
      tft->begin();
      if (num_args > 2)
        tft->setRotation(args[2]);
      tft->fillScreen(ILI9341_BLACK);
        form1.tft = tft;
      return PARSER_TRUE;
    }
    else if ( fname == F("fill") && num_args == 1 ) {
      // function tft.fill(color)   fill the screen with the color (565)
      tft->fillScreen(args[0]);
      return PARSER_TRUE;
    }
    else if ( fname == F("cls")  ) {
      // function tft.fill(color)   fill the screen with the color (565)
      tft->fillScreen(0);
	  form1.clear();
      return PARSER_TRUE;
    }
    else if ( fname == F("rgb") && num_args == 3 ) {
      // function tft.color(red, green, blue)    returns the color in 565 format from rgb 888
      *value = tft->color565(args[0], args[1], args[2]);
      return PARSER_TRUE;
    }
    else if ( fname == F("line") && num_args == 5 ) {
      // function tft.line(x1, y1, x2, y2, color)
      tft->drawLine(args[0], args[1], args[2], args[3], args[4]);
      return PARSER_TRUE;
    }
    else if ( fname == F("rect") && num_args == 5 ) {
      // function tft.rect(x, y, width, height, color)
      tft->drawRect(args[0], args[1], args[2], args[3], args[4]);
      return PARSER_TRUE;
    }
    else if ( fname == F("rect.fill") && num_args == 5 ) {
      // function tft.rect.fill(x, y, width, height, color)
      tft->fillRect(args[0], args[1], args[2], args[3], args[4]);
      return PARSER_TRUE;
    }
    else if ( fname == F("rect.round") && num_args == 6 ) {
      // function tft.rect.round(x, y, width, height, radius, color)
      tft->drawRoundRect(args[0], args[1], args[2], args[3], args[4], args[5]);
      return PARSER_TRUE;
    }
    else if ( fname == F("rect.round.fill") && num_args == 5 ) {
      // function tft.rect.round.fill(x, y, width, height, radius, color)
      tft->fillRoundRect(args[0], args[1], args[2], args[3], args[4], args[5]);
      return PARSER_TRUE;
    }
    else if ( fname == F("circle") && num_args == 4 ) {
      // function tft.circle(x, y, radius, color)
      tft->drawCircle(args[0], args[1], args[2], args[3]);
      return PARSER_TRUE;
    }
    else if ( fname == F("circle.fill") && num_args == 4 ) {
      // function tft.circle.fill(x, y, radius, color)
      tft->fillCircle(args[0], args[1], args[2], args[3]);
      return PARSER_TRUE;
    }
    else if ( fname == F("text.cursor") && num_args == 2 ) {
      // function tft.text.cursor(x, y)
      tft->setCursor(args[0], args[1]);
      return PARSER_TRUE;
    }
    else if ( fname == F("text.color") && num_args > 0 ) {
      // function tft.text.color(color, {backcolor})   // the back color is optional
      if (num_args == 1)
        tft->setTextColor(args[0]);
      else
        tft->setTextColor(args[0], args[1]);
      return PARSER_TRUE;
    }
    else if ( fname == F("text.size") && num_args == 1 ) {
      // function tft.text.size(size)
      tft->setTextSize(args[0]);
      return PARSER_TRUE;
    }
      else if ( fname == F("text.font") && num_args == 1 ) {
        // function tft.text.size(size) 
         switch ((int) args[0])
         {
            case 0:
              tft->setFont(NULL);
              break;
            case 1:
              tft->setFont(&FreeSerifBold9pt7b);
              break;
            case 2:
              tft->setFont(&FreeSerifBold12pt7b);
              break;
            case 3:
              tft->setFont(&FreeSerifBold18pt7b);
              break;
            case 4:
              tft->setFont(&FreeSerifBold24pt7b);
              break;
         }
         return PARSER_TRUE;
      } 
    else if ( fname == F("print") && num_args == 1 ) {
      // function tft.text.size(size)
      if (args_str[0] != NULL)
        tft->print(*args_str[0]);
      return PARSER_STRING;
    }
    else if ( fname == F("println") && num_args == 1 ) {
      // function tft.text.size(size)
      if (args_str[0] != NULL)
        tft->println(*args_str[0]);
      return PARSER_STRING;
    }
      else if ( fname == F("bmp") && num_args > 0 ) {
        // function tft.bmp(filename, x, y, backColor)   // by default the color is transparent
        uint16_t x = 0, y = 0;
        int backColor = -1;
        if (args_str[0] == NULL)  { SendErrorMsg(F("tft.bmp() : The first argument must be a string!")); return PARSER_FALSE; }
        if (num_args > 2)
        {
          x = (uint16_t) args[1];
          y = (uint16_t) args[2];
        }
        if (num_args > 3)
          backColor = args[3];
        show_bmp(*args_str[0], x, y, backColor);
         return PARSER_TRUE;
      }
      else if ( fname == F("touch.setup") && num_args == 1 ) {
        // function tft.touch.setup(CS_pin)   //set the pin used for the Touch CS
         Touch_CS_pin = args[0];
         pinMode(Touch_CS_pin, OUTPUT);
         return PARSER_TRUE;
      }      
      else if ( fname == F("touch.calibrate") && num_args == 0 ) {
        // function tft.calibrate() 
         //calibrate();
         return PARSER_TRUE;
      }
      else if ( fname == F("touchx") && num_args == 0 ) {
        // function tft.touchx() 
         *value = touchX;
         return PARSER_TRUE;
      }
      else if ( fname == F("touchy") && num_args == 0 ) {
        // function tft.touchy() 
         *value = touchY;
         return PARSER_TRUE;
      }
      else if ( fname == F("checktouch") && num_args == 0 ) {
        // function tft.getitem() 
         *value = form1.checkTouch(touchX, touchY);
         return PARSER_TRUE;
      }       
      else if ( fname == F("obj.button") && num_args >= 5 ) {
        // function tft.button("text", x,y,width,height, scale, forecolor ,backcolor)
        if (args_str[0] == NULL)  { SendErrorMsg(F("tft.obj.button() : The first argument must be a string!")); return PARSER_FALSE; }
        int obj = form1.add(BUTTON);
        form1[obj]->x = args[1];
        form1[obj]->y = args[2];
        form1[obj]->width = args[3];
        form1[obj]->height = args[4];
        if (num_args >= 6)
          form1[obj]->textsize = args[5];
        else
          form1[obj]->textsize = 2;
        if (num_args >= 7)
          form1[obj]->forecolor = args[6];
        else
          form1[obj]->forecolor = tft->color565(255,255,00);
        if (num_args >= 8)
          form1[obj]->backcolor = args[7];
        else
          form1[obj]->backcolor = tft->color565(100,100,100);
        form1[obj]->label = *args_str[0];
        form1.drawObject(obj);
        *value = obj;
        return PARSER_TRUE;
      }  
      else if ( fname == F("obj.label") && num_args >= 5 ) {
        // function tft.label("text", x,y,width,height,scale, forecolor ,backcolor)
        if (args_str[0] == NULL)  { SendErrorMsg(F("tft.obj.label() : The first argument must be a string!")); return PARSER_FALSE; }
        int obj = form1.add(LABEL);
        form1[obj]->x = args[1];
        form1[obj]->y = args[2];
        form1[obj]->width = args[3];
        form1[obj]->height = args[4];
        if (num_args >= 6)
          form1[obj]->textsize = args[5];
        else
          form1[obj]->textsize = 2;
        if (num_args >= 7)
          form1[obj]->forecolor = args[6];
        else
          form1[obj]->forecolor = tft->color565(255,255,00);
        if (num_args >= 8)
          form1[obj]->backcolor = args[7];
        else
          form1[obj]->backcolor = tft->color565(100,100,100);
        
        form1[obj]->label = *args_str[0];
        form1.drawObject(obj);
        *value = obj;
        return PARSER_TRUE;
      }
      else if ( fname == F("obj.checkbox") && num_args >= 4 ) {
        // function tft.checkbox("text", x,y,width,checked, scale, forecolor ,backcolor)
        if (args_str[0] == NULL)  { SendErrorMsg(F("tft.obj.checkbox() : The first argument must be a string!")); return PARSER_FALSE; }
        int obj = form1.add(CHECKBOX);
        form1[obj]->x = args[1];
        form1[obj]->y = args[2];
        form1[obj]->width = args[3];
        if (num_args >= 5)
          form1[obj]->checked = args[4];
        else
          form1[obj]->checked = false;
        if (num_args >= 6)
          form1[obj]->textsize = args[5];
        else
          form1[obj]->textsize = 2;
        if (num_args >= 7)
          form1[obj]->forecolor = args[6];
        else
          form1[obj]->forecolor = tft->color565(255,255,00);
        if (num_args >= 8)
          form1[obj]->backcolor = args[7];
        else
          form1[obj]->backcolor = tft->color565(100,100,100);
        
        form1[obj]->label = *args_str[0];
        form1.drawObject(obj);
        *value = obj;
        return PARSER_TRUE;
      }
      else if ( fname == F("obj.radio") && num_args >= 4 ) {
        // function tft.checkbox("text", x,y,width,checked, scale, forecolor ,backcolor)
        if (args_str[0] == NULL)  { SendErrorMsg(F("tft.obj.radio() : The first argument must be a string!")); return PARSER_FALSE; }
        int obj = form1.add(RADIO);
        form1[obj]->x = args[1];
        form1[obj]->y = args[2];
        form1[obj]->width = args[3];
        if (num_args >= 5)
          form1[obj]->checked = args[4];
        else
          form1[obj]->checked = false;
        if (num_args >= 6)
          form1[obj]->textsize = args[5];
        else
          form1[obj]->textsize = 2;
        if (num_args >= 7)
          form1[obj]->forecolor = args[6];
        else
          form1[obj]->forecolor = tft->color565(255,255,00);
        if (num_args >= 8)
          form1[obj]->backcolor = args[7];
        else
          form1[obj]->backcolor = tft->color565(100,100,100);
        
        form1[obj]->label = *args_str[0];
        form1.drawObject(obj);
        *value = obj;
        return PARSER_TRUE;
      } 
      else if ( fname == F("obj.toggle") && num_args >= 5 ) {
        // function tft.toggle("icon_true", "icon_false" x,y, checked, scale, back_color)
        if (args_str[0] == NULL)  { SendErrorMsg(F("tft.obj.toggle() : The first argument must be a string!")); return PARSER_FALSE; }
        if (args_str[1] == NULL)  { SendErrorMsg(F("tft.obj.toggle() : The second argument must be a string!")); return PARSER_FALSE; }
        int obj = form1.add(TOGGLE);
        form1[obj]->x = args[2];
        form1[obj]->y = args[3];
        form1[obj]->backcolor = args[6];
        form1[obj]->icon1 = *args_str[0];
        form1[obj]->icon2 = *args_str[1];
        if (num_args >= 5)
          form1[obj]->checked = args[4];
        else
          form1[obj]->checked = false;
        if (num_args >= 6)
          form1[obj]->scale = args[5];
        form1.drawObject(obj);
        *value = obj;
        return PARSER_TRUE;
      }  
      else if ( fname == F("obj.bar") && num_args >= 4 ) {
        // function tft.bar("text", x,y,width,height, scale, forecolor ,backcolor)
        if (args_str[0] == NULL)  { SendErrorMsg(F("tft.obj.radio() : The first argument must be a string!")); return PARSER_FALSE; }
        int obj = form1.add(BAR);
        form1[obj]->x = args[1];
        form1[obj]->y = args[2];
        form1[obj]->width = args[3];
        if (num_args >= 5)
          form1[obj]->height = args[4];
        else
          form1[obj]->height = 0;
        if (num_args >= 6)
          form1[obj]->textsize = args[5];
        else
          form1[obj]->textsize = 2;
        if (num_args >= 7)
          form1[obj]->forecolor = args[6];
        else
          form1[obj]->forecolor = tft->color565(255,255,00);
        if (num_args >= 8)
          form1[obj]->backcolor = args[7];
        else
          form1[obj]->backcolor = tft->color565(100,100,100);
        
        form1[obj]->label = *args_str[0];
        form1.drawObject(obj);
        *value = obj;
        return PARSER_TRUE;
      }    
      else if ( fname == F("obj.setlabel") && num_args == 2 ) {
        // function tft.setlabel(object_id, "label")
        if (args_str[1] == NULL)  { SendErrorMsg(F("tft.obj.setLabel() : The second argument must be a string!")); return PARSER_FALSE; }
        form1.setLabel(args[0], *args_str[1]);
        return PARSER_TRUE;
      }
      else if ( fname == F("obj.setvalue") && num_args == 2 ) {
        // function tft.setValue(object_id, value_float)
        form1.setValue(args[0], args[1]);
        return PARSER_TRUE;
      }
      else if ( fname == F("obj.setchecked") && num_args == 2 ) {
        // function tft.setChecked(object_id, checked)  // checked can be 0(false) or any value for true
        form1.setChecked(args[0], args[1]);
        return PARSER_TRUE;
      } 
      else if ( fname == F("obj.getlabel") && num_args == 1 ) {
        // function tft.getLabel(object_id) 
        *value_str = form1.getLabel(args[0]);
        return PARSER_STRING;
      } 
      else if ( fname == F("obj.getvalue") && num_args == 1 ) {
        // function tft.getValue(object_id) 
        *value = form1.getValue(args[0]);
        return PARSER_TRUE;
      }   
      else if ( fname == F("obj.getchecked") && num_args == 1 ) {
        // function tft.getChecked(object_id)  // checked can be 0(false) or any value for true
        *value = form1.getChecked(args[0]);
        return PARSER_TRUE;
      }   
      else if ( fname == F("obj.invert") && num_args == 1 ) {
        // function tft.invertChecked(object_id)  // checked can be 0(false) or any value for true
        *value = form1.invertChecked(args[0]);
        return PARSER_TRUE;
      }

  }
#endif  
  
  
  
  
  else if (fname.startsWith(F("debug.")) )      // block DEBUG functions; this reduces the number of compares
  {
    fname = fname.substring(6); // skip the term debug.
    if ( fname == F("setvar") && num_args >= 2 )
    {
      if (args_str[0] == NULL)  {
        SendErrorMsg(F("debug.setvar() : The first argument must be a string!"));
        return PARSER_FALSE;
      }
      String tmp = "set~^`" + *args_str[0] + "~^`";   // these are special chars the single quote is the reverse one (ascii code 96)
      if (args_str[1] == NULL)
        tmp = tmp + FloatToString(args[1]);
      else
        tmp = tmp + *args_str[1];

      WebSocketSend(  tmp.c_str());
      return PARSER_TRUE;
    }
    else if ( fname == F("getvar") && num_args == 1 )
    {
      if (args_str[0] == NULL)  {
        SendErrorMsg(F("debug.getvar() : The argument must be a string!"));
        return PARSER_FALSE;
      }
      String tmp = "get~^`" + *args_str[0];   // these are special chars the single quote is the reverse one (ascii code 96)
      WebSocketSend(  tmp.c_str());
      WebSockMessage = "";
      for (int i = 0; ((i < 5) && (WebSockMessage == "")); i++) // wait for the answer
      {
        webSocket->loop();
        delay(0);
      }
      *value_str = WebSockMessage;
      return PARSER_STRING;
    }
      else if ( fname == F("object") && num_args > 1 ) 
    {
        if (args_str[0] == NULL)  { SendErrorMsg(F("debug.object() : The first argument must be a string!")); return PARSER_FALSE; }
        String tmp = *args_str[0] + "~^`";   // these are special chars the single quote is the reverse one (ascii code 96)
        for (int i=1; i<num_args; i++)
        {
          if (args_str[i] == NULL)
            tmp = tmp + FloatToString(args[i]);
      else
            tmp = tmp + *args_str[i];
          tmp = tmp + "~^`"; 
        }
        webSocket->sendTXT(0,tmp.c_str());
      return PARSER_TRUE;
    }
    else if ( fname == F("log") && num_args >= 1 )
    {
      String tmp = "log~^`";   // these are special chars the single quote is the reverse one (ascii code 96)
      if (args_str[0] == NULL)
        tmp = tmp + FloatToString(args[0]);
      else
        tmp = tmp + *args_str[0];

      WebSocketSend( tmp.c_str());
      return PARSER_TRUE;
    }
    else if ( fname == F("getevent") && num_args == 0 )
    {
      *value_str = WebSockEventName;
      return PARSER_STRING;
    }
    else if ( fname == F("getchange") && num_args == 0 )
    {
      *value_str = WebSockChangeName;
      return PARSER_STRING;
    }
  }

  else if ( (i = Search_Array(String(name))) != -1) // check if is an array
  {
    if ( num_args == 0) // the array is just defined as a name without arguments such as a$()
    {
      SendErrorMsg(String(F("Array without argument ")) + String(name)  + String(i));
      return PARSER_FALSE;
    }
    else if ( num_args == 1)
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
  }

  // failed to evaluate function, return false
  return PARSER_FALSE;
}


String makeMeAString(String StringToReturnFromThisFunction)
{
  return StringToReturnFromThisFunction;
}


unsigned long HexToLongInt(String h)
{
  // this function replace the strtol as this function is not able to handle hex numbers greather than 7fffffff
  // I'll take char by char converting from hex to char then shifting 4 bits at the time
  int i;
  unsigned long tmp = 0;
  unsigned char c;
  int s = 0;
  h.toUpperCase();
  for (i = h.length() - 1; i >= 0 ; i--)
  {
    // take the char starting from the right
    c = h[i];
    // convert from hex to int
    c = c - '0';
    if (c > 9)
      c = c - 7;
    // add and shift of 4 bits per each char
    tmp += c << s;
    s += 4;
  }
  return tmp;
}



String IRtype(int decode_type)
{
  switch (decode_type)
  {
    case 1:
      return F("NEC");
      break;
    case 2:
      return F("SONY");
      break;
    case 3:
      return F("RC5");
      break;
    case 4:
      return F("RC6");
      break;
    case 5:
      return F("DISH");
      break;
    case 6:
      return F("SHARP");
      break;
    case 7:
      return F("PANASONIC");
      break;
    case 8:
      return F("JVC");
      break;
    case 9:
      return F("SANYO");
      break;
    case 10:
      return F("MITSUBISHI");
      break;
    case 11:
      return F("SAMSUNG");
      break;
    case 12:
      return F("LG");
      break;
    case 13:
      return F("WHYNTER");
      break;
    default:
      return F("UNKNOWN");
      break;
  }
}

void IRdump(void *res) {
  // Dumps out the decode_results structure.
  // Call this after IRrecv::decode()
  decode_results *results = (decode_results *) res;
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print(F("Unknown encoding: "));
  }
  else if (results->decode_type == NEC) {
    Serial.print(F("Decoded NEC: "));

  }
  else if (results->decode_type == SONY) {
    Serial.print(F("Decoded SONY: "));
  }
  else if (results->decode_type == RC5) {
    Serial.print(F("Decoded RC5: "));
  }
  else if (results->decode_type == RC6) {
    Serial.print(F("Decoded RC6: "));
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print(F("Decoded PANASONIC - Address: "));
    Serial.print(results->panasonicAddress, HEX);
    Serial.print(F(" Value: "));
  }
  else if (results->decode_type == LG) {
    Serial.print(F("Decoded LG: "));
  }
  else if (results->decode_type == JVC) {
    Serial.print(F("Decoded JVC: "));
  }
  else if (results->decode_type == AIWA_RC_T501) {
    Serial.print(F("Decoded AIWA RC T501: "));
  }
  else if (results->decode_type == WHYNTER) {
    Serial.print(F("Decoded Whynter: "));
  }
  Serial.print(results->value, HEX);
  Serial.print(F(" ("));
  Serial.print(results->bits, DEC);
  Serial.println(F(" bits)"));
  Serial.print(F("Raw ("));
  Serial.print(count, DEC);
  Serial.print(F("): "));

  for (int i = 1; i < count; i++) {
    if (i & 1) {
      Serial.print(results->rawbuf[i]*USECPERTICK, DEC);
    }
    else {
      Serial.write('-');
      Serial.print((unsigned long) results->rawbuf[i]*USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println();
}



#if defined(BASIC_TFT)
//////////////////////////////////////////////////////////////////////////
/*
typedef struct
{
    short   ident __attribute__((aligned(1), packed));
    long    file_size __attribute__((aligned(1), packed));
    long    reserved __attribute__((aligned(1), packed));
    long    offset __attribute__((aligned(1), packed));
    long    header_size __attribute__((aligned(1), packed));
    long    width __attribute__((aligned(1), packed));
    long    height __attribute__((aligned(1), packed));
    short   planes __attribute__((aligned(1), packed));
    short   bits_per_pixel __attribute__((aligned(1), packed));
    long    compression __attribute__((aligned(1), packed));
    long    image_size __attribute__((aligned(1), packed));
    long    hor_resolution __attribute__((aligned(1), packed));
    long    ver_resolution __attribute__((aligned(1), packed));
    long    palette_colors __attribute__((aligned(1), packed));
    long    important_colors __attribute__((aligned(1), packed));
} BMP_Header;

typedef struct
{
  unsigned char B,G,R;
} BMP_Pixel;

typedef struct
{
  unsigned char B,G,R,A;
} BMP_Pixel32;
*/
void show_bmp(String filename, uint16_t xi, uint16_t yi, int backColor)
{
  int r;
  int x, y, col;
  uint8_t rt;
  BMP_Pixel32  *p32;
  BMP_Pixel *px;
  BMP_Header header;
  
  File fs_bmp = SPIFFS.open(filename, "r");
  if (!fs_bmp) {
      HaltBasic(F("bmp file not found"));
      return;
  }
  r = fs_bmp.readBytes((char*) &header, sizeof(header));
  delay(0);
  if (sizeof(header) < header.offset) // align the buffer if the header is greather that 40bytes
      r = fs_bmp.readBytes((char*) bmp_buff, header.offset - sizeof(header));
//   Serial.println(header.file_size);
//   Serial.println(header.width);
//   Serial.println(header.height);
//   Serial.println(header.bits_per_pixel);
  delay(0);

  rt = tft->getRotation();
  tft->setRotation(rt ^ 0b10);  //reverse top/bottom
  if (rt & 1)
  {
      // landscape
      xi = 320 - xi - header.width;
      yi = 240 - yi - header.height;
  }
  else
  {
      // portrait
      xi = 240 - xi - header.width;
      yi = 320 - yi - header.height;
  }
  tft->setAddrWindow(xi, yi, xi + header.width - 1, yi + header.height - 1);

  if (header.bits_per_pixel == 32)
  {
    for (y=0; y<header.height; y++)
    {
        delay(0);
        r = fs_bmp.readBytes((char*) bmp_buff, header.width * sizeof(BMP_Pixel32));
        digitalWrite(TFT_CS_pin, LOW);   // TFT CS low
        for (x=header.width-1; x>=0; x--)
        {
            p32 = (BMP_Pixel32 *) &bmp_buff[x * sizeof(BMP_Pixel32)];
            col = ((p32->R & 0xf8) << 8) |  (( p32->G & 0xfc) << 3) | ((p32->B & 0xf8) >> 3) ;
  
            if (p32->A > 40) // means not transparent
            {
                if (backColor != -1)
                  SPI.write16(col, true);
                else
                  tft->drawPixel(x + xi, y + yi, col);
            }
            else
            {
              if (backColor != -1)
                SPI.write16(backColor, true);  // gets a background color pixel
              //else
                   //tft->drawPixel(x + xi, y + yi, 0); // background
            }
        }
        digitalWrite(TFT_CS_pin, HIGH);   // TFT CS High
    }
  }
  else
  if (header.bits_per_pixel == 16)
  {
      for (y=0; y<header.height; y++)
      {
          delay(0);
          r = fs_bmp.readBytes((char*) bmp_buff, header.width * 2);
          digitalWrite(TFT_CS_pin, LOW);   // TFT CS low
          //for (x=0; x<header.width; x++)
          for (x=header.width-1; x>=0; x--)
          {
              uint16_t *cc = (uint16_t *) &bmp_buff[x * 2];
              SPI.write16(*cc, true);
          }
          digitalWrite(TFT_CS_pin, HIGH);   // TFT CS High
      }
  }   
  else
  {
      for (y=0; y<header.height; y++) // should be 24
      {
          delay(0);
          r = fs_bmp.readBytes((char*) bmp_buff, header.width * sizeof(BMP_Pixel));
          digitalWrite(TFT_CS_pin, LOW);   // TFT CS low
          for (x=header.width-1; x>=0; x--)
          {
              px = (BMP_Pixel *) &bmp_buff[x * sizeof(BMP_Pixel)];
              col = ((px->R & 0xf8) << 8) |  (( px->G & 0xfc) << 3) | ((px->B & 0xf8) >> 3) ;
              //tft->drawPixel(x + xi, y + yi, col);
               SPI.write16(col, true);
          }
          digitalWrite(TFT_CS_pin, HIGH);   // TFT CS High
      }
  } 
  tft->setRotation(rt); 
  fs_bmp.close(); 
}

#define AVERAGE 10
int ReadTouchXY(char change_only, int *raw)
{
    unsigned char p[30];
    unsigned char i,j, k;
    short mx, my;
    short mmx[AVERAGE], mmy[AVERAGE];
    int m_x, m_y;
    unsigned char rxd1, rxd2, rxd3, rxd4;
    unsigned short px,py;
    int pos;
    static int pos_p = -1;
    short x,y, xf, yf;
    const unsigned short    // calibration
            calx1 = 180,
            calx2 = 1800,
            caly1 = 180,
            caly2 = 1800;
    SPI.setFrequency(100000);
    digitalWrite(Touch_CS_pin, LOW);   // touch CS low
    SPI.transfer(0x9c);      // Sends the control byte
    //delay(1);
    for (i=0; i<AVERAGE; i++)
    {
        SPI.transfer(0x90);
        rxd3 = SPI.transfer(0);
        rxd4 = SPI.transfer(0);
        mmy[i] = (rxd3<<4)|(rxd4>>4);        
        //delay(1);
        SPI.transfer(0xd0);
        rxd1 = SPI.transfer(0);
        rxd2 = SPI.transfer(0);
        mmx[i] = (rxd1<<4)|(rxd2>>4);        
    }
    digitalWrite(Touch_CS_pin, HIGH);   // touch CS high

    px = (rxd1<<4)|(rxd2>>4);
//    Serial.println("px " + String(px));
    if (px < 0x790)
    {
        for (j=0; j<AVERAGE; j++)
        {
            mx = mmx[j];
            k = 1;
            m_x = mx;
            for (i=0; i<AVERAGE; i++)
            {
                if (abs(mx-mmx[i])<8)
                {
                    k++;
                    m_x += mmx[i];
                }
            }
            if (k>=AVERAGE/2)
            {
                px = m_x/k;
                break;
            }
        }
        if (k<AVERAGE/2)
        {
            return (-1);
        }
    }
    else
       return -1;
    py = (rxd3<<4)|(rxd4>>4);
//    Serial.println("py " + String(py));
    if (py < 0x790)
    {
        for (j=0; j<AVERAGE; j++)
        {
            my = mmy[j];
            k = 1;
            m_y = my;
            for (i=0; i<AVERAGE; i++)
            {
                if (abs(my-mmy[i])<8)
                {
                    k++;
                    m_y += mmy[i];
                }
            }
            if (k>=AVERAGE/2)
            {
                py = m_y / k;
                break;
            }
        }
        if (k<AVERAGE/2)
        {
            return (-1);
        }
    }
    else
       return -1;
       
    if ((px == 0) || (py == 0) || (px > 0x780) || (py > 0x780))
        return -1;
    else
    {
        //printf("raw : x=%d y=%d\n", rxd1, rxd3);
        x = ((px-calx1)*240) /(calx2-calx1);
        y = ((caly2-py)*320) /(caly2-caly1);

        switch (tft->getRotation())
        {
          case 0:
            xf = 240 - x;
            yf = 320 - y;
            break;
          case 1:
            xf = 320 - y;
            yf = x;
            break;
          case 2:
            xf = x;
            yf = y;
            break;
          case 3:
            xf = y;
            yf = 240 - x;
            break;
       }

        *raw = py <<16 | px;

        pos = yf<<16 | xf;

        return pos;
    }

    return (-1);

}

/*
void show_bmp_old(String filename, uint16_t xi, uint16_t yi)
{
  int r;
  int x, y, col;
  BMP_Pixel32  *p32;
  BMP_Pixel *px;
  BMP_Header header;
  char bmp_buff[320 * 4];   // the buffer will contain a full line of 320 pixels
  
  File fs_bmp = SPIFFS.open(filename, "r");
  if (!fs_bmp) {
      //Serial.println("bmp file not found");
      HaltBasic(F("bmp file not found"));
      return;
  }
  r = fs_bmp.readBytes((char*) &header, sizeof(header));
  delay(0);
  if (sizeof(header) < header.offset) // align the buffer if the header is greather that 40bytes
    r = fs_bmp.readBytes((char*) bmp_buff, header.offset - sizeof(header));
//   Serial.println(header.file_size);
//   Serial.println(header.width);
//   Serial.println(header.height);
//   Serial.println(header.bits_per_pixel);
   delay(0);

  if (header.bits_per_pixel == 32)
  {
    for (y=header.height-1; y>=0; y--)
    {
        r = fs_bmp.readBytes((char*) bmp_buff, header.width * sizeof(BMP_Pixel32));
        delay(0);
        for (x=0; x<header.width; x++)
        {
            p32 = (BMP_Pixel32 *) &bmp_buff[x * sizeof(BMP_Pixel32)];
            col = ((p32->R & 0xf8) << 8) |  (( p32->G & 0xfc) << 3) | ((p32->B & 0xf8) >> 3) ;
  
            if (p32->A > 40) // means not transparent
            {
                tft->drawPixel(x + xi, y + yi, col);
            }
            else
            {
                //tft->drawPixel(x + xi, y + yi, 0); // background
            }
        }
    }
  }
  else
  {
      for (y=header.height-1; y>=0; y--)
      {
          r = fs_bmp.readBytes((char*) bmp_buff, header.width * sizeof(BMP_Pixel));
          delay(0);
          for (x=0; x<header.width; x++)
          {
              px = (BMP_Pixel *) &bmp_buff[x * sizeof(BMP_Pixel)];
              col = ((px->R & 0xf8) << 8) |  (( px->G & 0xfc) << 3) | ((px->B & 0xf8) >> 3) ;
              tft->drawPixel(x + xi, y + yi, col);
          }
      }
  }  
  fs_bmp.close(); 
}
static void calibratePoint(uint16_t x, uint16_t y, uint16_t &vi, uint16_t &vj) {
  // Draw cross
  tft->drawFastHLine(x - 8, y, 16, 0xff);
  tft->drawFastVLine(x, y - 8, 16, 0xff);
  
  while (!touch.isTouching()) {
    delay(10);
  }
  touch.getRaw(vi, vj);
  // Erase by overwriting with black
  tft->drawFastHLine(x - 8, y, 16, 0);
  tft->drawFastVLine(x, y - 8, 16, 0);
}

void calibrate() {
  uint16_t x1, y1, x2, y2;
  uint16_t vi1, vj1, vi2, vj2;
  touch.begin(tft->getWidth(), tft->getHeight());
  touch.getCalibrationPoints(x1, y1, x2, y2);
  calibratePoint(x1, y1, vi1, vj1);
  delay(1000);
  calibratePoint(x2, y2, vi2, vj2);
//  touch.setCalibration(vi1, vj1, vi2, vj2);

//  char buf[80];
//  snprintf(buf, sizeof(buf), "%d,%d,%d,%d", (int)vi1, (int)vj1, (int)vi2, (int)vj2);
//  //tft->setFont(ucg_font_helvR14_tr);
//  tft->setTextColor(0xffff);
//  tft->setCursor(0, 25);
//  tft->print("setCalibration params:");
//  tft->setCursor(0, 50);
//  tft->print(buf);
}

*/
#endif