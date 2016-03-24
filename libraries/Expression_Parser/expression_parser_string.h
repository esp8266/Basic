#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

/**
 @mainpage
 @author James Gregson james.gregson@gmail.com <br>&nbsp;<br>
 @brief A simple C expression parser.  Hand-rolled recursive descent style algorithm implements the parser, removing the need for external tools such as lex/yacc. Reads mathematical expression in infix notation (with a few built-in mathematical functions) and produces double-precision results.  
 
 The library handles:
 
 - standard arithmetic operations (+,-,*,/) with operator precedence
 - exponentiation ^ and nested exponentiation
 - unary + and -
 - expressions enclosed in parentheses ('(',')'), optionally nested
 - built-in math functions: pow(x,y), sqrt(x), log(x), exp(x), sin(x), asin(x),
 cos(x), acos(x), tan(x), atan(x), atan2(y,x), abs(x), fabs(x), floor(x),
 ceil(x), round(x), with input arguments checked for domain validity, e.g.
 'sqrt( -1.0 )' returns an error.
 - standard boolean operations (==,!=,>,<,>=,<=,&&,||,!) using the convention
 that False := fabs(value) <= PARSER_BOOLEAN_EQUALITY_THRESHOLD and
 True = !False := fabs(value) > PARSER_BOOLEAN_EQUALITY_THRESHOLD
 - predefined named variables and functions via a callback interface (see below)
 
 Operator precedence and syntax matches the C language as closely as possible to allow straightforward validation of code-correctness.  I.e. the parser should produce the same result as the C language to within rounding errors when only operations from C are used.
 
 Boolean operations may be excluded by defining the preprocessor symbol PARSER_EXCLUDE_BOOLEAN_OPS.
 
 The library is also thread safe, allowing multiple parsers to be operated (on  different inputs) simultaneously.
 
 Error handling is achieved using the setjmp() and longjmp() commands. To the best of my knowledge these are available on nearly all platforms, including embedded, platforms so the library should run happily even on AVRs (this has not been tested).
 
 License: GPLv2 for non-commercial use. Contact me for commercial licensing. This code is provided as-is, with no warranty whatsoever.
 
 Predefined variables and functions are accommodated with a callback interface that allows driver code to look up named variables and evaluate functions as required by the parser.  These callbacks must match the call-signature for the parser_variable_callback and parser_function_callback types below.  The variable callback takes the name of the variable to be looked up and returns true if the named variable value was copied into the output argument, returning false otherwise.  The function callback operates similarly, taking the name of the function to evaluate as well as a list of arguments to that function and (if successful) placing the evaluated function value in the return argument and returning true.  Function calls may be arbitrarily nested.

 @Author CiccioCB
 @Date March 2016
 @Modifications implemented :
	- Introduction of the % (modulo) operator; the arguments are converted to integer
	- Introduction of String arguments literals (string between double quotes), variables and functions
	- Introduction of the Operator '+' for the strings (sum of 2 strings)
	- Introduction of the '==' (equal) and '!=' (not equal) comparators for strings
	TBD : more control on the kind (string or number) for the arguments
		: more operators on strings
		: more comparators on strings
 */

#include<setjmp.h>
#include<stdlib.h>

#include <String>
using namespace std;

/**
 @brief define a threshold for defining true and false for boolean expressions on doubles
*/
#if !defined(PARSER_BOOLEAN_EQUALITY_THRESHOLD)
#define PARSER_BOOLEAN_EQUALITY_THRESHOLD	(1e-10)
#endif

/**
 @brief maximum length for tokens in characters for expressions, define this in the compiler options to change the maximum size
*/
#if !defined(PARSER_MAX_TOKEN_SIZE)
#define PARSER_MAX_TOKEN_SIZE 256
#endif

/**
 @brief maximum number of arguments to user-defined functions, define this in the compiler opetions to change.
*/
#if !defined(PARSER_MAX_ARGUMENT_COUNT)
#define PARSER_MAX_ARGUMENT_COUNT 4
#endif

/**
 @brief definitions for parser true and false or String
*/
#define PARSER_FALSE 	0
#define PARSER_TRUE  	1
#define PARSER_STRING	2

/**
 @brief definitions for math precision (double or float)
*/
#if !defined(PARSER_PREC)
#define PARSER_PREC	float
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
typedef int (*parser_variable_callback)( void *user_data, const char *name, PARSER_PREC *value, String *value_str  );

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
typedef int (*parser_function_callback)( void *user_data, const char *name, const int num_args, const PARSER_PREC *args, PARSER_PREC *value, String **args_str, String *value_str  );

