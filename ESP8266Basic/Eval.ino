#include "expression_parser_string.h"
extern bool  _parser_failed;
extern char* _parser_error_msg;

String FloatToString(float d)
{
  //Convert a double to string with 5 decimals and then removes the trailing zeros (and eventually the '.')
  String z = String(d, 5);
  char *p;
  p = (char*) z.c_str()+ z.length() -1;
  while (*p == '0')
  {
    *p-- = '\0';
  }
  if (*p == '.')
    *p = '\0';
  return String(z.c_str());
} 

String evaluate(String expr)
{
  int status;
  //Serial.print("eval function ");
  //Serial.print(expr);
  //Serial.print("---");
  delay(0);
  status = parse_expression_with_callbacks( expr.c_str(), variable_callback, function_callback, NULL, &double_value, string_value  );
  if (status == PARSER_STRING)
    return string_value;
  else
    return FloatToString(double_value);
  
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
int variable_callback( void *user_data, const char *name, double *value, String *value_str  ){
  // look up the variables by name
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
      if ( (*value == 0) &&  (AllMyVaribles[i][1].length() > 1)) // if the converted value is zero but the string is not "0"
        return PARSER_STRING;
       else        
        return PARSER_TRUE;
      break;
    }
  }
  // failed to find variable, return false
  return PARSER_FALSE;
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
int function_callback( void *user_data, const char *name, const int num_args, const double *args, double *value, String **args_str, String *value_str ){
  int i, max_args;
  double tmp;
  String fname = String(name);
  delay(0);
  // example to show the user-data parameter, sets the maximum number of
  // arguments allowed for the following functions from the user-data function
  max_args = *((int*)user_data);

  if( fname == F("millis") && num_args==0 )  // the function is millis()
  {
    // set return value and return true
    *value = millis();
    return PARSER_TRUE;
  }
  else
  if( fname == F("ramfree") && num_args==0 )  // the function is ramfree()
  {
    // set return value and return true
    *value = ESP.getFreeHeap();
    return PARSER_TRUE;
  }
  else
  if( fname == F("rnd") && num_args==1 )  // the function is rnd(seed)
  {
    // set return value and return true
    randomSeed(millis());
    *value = random(args[0]);
    return PARSER_TRUE;
  }  
  else
  if( fname == F("flashfree") && num_args==0 )  // the function is flashfree()
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
    *value = (double) Flashfree;
    return PARSER_TRUE;
  }
  else 
  if( fname == F("mid") && num_args == 2 ){
    // example of the mid(string, start)
    // set return value 
    *value_str = args_str[0]->substring((int) args[1]-1);
    *value = -1;
    return PARSER_STRING;
  }
  else
  if( fname == F("mid") && num_args == 3 ){
    // example of the mid(string, start, end)
    // set return value 
    *value_str = args_str[0]->substring((int) args[1]-1, (int) (args[1] + args[2]) -1 );
    *value = -1;
    return PARSER_STRING;
  }
  else  
  if( fname == F("right") && num_args == 2 ){
    // example of the right(string, length)
    // set return value 
    *value_str = args_str[0]->substring(args_str[0]->length() - (int) args[1]);
    *value = -1;
    return PARSER_STRING;
  } 
  else
  if( fname == F("left") && num_args == 2 ){
    // example of the left(string, length)
    // set return value 
    *value_str = args_str[0]->substring(0, (int) args[1]);
    *value = -1;
    return PARSER_STRING;
  } 
  else
  if( fname == F("len") && num_args == 1 ){
    // example of the len(string)
    // set return value 
    *value  = args_str[0]->length();
    return PARSER_TRUE;
  }
  else
  if( fname == F("upper") && num_args == 1 ){
    // example of the upper(string)
    // set return value 
    args_str[0]->toUpperCase();
    *value_str  =  *args_str[0];    
    return PARSER_STRING;
  }
  else
  if( fname == F("lower") && num_args == 1 ){
    // example of the lower(string)
    // set return value 
    args_str[0]->toLowerCase();
    *value_str  =  *args_str[0];
    return PARSER_STRING;
  } 
  else
  if( fname == F("instr") && num_args == 2 ){
    // example of the instr(string, string)
    // set return value 
    *value  = args_str[0]->indexOf(*args_str[1]) + 1;
    return PARSER_TRUE;
  } 
  else
  if( fname == F("hex") && num_args == 1 ){
    // example of the hex(value)
    // set return value 
    *value_str  = String((int) args[0], HEX);
    return PARSER_STRING;
  }
  else
  if( fname == F("oct") && num_args == 1 ){
    // example of the oct(value)
    // set return value 
    *value_str  = String((int) args[0], OCT);
    return PARSER_STRING;
  }
  else
    if( fname == F("chr") && num_args == 1 ){
    // example of the chr(value)
    // set return value 
    *value_str  = String((char) args[0]);
    return PARSER_STRING;
  }
  else
    if( fname == F("asc") && num_args == 1 ){
    // example of the asc(string) -> return the ascii code of the 1st char
    // set return value 
    *value = (*args_str[0])[0];
    return PARSER_TRUE;
  }
  else
  if( fname == F("replace") && num_args == 3 ){
    // example of the replace(string, string to search for, string to replacement for}
    // set return value 
    args_str[0]->replace(*args_str[1], *args_str[2]);
    *value_str = *args_str[0];
    return PARSER_STRING;
  }
  else
  if( fname == F("str") && num_args == 1 ){
    // example of str(number) converts the number to string
    // set return value 
    *value_str = FloatToString(args[0]);
    return PARSER_STRING;
  }  
  else
  if( fname == F("wifi.scan") && num_args == 0 ){
    // function wifi.scan() -> no arguments
    // set return value 
    *value = WiFi.scanNetworks() + 1;
    return PARSER_TRUE;
  }
  else
  if( fname == F("wifi.ssid") && num_args == 1 ){
    // function wifi.ssid(number)
    // set return value 
    *value_str =  String(WiFi.SSID(args[0] - 1));
    return PARSER_STRING;
  }  
  else
  if( fname == F("wifi.rssi") && num_args == 1 ){
    // function wifi.rssi(number)
    // set return value 
    *value_str =  String(WiFi.RSSI(args[0] - 1));
    return PARSER_STRING;
  }  
  else
  if( fname == F("ip") && num_args == 0 ){
    // function wifi.scan() -> no arguments
    // set return value 
    *value_str =  String(WiFi.localIP().toString());
    return PARSER_STRING;
  }  
  // failed to evaluate function, return false
  return PARSER_FALSE;
}

