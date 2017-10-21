%{
/***********************************************************************
 * 	Eva Bernal 1002252313 bernalev
 *  Ramon Sibello 999753253 sibellov
 * 
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/

#include <string.h>
#include <stdarg.h>
#include "common.h"
//#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */
void fTRACE(char* format, ...);

enum {
  DP3 = 0, 
  LIT = 1, 
  RSQ = 2
};

%}

/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}

// defines the yyval union
%union {
  int as_int;
  int as_vec;
  float as_float;
  char *as_str;
  int as_func;
}

%token          FLOAT_T
%token          INT_T
%token          BOOL_T
%token          CONST
%token          FALSE_C TRUE_C
%token          IF WHILE ELSE
%token          AND OR NEQ EQ LEQ GEQ

// links specific values of tokens to yyval
%token <as_vec>   VEC_T
%token <as_vec>   BVEC_T
%token <as_vec>   IVEC_T
%token <as_float> FLOAT_C
%token <as_int>   INT_C
%token <as_str>   ID
%token <as_func>   FUNC


%left 	OR AND EQ NEQ LEQ GEQ '<' '>'
%left 	'+''-'
%left 	'*''/'
%right 	'^'
%left   UNARY
%left 	'['']''('')'


%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 ***********************************************************************/
program
  :   scope     			 {fTRACE("program-> scope");}
  ;
scope
  :   '{' declarations statements '}'    {yTRACE("scope-> { declarations statements }");}
  ;
declarations:				 {yTRACE("declarations-> epsilon");}
  |   declarations declaration		 {yTRACE("declarations-> declarations declaration");}
  ;
statements:				 {yTRACE("statements-> epsilon");}
  |   statements statement 		 {yTRACE("statements-> statements statement");}
  ;
declaration:				 {yTRACE("declaration-> epsilon");}
  |   type ID ';'                        {fTRACE("declaration-> type ID;  ID: %s", $2);}
  |   type ID '=' expression ';'         {fTRACE("declaration-> type ID = expression;  ID: %s", $2);}
  |   CONST type ID '=' expression ';'   {fTRACE("declaration-> const type ID = expression; ID: %s", $3);}
  ;
statement
  :   variable '=' expression ';'        {yTRACE("statement-> variable = expression;");}
  |   IF '(' expression ')' statement else_statement 
                                         {yTRACE("statement-> if (expression) stmnt else_stmnt");}
  |   WHILE '(' expression ')' statement {yTRACE("statement-> while (expression) statement");}
  |   scope 				 {yTRACE("statement-> scope");}
  |   ';'
  ;
else_statement:				 {yTRACE("else_statement-> epsilon");}
  |   ELSE statement			 {yTRACE("else_statement-> else statement");}
  ;
type
  :   INT_T 				 {yTRACE("type-> INT_T");}
  |   IVEC_T 				 {fTRACE("type-> IVEC_T;  vec size: %d",$1+1);}
  |   BOOL_T 				 {yTRACE("type-> BOOL_T");}
  |   BVEC_T 				 {fTRACE("type-> BVEC_T;  vec size: %d",$1+1);}
  |   FLOAT_T 				 {yTRACE("type-> FLOAT_T");}
  |   VEC_T 				 {fTRACE("type-> VEC_T;  vec size: %d",$1+1);}
  ;
expression
  :   constructor 			 {yTRACE("expression-> constructor");}
  |   function 				 {yTRACE("expression-> function");}
  |   INT_C			         {fTRACE("expression-> INT_C  val: %d", $1);}
  |   FLOAT_C				 {fTRACE("expression-> FLOAT_ val: %f", $1);}
  |   variable				 {yTRACE("expression-> variable");}
  |   unary_op expression    %prec UNARY {yTRACE("expression-> unvary_op expression");}
  |   expression binary_op expression    {yTRACE("expression-> expression binary_op expression");}
  |   TRUE_C 				 {yTRACE("expression-> TRUE_C");}
  |   FALSE_C		                 {yTRACE("expression-> FALSE_C");}
  |   '(' expression ')'                 {yTRACE("expression-> (expression)");}
  ;
variable
  :   ID				 {fTRACE("variable-> ID;  ID: %s", $1);}
  |   ID '[' INT_C ']'                   {fTRACE("variable-> ID[INT_C];  ID: %s, INT_C: %d", $1, $3);}
  ;
unary_op
  :   '!' 				 {yTRACE("unary_op-> !");}
  |   '-'				 {yTRACE("unary_op-> -");}
  ;
binary_op
  :   AND                                {yTRACE("binary_op-> AND &&");} 
  |   OR                                 {yTRACE("binary_op-> OR  ||");} 
  |   NEQ                                {yTRACE("binary_op-> NEQ !=");} 
  |   LEQ                                {yTRACE("binary_op-> LEQ <=");} 
  |   GEQ                                {yTRACE("binary_op-> GEQ >=");} 
  |   EQ                                 {yTRACE("binary_op-> EQ  ==");} 
  |   '<'                                {yTRACE("binary_op-> <");} 
  |   '>'                                {yTRACE("binary_op-> >");} 
  |   '+'                                {yTRACE("binary_op-> +");} 
  |   '-'                                {yTRACE("binary_op-> -");} 
  |   '*'                                {yTRACE("binary_op-> *");} 
  |   '/'                                {yTRACE("binary_op-> /");} 
  |   '^'				 {yTRACE("binary_op-> ^");}
  ;
constructor
  :   type '(' arguments ')' 		 {yTRACE("constructor-> type (arguments)");}
  ;
function
  :   FUNC '(' arguments_opt ')'         {fTRACE("function-> FUNC(arguments_opt); Func #%d",$1);}
  ;
arguments_opt:				 {yTRACE("arguments_opt-> epsilon");}
  |   arguments                          {yTRACE("arguments_opt-> arguments");}
  ;
arguments
  :   arguments ',' expression           {yTRACE("arguments-> arguments , expression");}
  |   expression			 {yTRACE("arguments-> expression");}
  ;


%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(char* s) {
  if(errorOccurred) {
    return;    /* Error has already been reported by scanner */
  } else {
    errorOccurred = 1;
  }

  fprintf(errorFile, "\nPARSER ERROR, LINE %d", yyline);
  
  if(strcmp(s, "parse error")) {
    if(strncmp(s, "parse error, ", 13)) {
      fprintf(errorFile, ": %s\n", s);
    } else {
      fprintf(errorFile, ": %s\n", s+13);
    }
  } else {
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
  }
}

void fTRACE(char* format, ...)
{
    if (traceParser) {
        va_list args;
        va_start(args, format);

        vfprintf(traceFile, format, args);
        
        va_end(args);

        fprintf(traceFile, "\n");
    }
}