/**
 @brief main data structure for the parser, holds a pointer to the input String and the index of the current position of the parser in the input
*/
typedef struct { 
	
	/** @brief input String to be parsed */
	const char *str; 
	
	/** @brief length of input String */
	int        len;
	
	/** @brief current parser position in the input */
	int        pos;
	
	/** @brief position to return to for exception handling */
	jmp_buf		err_jmp_buf;
	
	/** @brief error String to display, or query on failure */
	const char *error;
	
	/** @brief data pointer that is passed to the variable and function callback. Can be used to stored application state data necessary for performing variable and function lookup. Set to NULL if not used */
	void						*user_data;
	
	/** @brief callback function used to lookup variable values, set to NULL if not used */
	parser_variable_callback	variable_cb;
	
	/** @brief callback function used to perform user-function evaluations, set to NULL if not used */
	parser_function_callback	function_cb;
} parser_data;

/**
 @brief convenience function for using the library, handles initialization and destruction. basically just wraps parser_parse().
 @param[in] expr expression to parse
 @param[out] value output evaluated result of the expression call (double precision number)
 @param[out] value output evaluated result of the expression call (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise
 */
int parse_expression( const char *expr, PARSER_PREC *value, String *str_value );

/**
 @brief convenience function for using the library that exposes the callback interface to the variable and function features.  Initializes a parser_data structure on the stack (i.e. no malloc() or free()), sets the appropriate fields and then calls the internal library functions.
 @param[in] expr expression to parse
 @param[in] variable_cb the user-defined variables callback function. set to NULL if unused. see the parser_data structure and the header documentation for this file for more information.
 @param[in] function_cb the user-defined functions callback function. set to NULL if unused. see the parser_data structure and the header documentation of this file for more information.
 @param[in] user_data void pointer that is passed unaltered to the variable_cb and function_cb pointers, for storing application state needed to look up variables and to evaluate functions. set to NULL if unused
 @param[out] value output evaluated result of the expression call (double precision number)
 @param[out] value output evaluated result of the expression call (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
*/
int parse_expression_with_callbacks( const char *expr, parser_variable_callback variable_cb, parser_function_callback function_cb, void *user_data, PARSER_PREC *value, String &str_value );

/**
 @brief primary public routine for the library
 @param[in] expr expression to parse
 @param[out] value expression parsed (double precision number)
 @param[out] value value expression parsed (string)
 @return PARSER_TRUE if the expression has a double-precision result or
         PARSER_STRING if the expression has a string result stored or
		 PARSER_FALSE otherwise.
 */
int parser_parse( parser_data *pd, PARSER_PREC *value, String &str_value );

/**
 @brief initializes a pre-existing parser_data struture. Use this function to avoid any dynamic memory allocation by the code by passing a pointer to a parser_data structure that has been initialized on the stack.
 @param[inout] pd input and output parser data structure to initialize
 @param[in] str input String to parse
 @param[in] variable_cb variable callback function pointer, set to NULL if not used
 @param[in] function_cb function callback function pointer, set to NULL if not used
 @param[in] user_data pointer to arbitrary user-specified data needed by either the variable or function callback. The same pointer is passed to both functions.  Set to NULL if not needed.
 @return true if initialization was successful, false otherwise
 */
int parser_data_init( parser_data *pd, const char *str, parser_variable_callback variable_cb, parser_function_callback function_cb, void *user_data );

/**
 @brief frees a previously allocated parser_data structure
 @param[in] pd input parser_data structure to free
 */
void parser_data_free( parser_data *pd );

/**
 @brief error function for the parser, simply bails on the code
 @param[in] error String to print
 */
void parser_error( parser_data *pd, const char *err );

/**
 @brief looks at a input character, potentially offset from the current character, without consuming any
 @param[in] pd input parser_data structure to operate on
 @param[in] offset optional offset for character, relative to current character
 @return character that is offset characters from the current input
 */
char parser_peek( parser_data *pd );
	
/**
 @brief returns the current character, and advances the input position
 @param[in] pd input parser_data structure to operate on
 @return current character
 */
char parser_eat( parser_data *pd );
	
/**
 @brief voraciously consumes whitespace input until a non-whitespace character is reached
 @param[in] pd input parser_data structure to operate on
 */
