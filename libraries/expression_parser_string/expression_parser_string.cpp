#include <math.h>
#include <stdio.h>
#include <String.h>
#include <stdlib.h>

#include <arduino.h>

/**
 @file expression_parser.c
 @author James Gregson (james.gregson@gmail.com)
 @brief implementation of the mathematical expression parser, see expression_parser.h for more information and license terms.
 @
 @author CiccioCB March 2016
 @implementation of string parsing, including literals, variables and functions
 @changes on the functions permitting to recognize the kind of result returned by the expression (number or string)
*/
using namespace std;

extern char* _parser_error_msg;

#include "expression_parser_string.h"
String args_var[PARSER_MAX_ARGUMENT_COUNT];
String tmp_var;
int args_var_pos = 0;
int args_var_level = 0;

int ICACHE_FLASH_ATTR parse_expression( const char *expr, PARSER_PREC *value, String &value_str )
 {
 	return parse_expression_with_callbacks( expr, NULL, NULL, NULL, value, value_str );
 }

int ICACHE_FLASH_ATTR parse_expression_with_callbacks( const char *expr, parser_variable_callback variable_cb, parser_function_callback function_cb, void *user_data, PARSER_PREC *value, String &str_value ){
	int r;
	parser_data pd;
	args_var_level = 0;
	args_var_pos = 0;
	for (int i=0; i<PARSER_MAX_ARGUMENT_COUNT; i++)
		args_var[i] = "";
	parser_data_init( &pd, expr, variable_cb, function_cb, user_data );
	r = parser_parse( &pd, value, str_value );
	if( pd.error ){
		//printf("Error: %s\n", pd.error );
		//printf("Expression '%s' failed to parse, returning nan\n", expr );
	}
	_parser_error_msg = (char *) pd.error;
	return r;	  
}

int ICACHE_FLASH_ATTR parser_data_init( parser_data *pd, const char *str, parser_variable_callback variable_cb, parser_function_callback function_cb, void *user_data ){
	pd->str = str;
	pd->len = strlen( str )+1;
	pd->pos = 0;
	pd->error = NULL;
	pd->user_data   = user_data;
	pd->variable_cb = variable_cb;
	pd->function_cb = function_cb;
	return PARSER_TRUE;
}


int ICACHE_FLASH_ATTR parser_parse( parser_data *pd, PARSER_PREC *value, String &str_value ){
    //PARSER_PREC result = 0.0;
	int r;
	// set the jump position and launch the parser
	//if( !setjmp( pd->err_jmp_buf ) ){
    if (1){
#if !defined(PARSER_EXCLUDE_BOOLEAN_OPS)
		r = parser_read_boolean_or( pd, value, str_value );
#else
		r = parser_read_expr( pd, value, str_value );
#endif
        parser_eat_whitespace( pd );
        if( pd->pos < pd->len-1 )
		{
            parser_error( pd, PSTR("Failed to reach end of input expression, likely malformed input") );
        }
		else
		{
			//*value = result;
			return r;
		}
	}
	else 
	{
		// error was returned, output a nan silently
		*value =  sqrt(-1.0);
		return PARSER_FALSE;
	}
	*value =  sqrt(-1.0);
	return PARSER_FALSE;
}
									   
void ICACHE_FLASH_ATTR parser_error( parser_data *pd, const char *err ){
	pd->error = err;
	//longjmp( pd->err_jmp_buf, 1);
}

char ICACHE_FLASH_ATTR parser_peek( parser_data *pd ){
delay(0);
	if( pd->pos < pd->len )
	{
		return pd->str[pd->pos];
	}
	parser_error( pd, PSTR("Tried to read past end of String!" ));
	return '\0';
}

char ICACHE_FLASH_ATTR parser_peek_n( parser_data *pd, int n ){
delay(0);
	if( pd->pos+n < pd->len )
		return pd->str[pd->pos+n];
	parser_error( pd, PSTR("Tried to read past end of String!" ));
	return '\0';
}

char ICACHE_FLASH_ATTR parser_eat( parser_data *pd ){
delay(0);
	if( pd->pos < pd->len )
		return pd->str[pd->pos++];
	parser_error( pd, PSTR("Tried to read past end of String!" ));
	return '\0';
}

void ICACHE_FLASH_ATTR parser_eat_whitespace( parser_data *pd ){
delay(0);
	while( isspace( parser_peek( pd ) ) )
		parser_eat( pd );
}

