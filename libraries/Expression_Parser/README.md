expression_parser
=================

A simple C expression parser.  Hand-rolled recursive descent style algorithm implements the parser, removing the need for external tools such as lex/yacc. Reads mathematical expression in infix notation (with a few built-in mathematical functions) and produces double-precision results.  
 
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
