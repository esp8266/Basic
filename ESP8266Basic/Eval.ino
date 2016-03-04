#include "expression_parser.h"
extern bool  _parser_failed;
extern char* _parser_error_msg;


double evaluate(String expr)
{
  double value;
  int num_arguments = 10;
//  Serial.print("eval function ");
//  Serial.print(expr);
  delay(0);
  value = parse_expression_with_callbacks( expr.c_str(), variable_callback, function_callback, &num_arguments );
  return value;
}


/**
 @brief user-defined variable callback function. see expression_parser.h for more details.
 @param[in] user_data pointer to any user-defined state data that is required, none in this case
 @param[in] name name of the variable to look up the value of
 @param[out] value output point to double that holds the variable value on completion
 @return true if the variable exists and the output value was set, false otherwise
*/
int variable_callback( void *user_data, const char *name, double *value ){
  // look up the variables by name
  String Name = String(name);
  for (byte i = 0; i < 50; i++)
  {
    if (AllMyVaribles[i][0] == Name)
    {
      delay(0);
      *value =  AllMyVaribles[i][1].toFloat();
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
 @param[in] args list of parsed arguments
 @param[out] value output evaluated result of the function call
 @return true if the function exists and was evaluated successfully with the result stored in value, false otherwise.
*/
int function_callback( void *user_data, const char *name, const int num_args, const double *args, double *value ){
  int i, max_args;
  double tmp;
  delay(0);
  // example to show the user-data parameter, sets the maximum number of
  // arguments allowed for the following functions from the user-data function
  max_args = *((int*)user_data);

  if( strcmp( name, "millis" ) == 0 && num_args==0 )  // the function is millis()
  {
    // set return value and return true
    *value = millis();
    return PARSER_TRUE;
  }
  else
  if( strcmp( name, "ramfree" ) == 0 && num_args==0 )  // the function is ramfree()
  {
    // set return value and return true
    *value = millis();
    return PARSER_TRUE;
  }
  else
  if( strcmp( name, "rnd" ) == 0 && num_args==1 )  // the function is rnd(seed)
  {
    // set return value and return true
    randomSeed(millis());
    *value = random(args[0]);
    return PARSER_TRUE;
  }  
  else
  if( strcmp( name, "flashfree" ) == 0 && num_args==0 )  // the function is flashfree()
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
  // failed to evaluate function, return false
  return PARSER_FALSE;
}