int ICACHE_FLASH_ATTR parser_read_Value( parser_data *pd, PARSER_PREC *value, String &str_value ){
	char c;
	char *token; //[PARSER_MAX_TOKEN_SIZE];
	int pos=0;
	//PARSER_PREC val=0.0;
	*value = 0.0;
	token = (char*) malloc(PARSER_MAX_TOKEN_SIZE); // allocate memory 
	
	// read a leading sign
	c = parser_peek( pd );
	if( c == '+' || c == '-' )
		token[pos++] = parser_eat( pd );

	// read optional digits leading the decimal point
	while( isdigit(parser_peek(pd)) )
		token[pos++] = parser_eat( pd );

	// read the optional decimal point
	c = parser_peek( pd );
	if( c == '.' )
		token[pos++] = parser_eat( pd );

	// read optional digits after the decimal point
	while( isdigit(parser_peek(pd)) )
		token[pos++] = parser_eat( pd );

	// read the exponent delimiter
	c = parser_peek( pd );
	if( c == 'e' || c == 'E' ){
		token[pos++] = parser_eat( pd );

		// check if the expoentn has a sign,
		// if so, read it 
		c = parser_peek( pd );
		if( c == '+' || c == '-' ){
			token[pos++] = parser_eat( pd );
		}
	}

	// read the exponent delimiter
	while( isdigit(parser_peek(pd) ) )
		token[pos++] = parser_eat( pd );

	// remove any trailing whitespace
	parser_eat_whitespace( pd );

	// null-terminate the String
	token[pos] = '\0';

  // cicciocb TBD : replace the atof with a more efficient function
  // and insert a more strict control on the double-precision format
  *value = atof(token);
	// check that a double-precision was read, otherwise throw an error
	//if( pos == 0) || sscanf( token, "%lf", &val ) != 1 )
  if( (pos == 0))// || (token[0] != '0' ) )
	{
		//parser_error( pd, "Failed to read real number" );
																						      //printf("Failed to read real number" );
		// now try to analyze if is a string;
		//the string is defined as the text between " or | using | you can put " inside strings (and vice-versa)
		// read a leading sign
		c = parser_peek( pd );
		if( c == '"' || c == '|')
			parser_eat( pd );

		// read optional chars until next " or |
		while( (parser_peek(pd) != c) && (parser_peek(pd) != '\0') ) // stop if the end of String is reached
			token[pos++] = parser_eat( pd );

		// consume the final " or |
		parser_eat( pd );
		token[pos] = '\0';
		// return the parsed value
		str_value = String(token);
		free(token);	// free alllocated memory
		// return the status
		return PARSER_STRING;
	}

	// return the parsed value
	//*value = val;
	free(token);	// free alllocated memory
	// return the status
	return PARSER_TRUE;
}
int ICACHE_FLASH_ATTR parser_read_argument( parser_data *pd, PARSER_PREC *value, String &str_value ){
	char c;
	int r;
	// eat leading whitespace
	parser_eat_whitespace( pd );
	
	// read the argument
	r = parser_read_expr( pd, value, str_value );
	
	// read trailing whitespace
	parser_eat_whitespace( pd );
	
	// check if there's a comma
	c = parser_peek( pd );
	if( c == ',' )
		parser_eat( pd );
	
	// eat trailing whitespace
	parser_eat_whitespace( pd );
	
	// return status
	return r;

}

