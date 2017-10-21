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
#include "common.h"
//#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

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
%token          FUNC
%token          IF WHILE ELSE
%token          AND OR NEQ EQ LEQ GEQ

// links specific values of tokens to yyval
%token <as_vec>   VEC_T
%token <as_vec>   BVEC_T
%token <as_vec>   IVEC_T
%token <as_float> FLOAT_C
%token <as_int>   INT_C
%token <as_str>   ID


%left 	"||" 
%left	"&&"
%left 	"==""!="'<'"<="'>'">="
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
  :   scope     			 {yTRACE("scope");}
  ;
scope
  :   '{' declarations statements '}'    {yTRACE("{ declarations statements }");}
  ;
declarations:				 {yTRACE("epsilon: declarations");}
  |   declarations declaration		 {yTRACE("declarations declaration");}
  ;
statements:				 {yTRACE("epsilon: statements");}
  |   statements statement 		 {yTRACE("statements statement");}
  ;
declaration:				 {yTRACE("epsilon: declaration");}
  |   type ID ';'                        {yTRACE("type ID;");}
  |   type ID '=' expression ';'         {yTRACE("type ID = expression;");}
  |   CONST type ID '=' expression ';'   {yTRACE("const type ID = expression;");}
  ;
statement
  :   variable '=' expression ';'        {yTRACE("variable = expression;");}
  |   IF '(' expression ')' statement else_statement {yTRACE("if (expression) stmnt else_stmnt");}
  |   WHILE '(' expression ')' statement {yTRACE("while (expression) statement");}
  |   scope
  |   ';'
  ;
else_statement:				 {yTRACE("epsilon: else_statement");}
  |   ELSE statement			 {yTRACE("else statement");}
  ;
type
  :   INT_T | IVEC_T | BOOL_T | BVEC_T | FLOAT_T | VEC_T {yTRACE("type");}
  ;
expression
  :   constructor 			 {yTRACE("constructor");}
  |   function 				 {yTRACE("function");}
  |   INT_C			         {yTRACE("integer literal");}
  |   FLOAT_C				 {yTRACE("floating point literal");}
  |   variable				 {yTRACE("variable");}
  |   unary_op expression	%prec UNARY	 {yTRACE("unvary_op expression");}
  |   expression binary_op expression    {yTRACE("expression binary_op expression");}
  |   TRUE_C | FALSE_C		         {yTRACE("true | false");}
  |   '(' expression ')'                 {yTRACE("(expression)");}
  ;
variable
  :   ID				 {fprintf(traceFile, "(%s) ", $1); yTRACE("ID");}
  |   ID '[' INT_C ']'                   {yTRACE("ID[int_lit]");}
  ;
unary_op
  :   '!' | '-'				 {yTRACE("Unary_op" );}
  ;
binary_op
  :   AND | OR | NEQ | LEQ | GEQ | EQ | '<' | '>' | '+' | '-' | '*' | '/' | '^'
                                         {yTRACE("Binary_op");}
  ;
constructor
  :   type '(' arguments ')'
  ;
function
  :   FUNC '(' arguments_opt ')'         {yTRACE("function_name (arguments_opt)");}
  ;
arguments_opt:				 {yTRACE("epsilon: arguments_opt");}
  |   arguments                          {yTRACE("arguments");}
  ;
arguments
  :   arguments ',' expression           {yTRACE("arguments , expression");}
  |   expression			 {yTRACE("expression");}
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