void parser_eat_whitespace( parser_data *pd );

/**  NEW FUNCTION BY CICCIOCB 
 @brief reads and converts a double precision floating point value in one of the many forms,
 e.g. +1.0, -1.0, -1, +1, -1., 1., 0.5, .5, .5e10, .5e-2 .
 It's able now to read and convert also string arguments. A string can be a function,
 a variable or a literal (text between "double quotes").
 @param[in] pd input parser_data structure to operate on
 @param[out] value output evaluated result of the expression call (double precision number)
 @param[out] value output evaluated result of the expression call (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
 */
int parser_read_Value( parser_data *pd, PARSER_PREC *value, String &str_value );

/**
 @brief reads arguments for the builtin functions, auxilliary function for parser_read_builtin()
 @param[in] pd input parser_data structure to operate upon
 @param[out] value output value of the argument that was read (double precision number)
 @param[out] value output value of the argument that was read (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
 */
int parser_read_argument( parser_data *pd, PARSER_PREC *value, String &str_value );

int parser_read_argument_list( parser_data *pd, int *num_args, PARSER_PREC *args, String **args_str);

/**
 @brief reads and calls built-in functions, like sqrt(.), pow(.), etc.
 @param[in] pd input parser_data structure to operate upon
 @param[out] value output value of the resulting value (double precision number)
 @param[out] value output value of the resulting value (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
*/
int parser_read_builtin( parser_data *pd, PARSER_PREC *value, String &str_value );

/**
 @brief attempts to read an expression in parentheses, or failing that a literal value
 @param[in] pd input parser_data structure to operate upon
 @param[out] expression/literal value (double precision number)
 @param[out] expression/literal value (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
 */
int parser_read_paren( parser_data *pd, PARSER_PREC *value, String &str_value );

/**
 @brief attempts to read a unary operation, or failing that, a parenthetical or literal value
 @param[in] pd input parser_data structure to operate upon
 @param[out] expression/literal value (double precision number)
 @param[out] expression/literal value (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
*/
int parser_read_unary( parser_data *pd, PARSER_PREC *value, String &str_value );

/**
 @brief attempts to read an exponentiation operator, or failing that, a parenthetical expression 
 @param[in] pd input parser_data structure to operate upon
 @param[out] exponentiation value (double precision number)
 @param[out] exponentiation value (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
 */
int parser_read_power( parser_data *pd, PARSER_PREC *value, String &str_value );
	
/**
 @brief reads a term in an expression
 @param[in] pd input parser_data structure to operate on
 @param[out] return value of the term (double precision number)
 @param[out] return value of the term (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
 */
int parser_read_term( parser_data *pd, PARSER_PREC *value, String &str_value );

/**
 @brief attempts to read an expression
 @param[in] pd input parser_data structure
 @param[out] return expression value (double precision number)
 @param[out] return expression value (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
 */
int parser_read_expr( parser_data *pd, PARSER_PREC *value, String &str_value );

/**
 @brief reads and performs a boolean comparison operations (<,>,<=,>=,==) if found
 @param[in] pd input parser_data structure
 @param[out] return sub-expression value (double precision number)
 @param[out] return sub-expression value (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
 */
int parser_read_boolean_comparison( parser_data *pd, PARSER_PREC *value, String &str_value );

/**
 @brief reads and performs a boolean 'and' operation (if found)
 @param[in] pd input parser_data structure
 @param[out] return sub-expression value (double precision number)
 @param[out] return sub-expression value (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
*/
int parser_read_boolean_and( parser_data *pd, PARSER_PREC *value, String &str_value );
	
/**
 @brief reads and performs a boolean or operation (if found)
 @param[in] pd input parser_data structure
 @param[out] return expression value (double precision number)
 @param[out] return expression value (string)
 @return PARSER_TRUE if the function exists and was evaluated successfully with the double-prevision result stored in value or
		 PARSER_STRING if the function exists and was evaluated successfully with the string result stored in value_str or
		 PARSER_FALSE otherwise.
*/
int parser_read_boolean_or( parser_data *pd, PARSER_PREC *value, String &str_value );


/**
 @brief Convert a float to string with 5 decimals and then removes the trailing zeros (and eventually the '.')
 @param[in] value to be converted (double precision number)
 @param[out] return expression value (string)
 @return converted value (string)
*/
String FloatToString(PARSER_PREC d);
#endif