int ICACHE_FLASH_ATTR parser_read_argument_list( parser_data *pd, int *num_args, PARSER_PREC *args, String **args_str){
	char c;
	int r = PARSER_FALSE; // in case the argument is empty as fun(), the return value will be PARSER_FALSE
	// set the initial number of arguments to zero
	*num_args = 0;
	//Serial.println("parser_read_argument_list");
	// eat any leading whitespace
	parser_eat_whitespace( pd );
	while( parser_peek( pd ) != ')' ){
		// check that we haven't read too many arguments
		if( *num_args >= PARSER_MAX_ARGUMENT_COUNT )
			parser_error( pd, PSTR("Exceeded maximum argument count for function call, increase PARSER_MAX_ARGUMENT_COUNT and recompile!" ));
		
		// read the argument and add it to the list of arguments
		String ss = "";
		//r = parser_read_expr( pd, &args[*num_args], &ss );
		r = parser_read_expr( pd, &args[*num_args], ss );
		// here we try to characterize each element; we could add another array to store the kind of argument available (Double or String)
		// but we can simply put a nan when the arg is a String and a '\0' when the arg is a Number
		if (args_var_level <= 1)
		{
			args_var[args_var_pos++] = tmp_var;
			tmp_var = "";
		}
 		if (r == PARSER_STRING)
		{
			args[*num_args] = sqrt(-1); // nan
			args_str[*num_args] = new String(ss);
		}
		else
			args_str[*num_args] = NULL;	// this means that the arg is not valid!
 
		*num_args = *num_args+1;
		// eat any following whitespace
		parser_eat_whitespace( pd );
	
		// check the next character
		c = parser_peek( pd );
		//Serial.println(c);
		if( c == ')' ){
			// closing parenthesis, end of argument list, return
			// and allow calling function to match the character
			break;
	    } else if( c == ',' ){
			// comma, indicates another argument follows, match
			// the comma, eat any remaining whitespace and continue
			// parsing arguments
			parser_eat( pd );
			parser_eat_whitespace( pd );
		} else {
			// invalid character, print an error and return
			parser_error( pd, PSTR("Expected ')' or ',' in function argument list!" ));
			return PARSER_FALSE;
		}
	}
	return r;
}

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
// This is a C99 compiler - use the built-in round function.
#else
// This is not a C99-compliant compiler - roll our own round function.
// We'll use a name different from round in case this compiler has a non-standard implementation.
int ICACHE_FLASH_ATTR parser_round(PARSER_PREC x){
	int i = (int) x;
	if (x >= 0.0) {
		return ((x-i) >= 0.5) ? (i + 1) : (i);
	} else {
		return (-x+i >= 0.5) ? (i - 1) : (i);
	}
}
#endif


