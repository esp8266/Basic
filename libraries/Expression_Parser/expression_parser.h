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
 
 Licence: GPLv2 for non-commercial use. Contact me for commercial licensing. This code is provided as-is, with no warranty whatsoever.
 
 Predefined variables and functions are accomodated with a callback interface that allows driver code to look up named variables and evaluate functions as required by the parser.  These callbacks must match the call-signature for the parser_variable_callback and parser_function_callback types below.  The variable callback takes the name of the variable to be looked up and returns true if the named variable value was copied into the output argument, returning false otherwise.  The function callback operates similarly, taking the name of the function to evaluate as well as a list of arguments to that function and (if successful) placing the evaluated function value in the return argument and returning true.  Function calls may be arbitrarily nested.

 Adapted to ESP8266 by CiccioCB (March 2016):
  - Removing of the setjmp() and longjmp() commands not available in that environment
  - Removing reference to sscanf not available also
  - Changes in the constant strings definitions in order to reduce RAM footprint
 */

#include<setjmp.h>
#include<stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

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
#define PARSER_MAX_ARGUMENT_COUNT 10
#endif

/**
 @brief definitions for parser true and false
*/
#define PARSER_FALSE 0
#define PARSER_TRUE  (!PARSER_FALSE)

/**
 @brief definition of the variable callback function type.  
 @param[in] user_data user-specified data pointer that will be passed to the callback, for holding application state
 @param[in] name the name of the variable that is being looked up
 @param[out] value pointer to a double precision value in which to put the variable value
 @return PARSER_TRUE if the variable exists and value was set by the callback, PARSER_FALSE otherwise
*/
typedef int (*parser_variable_callback)( void *user_data, const char *name, double *value );

/**
 @brief definition of the function callback type
 @param[in] user_data user-specified data pointer that will be passed to the callback, for holding application state
 @param[in] name the name of the function to be called
 @param[in] num_args the number of arguments in the function call
 @param[in] args a pointer to a double precision list of arguments for the function call
 @param[out] value the return value of the evaluated function
 @return PARSER_TRUE if the function was evaluated successfully and value was set, PARSER_FALSE otherwise
*/
typedef int (*parser_function_callback)( void *user_data, const char *name, const int num_args, const double *args, double *value );

/**
 @brief main data structure for the parser, holds a pointer to the input string and the index of the current position of the parser in the input
*/
typedef struct { 
	
	/** @brief input string to be parsed */
	const char *str; 
	
	/** @brief length of input string */
	int        len;
	
	/** @brief current parser position in the input */
	int        pos;
	
	/** @brief position to return to for exception handling */
	jmp_buf		err_jmp_buf;
	
	/** @brief error string to display, or query on failure */
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
 @return expression value
 */
double parse_expression( const char *expr );

/**
 @brief convenience function for using the library that exposes the callback interface to the variable and function features.  Initializes a parser_data structure on the stack (i.e. no malloc() or free()), sets the appropriate fields and then calls the internal library functions.
 @param[in] expr expression to parse
 @param[in] variable_cb the user-defined variables callback function. set to NULL if unused. see the parser_data structure and the header documentation for this file for more information.
 @param[in] function_cb the user-defined functions callback function. set to NULL if unused. see the parser_data structure and the header documentation of this file for more information.
 @param[in] user_data void pointer that is passed unaltered to the variable_cb and function_cb pointers, for storing application state needed to look up variables and to evaluate functions. set to NULL if unused
*/
double parse_expression_with_callbacks( const char *expr, parser_variable_callback variable_cb, parser_function_callback function_cb, void *user_data );

/**
 @brief primary public routine for the library
 @param[in] expr expression to parse
 @return expression value
 */
double parser_parse( parser_data *pd );

/**
 @brief initializes a pre-existing parser_data struture. Use this function to avoid any dynamic memory allocation by the code by passing a pointer to a parser_data structure that has been initialized on the stack.
 @param[inout] pd input and output parser data structure to initialize
 @param[in] str input string to parse
 @param[in] variable_cb variable callback function pointer, set to NULL if not used
 @param[in] function_cb function callback function pointer, set to NULL if not used
 @param[in] user_data pointer to arbitrary user-specified data needed by either the variable or function callback. The same pointer is passed to both functions.  Set to NULL if not needed.
 @return true if initialization was successful, false otherwise
 */
int parser_data_init( parser_data *pd, const char *str, parser_variable_callback variable_cb, parser_function_callback function_cb, void *user_data );

/**
 @brief allocates a new parser_data structure and initializes the member variables
 @param[in] str input string to be parsed
 @param[in] variable_cb variable-lookup callback function pointer, set to NULL if unused
 @param[in] function_cb function-evaluation callback function pointer, set to NULL if unused
 @param[in] user_data user-specified data pointer to be used by the variable_cb and/or function_cb callbacks.  Set to NULL if unused.
 @return parser_data structure if successful, or NULL on failure
 */
parser_data *parser_data_new( const char *str, parser_variable_callback variable_cb, parser_function_callback function_cb, void *user_data );

/**
 @brief frees a previously allocated parser_data structure
 @param[in] pd input parser_data structure to free
 */
void parser_data_free( parser_data *pd );

/**
 @brief error function for the parser, simply bails on the code
 @param[in] error string to print
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

/**
 @brief reads and converts a double precision floating point value in one of the many forms,
 e.g. +1.0, -1.0, -1, +1, -1., 1., 0.5, .5, .5e10, .5e-2
 @param[in] pd input parser_data structure to operate on
 @return parsed value as double precision floating point number
 */
double parser_read_double( parser_data *pd );

/**
 @brief reads arguments for the builtin functions, auxilliary function for 
 parser_read_builtin()
 @param[in] pd input parser_data structure to operate upon
 @return value of the argument that was read
 */
double parser_read_argument( parser_data *pd ); 

/**
 @brief reads and calls built-in functions, like sqrt(.), pow(.), etc.
 @param[in] pd input parser_data structure to operate upon
 @return resulting value
*/
double parser_read_builtin( parser_data *pd );

/**
 @brief attempts to read an expression in parentheses, or failing that a literal value
 @param[in] pd input parser_data structure to operate upon
 @return expression/literal value
 */
double parser_read_paren( parser_data *pd );

/**
 @brief attempts to read a unary operation, or failing that, a parenthetical or literal value
 @param[in] pd input parser_data structure to operate upon
 @return expression/literal value
*/
double parser_read_unary( parser_data *pd );

/**
 @brief attempts to read an exponentiation operator, or failing that, a parenthetical expression 
 @param[in] pd input parser_data structure to operate upon
 @return exponentiation value
 */
double parser_read_power( parser_data *pd );
	
/**
 @brief reads a term in an expression
 @param[in] pd input parser_data structure to operate on
 @return value of the term
 */
double parser_read_term( parser_data *pd );

/**
 @brief attempts to read an expression
 @param[in] pd input parser_data structure
 @return expression value
 */
double parser_read_expr( parser_data *pd );

/**
 @brief reads and performs a boolean comparison operations (<,>,<=,>=,==) if found
 @param[in] pd input parser_data structure
 @return sub-expression value
 */
double parser_read_boolean_comparison( parser_data *pd );

/**
 @brief reads and performs a boolean 'and' operation (if found)
 @param[in] pd input parser_data structure
 @return sub-expression value
*/
double parser_read_boolean_and( parser_data *pd );
	
/**
 @brief reads and performs a boolean or operation (if found)
 @param[in] pd input parser_data structure
 @return expression value
*/
double parser_read_boolean_or( parser_data *pd );

#ifdef __cplusplus
};
#endif

#endif