int ICACHE_FLASH_ATTR parser_read_builtin( parser_data *pd, PARSER_PREC *value, String &str_value ){
		
	char c;
	char *token;//[PARSER_MAX_TOKEN_SIZE];
	int num_args, pos=0;
	int r;
	//PARSER_PREC v0=0.0, v1=0.0, args[PARSER_MAX_ARGUMENT_COUNT];
	PARSER_PREC v0 = 0.0, v1 = 1.0;
	PARSER_PREC args[PARSER_MAX_ARGUMENT_COUNT];
	
	//PARSER_PREC *args = (PARSER_PREC*) malloc(PARSER_MAX_ARGUMENT_COUNT*sizeof(PARSER_PREC));
	
	String v1_str=""; // args_str[PARSER_MAX_ARGUMENT_COUNT];
	String *args_str[PARSER_MAX_ARGUMENT_COUNT];
	// put a null for each element; this will permit to recognise the elements created
	for (int i=0; i<PARSER_MAX_ARGUMENT_COUNT; i++)
		args_str[i] = NULL;

	token = (char*) malloc(PARSER_MAX_TOKEN_SIZE); // allocate memory 
	c = parser_peek( pd );

	if( isalpha(c) || c == '_' ){
		// alphabetic character or underscore, indicates that either a function 
		// call or variable follows
		while( isalpha(c) || isdigit(c) || c == '_' || c == '.' || c == '$') {  // the variable/function can contains a point in the name as my.port.id or the $ as A$
			token[pos++] = parser_eat( pd );
			c = parser_peek( pd );
		}
		token[pos] = '\0';
		// check for an opening bracket, which indicates a function call
		if( parser_peek(pd) == '(' ){
			// eat the bracket
			parser_eat(pd);
			// start handling the specific built-in functions
 			if( strcmp_P( token, PSTR("pow") ) == 0 ){
				r = parser_read_argument( pd, &v0, str_value );
				r = parser_read_argument( pd, &v1, str_value );
				v0 = pow( v0, v1 ); 
 			} else if( strcmp_P( token, PSTR("sqr") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				if( v0 < 0.0 ) 
 					parser_error( pd, PSTR("sqrt(x) undefined for x < 0!" ));
 				v0 = sqrt( v0 );
 			} else if( strcmp_P( token, PSTR("log") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				if( v0 <= 0 )
 					parser_error( pd, PSTR("log(x) undefined for x <= 0!" ));
 				v0 = log( v0 );
 			} else if( strcmp_P( token, PSTR("exp") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				v0 = exp( v0 );
 			} else if( strcmp_P( token, PSTR("sin") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );	
				if (r == PARSER_STRING)
					parser_error( pd, PSTR("The argument must be a number!" ));
 				v0 = sin( v0 );
 			} else if( strcmp_P( token, PSTR("asin") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				if( fabs(v0) > 1.0 )
 					parser_error( pd, PSTR("asin(x) undefined for |x| > 1!" ));
 				v0 = asin( v0 );
 			} else if( strcmp_P( token, PSTR("cos") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				v0 = cos( v0 );
 			} else if( strcmp_P( token, PSTR("acos") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				if( fabs(v0 ) > 1.0 )
 					parser_error( pd, PSTR("acos(x) undefined for |x| > 1!" ));
 				v0 = acos( v0 );
 			} else if( strcmp_P( token, PSTR("tan") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );	
 				v0 = tan( v0 );
 			} else if( strcmp_P( token, PSTR("atan") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				v0 = atan( v0 );
 			} else if( strcmp_P( token, PSTR("atan2") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				r = parser_read_argument( pd, &v1, str_value );
 				v0 = atan2( v0, v1 );
 			} else if( strcmp_P( token, PSTR("abs") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				v0 = abs( (int)v0 );
 			} else if( strcmp_P( token, PSTR("fabs") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				v0 = fabs( v0 );
 			} else if( strcmp_P( token, PSTR("floor") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				v0 = floor( v0 );
 			} else if( strcmp_P( token, PSTR("ceil") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 				v0 = floor( v0 );
 			} else if( strcmp_P( token, PSTR("round") ) == 0 ){
 				r = parser_read_argument( pd, &v0, str_value );
 #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
 				// This is a C99 compiler - use the built-in round function.
 				v0 = round( v0 );
 #else
 				// This is not a C99-compliant compiler - use our own round function.
 				v0 = parser_round( v0 );
 #endif
		 } else {
				args_var_level++;
				r = parser_read_argument_list( pd, &num_args, args, args_str);
				delay(0);
				if( pd->function_cb && (r = pd->function_cb( pd->user_data, token, num_args, args, &v1, args_str, &v1_str)) ){
					v0 = v1;
					str_value = v1_str;
				} else {
					parser_error( pd, PSTR("Tried to call unknown built-in function!" ));
				}
				// delete all allocated String arguments
				for (int i=0; i<num_args; i++)
					delete args_str[i];	
				args_var_level--;
				// free the allocated memory
				//free(args);
			}
		
			// eat closing bracket of function call
			if( parser_eat( pd ) != ')' )
				parser_error( pd, PSTR("Expected ')' in built-in call!" ));
		} else {
			// no opening bracket, indicates a variable lookup
			if( pd->variable_cb != NULL && (r = pd->variable_cb( pd->user_data, token, &v1, &v1_str )) ){
					v0 = v1;
					str_value = v1_str;
					if (tmp_var == "")
						tmp_var = token;  // take the name of the variable to be used for variable lookup
			} else {
				parser_error( pd, PSTR("Could not look up value for variable!" ));
			}
		}
	} else {
		// not a built-in function call, just read a literal double or string
		int r = parser_read_Value( pd, value, str_value);
		// consume whitespace
		parser_eat_whitespace( pd );
		
		free(token); // free the allocated memory
		return r;
	}
	
	// consume whitespace
	parser_eat_whitespace( pd );
	
	*value = v0;
	// return the status
	
	free(token); // free the allocated memory
	return r;
}

int ICACHE_FLASH_ATTR parser_read_paren( parser_data *pd, PARSER_PREC *value, String &str_value ){
	PARSER_PREC val;
	int r;

	// check if the expression has a parenthesis
	if( parser_peek( pd ) == '(' ){
		// eat the character
		parser_eat( pd );
		
		// eat remaining whitespace
		parser_eat_whitespace( pd );
		
		// if there is a parenthesis, read it 
		// and then read an expression, then
		// match the closing brace
		r = parser_read_boolean_or( pd, &val, str_value );
		
		// consume remaining whitespace
		parser_eat_whitespace( pd );
		
		// match the closing brace
		if( parser_peek(pd) != ')' )
			parser_error( pd, PSTR("Expected ')'!" ));		
		parser_eat(pd);
	} else {
		// otherwise just read a literal value
		r = parser_read_builtin( pd, value, str_value );
		return r;
	}
	// eat following whitespace
	parser_eat_whitespace( pd );

	*value = val;

	// return the status
	return r;
}

int ICACHE_FLASH_ATTR parser_read_unary( parser_data *pd, PARSER_PREC *value, String &str_value ){
	char c;
	int r;
	PARSER_PREC v0;
	c = parser_peek( pd );
	if ( ( c == '!' ) || (strncmp_P(&pd->str[pd->pos], PSTR("not"), 3) == 0) ){
		// if the first character is a '!', perform a boolean not operation
#if !defined(PARSER_EXCLUDE_BOOLEAN_OPS)
		if (c == 'n')
		{
			// 'not' is found. Remove 2 chars
			pd->pos += 2;
		}
		parser_eat(pd);
		parser_eat_whitespace(pd);
		r = parser_read_paren(pd, &v0, str_value);
//		v0 = fabs(v0) >= PARSER_BOOLEAN_EQUALITY_THRESHOLD ? 0.0 : 1.0;

		// define a more "global" way to work; a binary 'not' is done on the operator
		// this will permit to cover the comparaison and also the binary 'not' operator
		v0 = ~((int) v0);

#else
		parser_error( pd, PSTR("Expected '+' or '-' for unary expression, got '!' or 'not'" ));
#endif
	} else if( c == '-' ){
		// perform unary negation
		parser_eat(pd);
		parser_eat_whitespace(pd);
		r = parser_read_paren(pd, &v0, str_value);
		v0 = -v0;
	} else if( c == '+' ){
		// consume extra '+' sign and continue reading
		parser_eat( pd );
		parser_eat_whitespace(pd);
		r = parser_read_paren(pd, &v0, str_value);
	} else {
		r = parser_read_paren(pd, &v0, str_value);
	}
	parser_eat_whitespace(pd);
	*value = v0;
	// return the status
	return r;
}

int ICACHE_FLASH_ATTR parser_read_power( parser_data *pd, PARSER_PREC *value, String &str_value ){
	PARSER_PREC v0, v1=1.0, s=1.0;
	int r;

	// read the first operand
	r = parser_read_unary( pd, &v0, str_value );
	
	// eat remaining whitespace
	parser_eat_whitespace( pd );
	
	// attempt to read the exponentiation operator
	while( parser_peek(pd) == '^' ){
		parser_eat(pd );
		
		// eat remaining whitespace
		parser_eat_whitespace( pd );
		
		// handles case of a negative immediately 
		// following exponentiation but leading
		// the parenthetical exponent
		if( parser_peek( pd ) == '-' ){
			parser_eat( pd );
			s = -1.0;
			parser_eat_whitespace( pd );
		}
		
		// read the second operand
		//v1 = s*parser_read_power( pd );
		r = parser_read_power( pd, &v1, str_value );
		v1 = v1 * s;
		
		// perform the exponentiation
		v0 = pow( v0, v1 );
		
		// eat remaining whitespace
		parser_eat_whitespace( pd );
	}
	
	// return the result
	*value = v0;
	// return the status
	return r;
}

int ICACHE_FLASH_ATTR parser_read_term( parser_data *pd, PARSER_PREC *value, String &str_value ){
	PARSER_PREC v0, v1;
	char c;
	int r;
	// read the first operand
	//v0 = parser_read_power( pd );
	r = parser_read_power( pd, &v0, str_value );
	
	// eat remaining whitespace
	parser_eat_whitespace( pd );
	
	// check to see if the next character is a
	// multiplication, division, modulo operand (%) or shift left/right (<< or >>)
	c = parser_peek( pd );
	while( c == '*' || c == '/' || c == '%' ||
		  (c == '<' && parser_peek_n(pd,1) == '<') || (c == '>' && parser_peek_n(pd,1) == '>') )
	{
		// eat the character
		parser_eat( pd );
		if ((c == '<') || (c == '>')) // if operator with 2 chars
			parser_eat( pd );   // eat one more char
		// eat remaining whitespace
		parser_eat_whitespace( pd );
		
		// perform the appropriate operation
		if( c == '*' ){
			r = parser_read_power( pd, &v1, str_value );
			v0 *= v1;
		} else if( c == '/' ){
			r = parser_read_power( pd, &v1, str_value );
			v0 /= v1;
		} else if( c == '%' ){
			r = parser_read_power( pd, &v1, str_value );
			v0 = (int)v0 % (int)v1;
		} else if (c == '<'){
			// this should be the << (shift left operator)
			r = parser_read_power( pd, &v1, str_value );
			v0 = (int)v0 << (int)v1;
		} else if (c == '>'){
			// this should be the >> (shift right operator)
			r = parser_read_power( pd, &v1, str_value );
			v0 = (int)v0 >> (int)v1;
		}
		
		// eat remaining whitespace
		parser_eat_whitespace( pd );
		
		// update the character
		c = parser_peek( pd );
	}

	*value = v0;
	// return the status
	return r;
}

int ICACHE_FLASH_ATTR parser_read_expr( parser_data *pd, PARSER_PREC *value, String &str_value ){
	PARSER_PREC v0 = 0.0;
	PARSER_PREC v1;
	char c;
	int r, r0;
	String s0;
	// handle unary minus
	c = parser_peek( pd );
	if( c == '+' || c == '-' ){
		parser_eat( pd );
		parser_eat_whitespace( pd );
		if( c == '+' )
		{
			r = parser_read_term( pd, &v1, str_value );
			v0 +=v1;
		}
		else 
			if( c == '-' )
			{
				r = parser_read_term( pd, &v1, str_value );
				v0 -=v1;
			}
	} 
	else 
	{
		r = parser_read_term( pd, &v0, str_value );
	}
	parser_eat_whitespace( pd );
	
	// check if there is an addition or
	// subtraction operation following
	c = parser_peek( pd );			/* match & but not &&                   */
	while( c == '+' || c == '-'  || (c == '&' && parser_peek_n(pd, 1) != '&') ){ 
		// advance the input
		parser_eat( pd );
		
		// eat any extra whitespace
		parser_eat_whitespace( pd );
		
		// perform the operation
		if( c == '+' )
		{	
			r0 = r;
			s0 = str_value;
			r = parser_read_term( pd, &v1, str_value );
			/*		uncomment this part to permit to use the '+' operator also for the strings	
			// if one of the arguments is string, the result will be a string
			if ((r0 == PARSER_STRING) || (r == PARSER_STRING))
			{
				if (r == PARSER_TRUE)
					str_value = s0 + FloatToString(v1);
				else
					if (r0 == PARSER_TRUE)
						str_value = FloatToString(v0) + str_value;
					else
						str_value =  s0 + str_value;
				r = PARSER_STRING;
			}
			else  // else the result is the numerical sum
			*/
				v0 +=v1;
			r = PARSER_TRUE; // this needs to be removed if the '+' for strings is required
		} 
		else if ( c == '&' )
			{
				r0 = r;
				s0 = str_value;
				r = parser_read_term( pd, &v1, str_value );
				// if one of the arguments is string, the result will be a string
				if ((r0 == PARSER_STRING) || (r == PARSER_STRING))
				{
					if (r == PARSER_TRUE)
						str_value = s0 +  FloatToString(v1);
					else
						if (r0 == PARSER_TRUE)
							str_value =  FloatToString(v0) + str_value;
						else
							str_value =  s0 + str_value;
					r = PARSER_STRING;
				}
				else
				{
					// these are Numeric terms so, we will add as they were strings
					str_value = FloatToString(v0) + FloatToString(v1);
					r = PARSER_STRING;
				}
		} 
		else if( c == '-' )
			{
				r = parser_read_term( pd, &v1, str_value );
				v0 -=v1;
			}	
		
		// eat whitespace
		parser_eat_whitespace( pd );
		
		// update the character being tested in the while loop
		c = parser_peek( pd );
	}
	
	// return expression result
	*value = v0;

	// return the status
	return r;
}



int ICACHE_FLASH_ATTR parser_read_boolean_comparison( parser_data *pd, PARSER_PREC *value, String &str_value ){
	char c, oper[] = { '\0', '\0', '\0' };
	PARSER_PREC v0, v1;
	int r;
	// eat whitespace
	parser_eat_whitespace( pd );
	
	// read the first value
	r = parser_read_expr( pd,  &v0, str_value);
	
	// eat trailing whitespace
	parser_eat_whitespace( pd );
	
	// try to perform boolean comparison operator. Unlike the other operators
	// like the arithmetic operations and the boolean and/or operations, we
	// only allow one operation to be performed. This is done since cascading
	// operations would have unintended results: 2.0 < 3.0 < 1.5 would
	// evaluate to true, since (2.0 < 3.0) == 1.0, which is less than 1.5, even
	// though the 3.0 < 1.5 does not hold.
	c = parser_peek( pd );
	if ( (c == '>' || c == '<') && (strncmp_P(&pd->str[pd->pos], PSTR("<>"), 2) != 0) ){  // only > >= < <= not <>
		// read the operation
		oper[0] = parser_eat( pd );
		c = parser_peek( pd );
		if( c == '=' )
			oper[1] = parser_eat( pd );
		
		// eat trailing whitespace
		parser_eat_whitespace( pd );
		
		// try to read the next term
		int r0 = r;
		String s0 = str_value;

		r = parser_read_expr( pd,  &v1, str_value);
			
		// if both the arguments are numerical
		if ((r0 == PARSER_TRUE) || (r == PARSER_TRUE))
		{
			// perform the boolean operations
			if( strcmp_P( oper, PSTR("<") ) == 0 ){
			v0 = (v0 < v1) ? -1.0 : 0.0;
			} else if( strcmp_P( oper, PSTR(">") ) == 0 ){
			v0 = (v0 > v1) ? -1.0 : 0.0;
			} else if( strcmp_P( oper, PSTR("<=") ) == 0 ){
			v0 = (v0 <= v1) ? -1.0 : 0.0;
			} else if( strcmp_P( oper, PSTR(">=") ) == 0 ){
			v0 = (v0 >= v1) ? -1.0 : 0.0;
			} else {
				parser_error( pd, PSTR("Unknown operation!" ));
			}
		}
		else
			if ((r0 == PARSER_STRING) && (r == PARSER_STRING))
			{
				// perform the boolean operations
				if( strcmp_P( oper, PSTR("<") ) == 0 ){
					v0 = (s0 < str_value) ? -1.0 : 0.0;
				} else if( strcmp_P( oper, PSTR(">") ) == 0 ){
					v0 = (s0 > str_value) ? -1.0 : 0.0;
				} else if( strcmp_P( oper, PSTR("<=") ) == 0 ){
					v0 = (s0 <= str_value) ? -1.0 : 0.0;
				} else if( strcmp_P( oper, PSTR(">=") ) == 0 ){
					v0 = (s0 >= str_value) ? -1.0 : 0.0;
				} else {
					parser_error( pd, PSTR("Unknown operation!" ));
				}	
				r = PARSER_TRUE;
			}
			else
				parser_error( pd, PSTR("Comparaison between string and number!" ));
				
		// read trailing whitespace
		parser_eat_whitespace( pd );
	}
	*value = v0;
	return r;
}

int ICACHE_FLASH_ATTR parser_read_boolean_equality( parser_data *pd, PARSER_PREC *value, String &str_value ){
	char c, oper[] = { '\0', '\0', '\0' };
	PARSER_PREC v0, v1;
	int r;
	// eat whitespace
	parser_eat_whitespace( pd );
	
	// read the first value
	r = parser_read_boolean_comparison( pd, &v0, str_value );
	
	// eat trailing whitespace
	parser_eat_whitespace( pd );
	
	// try to perform boolean equality operator
	c = parser_peek( pd );
	if ( (c == '=' || c == '!' ) || (strncmp_P(&pd->str[pd->pos], PSTR("<>"), 2) == 0) ){
		if( c == '!' ){
			// try to match '!=' without advancing input to not clobber unary not
			if( parser_peek_n( pd, 1 ) == '=' ){
				oper[0] = parser_eat( pd );
				oper[1] = parser_eat( pd );
			} else {
				*value = v0;
				return r;
			}
		} else 
			if( c == '<' ){
				// try to match '<>' without advancing input to not clobber unary not
				if( parser_peek_n( pd, 1 ) == '>' ){
					oper[0] = parser_eat( pd );
					oper[1] = parser_eat( pd );
		} else {
					*value = v0;
					return r;
				}
			} else {		
				// try to match '==' or the "basic language" '='
			oper[0] = parser_eat( pd );
			c = parser_peek( pd );
			if( c != '=' )
			{
				//parser_error( pd, PSTR("Expected a '=' for boolean '==' operator!" ));
				// this is a 'basic language' = operator; we trick it going back to the previous '='
					pd->pos--;
			}
			oper[1] = parser_eat( pd );
		}
		// eat trailing whitespace
		parser_eat_whitespace( pd );
		
		// try to read the next term
		String s0 = str_value;
		int r1 = r;
		r = parser_read_boolean_comparison( pd, &v1, str_value );
		if (r1 != r)
			parser_error( pd, PSTR("Comparaison between string and number!" ));
		// perform the boolean operations
		if( strcmp_P( oper, PSTR("==") ) == 0 )
		{
			if (r == PARSER_STRING)
			{
				v0 = (s0 == str_value) ? -1.0 : 0.0;
				r = PARSER_TRUE;
			}
			else
				v0 = ( fabs(v0 - v1) < PARSER_BOOLEAN_EQUALITY_THRESHOLD ) ? -1.0 : 0.0;
		} 
		else 
			if( (strcmp_P( oper, PSTR("!=") ) == 0 ) || (strcmp_P( oper, PSTR("<>") ) == 0 ) )
			{
				if (r == PARSER_STRING)
				{
					v0 = (s0 != str_value) ? -1.0 : 0.0;
					r = PARSER_TRUE;
				}
				else
					v0 = ( fabs(v0 - v1) > PARSER_BOOLEAN_EQUALITY_THRESHOLD ) ? -1.0 : 0.0;
			} 
			else 
			{
				parser_error( pd, PSTR("Unknown operation!" ));
			}

		// read trailing whitespace
		parser_eat_whitespace( pd );
	}
	*value = v0;
	return r;
}



int ICACHE_FLASH_ATTR parser_read_boolean_and( parser_data *pd, PARSER_PREC *value, String &str_value ){
	char c;
	PARSER_PREC v0, v1;
	int r;
	// tries to read a boolean comparison operator ( <, >, <=, >= ) 
	// as the first operand of the expression
	r = parser_read_boolean_equality( pd, &v0, str_value );
	
	// consume any whitespace befor the operator
	parser_eat_whitespace( pd );
	
	// grab the next character and check if it matches an 'and'
	// operation. If so, match and perform and operations until
	// there are no more to perform
	c = parser_peek( pd );
	while( (c == '&') || (strncmp_P(&pd->str[pd->pos], PSTR("and"), 3) == 0) ){
		if (c == 'a') // means that we found 'and'
		{
			// eat the full 'and'
			pd->pos += 3;
		}
		else
		{
		// eat the first '&'
		parser_eat( pd );
		// check for and eat the second '&'
		c = parser_peek( pd );
		if( c != '&' )
				parser_error( pd, "Expected '&' to follow '&' in logical and operation!" );
			// eat the 2nd '&'
		parser_eat( pd );
		}		
		
		// eat any remaining whitespace
		parser_eat_whitespace( pd );

		// read the second operand of the
		r = parser_read_boolean_equality( pd, &v1, str_value );
		
//		// perform the operation, returning 1.0 for TRUE and 0.0 for FALSE
//		v0 = ( fabs(v0) >= PARSER_BOOLEAN_EQUALITY_THRESHOLD && fabs(v1) >= PARSER_BOOLEAN_EQUALITY_THRESHOLD ) ? 1.0 : 0.0;
		
		// define a more "global" way to work; a binary 'and' is done between the 2 operators
		// this will permit to cover the comparaison and also the binary 'and' operator
		v0 = ((int) v0) & ((int) v1);
	
		// eat any following whitespace
		parser_eat_whitespace( pd );
		
		// grab the next character to continue trying to perform 'and' operations
		c = parser_peek( pd );
	}
	*value = v0;
	return r;
}

int ICACHE_FLASH_ATTR parser_read_boolean_or( parser_data *pd, PARSER_PREC *value, String &str_value ){
	char c;
	PARSER_PREC v0, v1;
	int r;
	// read the first term
	r = parser_read_boolean_and( pd, &v0, str_value );
	
	// eat whitespace
	parser_eat_whitespace( pd );

	// grab the next character and check if it matches an 'or' or 'xor'
	// operation. If so, match and perform and operations until
	// there are no more to perform
	c = parser_peek( pd );

	while( (c == '|') || (strncmp_P(&pd->str[pd->pos], PSTR("or"), 2) == 0) ||
						 (strncmp_P(&pd->str[pd->pos], PSTR("xor"), 3) == 0) )
	{
		if (c == 'o') // means that we found 'or'
		{
			// eat the full 'or'
			pd->pos += 2;
		}
		else
			if (c == 'x') // means that we found 'xor'
			{
				// eat the full 'xor'
				pd->pos += 3;
			}		
			else
			{
				// match the first '|' character
				parser_eat( pd );
				
				// check for and match the second '|' character
				c = parser_peek( pd );
				if( c != '|' )
					parser_error( pd, PSTR("Expected '|' to follow '|' in logical or operation!" ));
				parser_eat( pd );
			}
		
		// eat any following whitespace
		parser_eat_whitespace( pd );
		
		// read the second operand
		r = parser_read_boolean_and( pd, &v1, str_value );
	
//		// perform the 'or' operation
//		v0 = ( fabs(v0) >= PARSER_BOOLEAN_EQUALITY_THRESHOLD || fabs(v1) >= PARSER_BOOLEAN_EQUALITY_THRESHOLD ) ? 1.0 : 0.0;
		
		// define a more "global" way to work; a binary 'or' is done between the 2 operators
		// this will permit to cover the comparaison and also the binary 'or' operator
		if (c != 'x')
			v0 = ((int) v0) | ((int) v1);	//or
		else
			v0 = ((int) v0) ^ ((int) v1);	//xor
		
		// eat any following whitespace
		parser_eat_whitespace( pd );
		
		// grab the next character to continue trying to match
		// 'or' operations
		c = parser_peek( pd );
	}
	
	// return the resulting value
	*value = v0;
	return r;
}

String FloatToString(float d)
{
  //Convert a float to string with 5 decimals and then removes the trailing zeros (and eventually the '.')
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
