%{
/***********************************************************************
 *  Eva Bernal 1002252313 bernalev
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
#include "ast.hpp"
#include "symbol.hpp"
#include "semantic.hpp"

#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(const char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */
void setParserInfo(Node* node);

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
  Node *as_ast;
}

%token          FLOAT_T
%token          INT_T
%token          BOOL_T
%token          CONST
%token          FALSE_C TRUE_C
%token          FUNC
%token          IF ELSE
%token          AND OR NEQ EQ LEQ GEQ

//ANY_T defined here so that it's present in 'parser.h' and can be used as a type
%token          ANY_T 

// links specific values of tokens to yyval
%token <as_vec>   VEC_T
%token <as_vec>   BVEC_T
%token <as_vec>   IVEC_T
%token <as_float> FLOAT_C
%token <as_int>   INT_C
%token <as_str>   ID

// operator precdence
%left     OR                        // 7
%left     AND                       // 6
%left     EQ NEQ '<' LEQ '>' GEQ    // 5
%left     '+' '-'                   // 4
%left     '*' '/'                   // 3
%right    '^'                       // 2
%right    '!' UMINUS                // 1
%left     '(' '['                   // 0

// resolve dangling else shift/reduce conflict with associativity
%left     WITHOUT_ELSE
%left     WITH_ELSE

// type declarations
// TODO: fill this out
%type <as_ast> program
%type <as_ast> scope
%type <as_ast> declarations
%type <as_ast> statements
%type <as_ast> declaration
%type <as_ast> statement
%type <as_ast> type
%type <as_ast> expression
%type <as_ast> variable
%type <as_ast> arguments
%type <as_ast> arguments_opt

%type  <as_int> FUNC

// expect one shift/reduce conflict, where Bison chooses to shift
// the ELSE.
%expect 1

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
  : scope
      { yTRACE("program -> scope\n")
        ast = $1;
        setParserInfo(ast);
      }
  ;

scope
  : '{' declarations statements '}'
      { yTRACE("scope -> { declarations statements }\n")
        $$ = new Scope(static_cast<Declarations*>($2), static_cast<Statements*>($3)); 
        setParserInfo($$); }
  ;

declarations
  : declarations declaration
      { yTRACE("declarations -> declarations declaration\n")
        $$ = new Declarations(static_cast<Declarations*>($1), static_cast<Declaration*>($2)); 
        setParserInfo($$); }
  | 
      { yTRACE("declarations -> \n")
        $$ = new Declarations(); 
        setParserInfo($$); }
  ;

statements
  : statements statement
      { yTRACE("statements -> statements statement\n")
        $$ = new Statements(static_cast<Statements*>($1), static_cast<Statement*>($2)); 
        setParserInfo($$); }
  | 
      { yTRACE("statements -> \n")
        $$ = new Statements(); 
        setParserInfo($$); }
  ;

declaration
  : type ID ';' 
      { yTRACE("declaration -> type ID ;\n")
        $$ = new Declaration(false, static_cast<TypeNode*>($1), std::string($2)); 
        setParserInfo($$); }
  | type ID '=' expression ';'
      { yTRACE("declaration -> type ID = expression ;\n")
        $$ = new Declaration(false, static_cast<TypeNode*>($1), 
                             std::string($2), static_cast<Expression*>($4));  
        setParserInfo($$);}
  | CONST type ID '=' expression ';'
      { yTRACE("declaration -> CONST type ID = expression ;\n")
        $$ = new Declaration(true, static_cast<TypeNode*>($2), 
                             std::string($3), static_cast<Expression*>($5)); 
        setParserInfo($$); }
  ;

statement
  : variable '=' expression ';'
      { yTRACE("statement -> variable = expression ;\n") 
        $$ = new Statement(static_cast<Variable*>($1), static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | IF '(' expression ')' statement ELSE statement %prec WITH_ELSE
      { yTRACE("statement -> IF ( expression ) statement ELSE statement \n") 
        $$ = new Statement(static_cast<Expression*>($3), 
                           static_cast<Statement*>($5),
                           static_cast<Statement*>($7)); 
        setParserInfo($$);}
  | IF '(' expression ')' statement %prec WITHOUT_ELSE
      { yTRACE("statement -> IF ( expression ) statement \n") 
        $$ = new Statement(static_cast<Expression*>($3), 
                           static_cast<Statement*>($5)); 
        setParserInfo($$);}
  | scope 
      { yTRACE("statement -> scope \n") 
        $$ = new Statement(static_cast<Scope*>($1)); 
        setParserInfo($$);}
  | ';'
      { yTRACE("statement -> ; \n") 
        $$ = new Statement(); 
        setParserInfo($$);}
  ;

type
  : INT_T
      { yTRACE("type -> INT_T \n") 
        $$ = new TypeNode(INT_T);  
        setParserInfo($$);}
  | IVEC_T
      { yTRACE("type -> IVEC_T \n") 
	//+1 here is because in the scanner yytext[3] - '1' instead of -'0'
        //We could change the scanner, but it's an easy fix and I'd like to avoid changing the starter
	$$ = new TypeNode(IVEC_T, yylval.as_vec + 1);  
        setParserInfo($$);}
  | BOOL_T
      { yTRACE("type -> BOOL_T \n") 
        $$ = new TypeNode(BOOL_T);  
        setParserInfo($$);}
  | BVEC_T
      { yTRACE("type -> BVEC_T \n") 
        $$ = new TypeNode(BVEC_T, yylval.as_vec + 1);  
        setParserInfo($$);}
  | FLOAT_T
      { yTRACE("type -> FLOAT_T \n") 
        $$ = new TypeNode(FLOAT_T);  
        setParserInfo($$);}
  | VEC_T
      { yTRACE("type -> VEC_T \n") 
        $$ = new TypeNode(VEC_T, yylval.as_vec + 1);  
        setParserInfo($$);}
  ;

expression

  /* function-like operators */
  : type '(' arguments_opt ')' %prec '('
      { yTRACE("expression -> type ( arguments_opt ) \n") 
        $$ = new OtherExpression(static_cast<TypeNode*>($1), static_cast<Arguments*>($3)); 
        setParserInfo($$);}
  | FUNC '(' arguments_opt ')' %prec '('
      { yTRACE("expression -> FUNC ( arguments_opt ) \n") 
        $$ = new OtherExpression($1, static_cast<Arguments*>($3)); 
        setParserInfo($$);}

  /* unary opterators */
  | '-' expression %prec UMINUS
      { yTRACE("expression -> - expression \n") 
        $$ = new OperationExpression(UMINUS, static_cast<Expression*>($2)); 
        setParserInfo($$);}
  | '!' expression %prec '!'
      { yTRACE("expression -> ! expression \n") 
        $$ = new OperationExpression('!', static_cast<Expression*>($2)); 
        setParserInfo($$);}

  /* binary operators */
  | expression AND expression %prec AND
      { yTRACE("expression -> expression AND expression \n") 
        $$ = new OperationExpression(static_cast<Expression*>($1), AND, static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression OR expression %prec OR
      { yTRACE("expression -> expression OR expression \n") 
        $$ = new OperationExpression(static_cast<Expression*>($1), OR, static_cast<Expression*>($3));  
        setParserInfo($$);}
  | expression EQ expression %prec EQ
      { yTRACE("expression -> expression EQ expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), EQ, static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression NEQ expression %prec NEQ
      { yTRACE("expression -> expression NEQ expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), NEQ, static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression '<' expression %prec '<'
      { yTRACE("expression -> expression < expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), '<', static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression LEQ expression %prec LEQ
      { yTRACE("expression -> expression LEQ expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), LEQ, static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression '>' expression %prec '>'
      { yTRACE("expression -> expression > expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), '>', static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression GEQ expression %prec GEQ
      { yTRACE("expression -> expression GEQ expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), GEQ, static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression '+' expression %prec '+'
      { yTRACE("expression -> expression + expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), '+', static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression '-' expression %prec '-'
      { yTRACE("expression -> expression - expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), '-', static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression '*' expression %prec '*'
      { yTRACE("expression -> expression * expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), '*', static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression '/' expression %prec '/'
      { yTRACE("expression -> expression / expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), '/', static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression '^' expression %prec '^'
      { yTRACE("expression -> expression ^ expression \n")  
        $$ = new OperationExpression(static_cast<Expression*>($1), '^', static_cast<Expression*>($3)); 
        setParserInfo($$);}

  /* literals */
  | TRUE_C
      { yTRACE("expression -> TRUE_C \n") 
        $$ = new LiteralExpression(true); 
        setParserInfo($$);}
  | FALSE_C
      { yTRACE("expression -> FALSE_C \n") 
        $$ = new LiteralExpression(false); 
        setParserInfo($$);}
  | INT_C
      { yTRACE("expression -> INT_C \n") 
        $$ = new LiteralExpression(yylval.as_int); 
        setParserInfo($$);}
  | FLOAT_C
      { yTRACE("expression -> FLOAT_C \n") 
        $$ = new LiteralExpression(yylval.as_float); 
        setParserInfo($$);}

  /* misc */
  | '(' expression ')'
      { yTRACE("expression -> ( expression ) \n") 
        $$ = new OtherExpression(static_cast<Expression*>($2)); 
        setParserInfo($$);}
  | variable
    { yTRACE("expression -> variable \n") 
      $$ = new OtherExpression(static_cast<Variable*>($1)); 
        setParserInfo($$);}
  ;

variable
  : ID
      { yTRACE("variable -> ID \n") 
        $$ = new Variable(std::string($1)); 
        setParserInfo($$);}
  | ID '[' INT_C ']' %prec '['
      { yTRACE("variable -> ID [ INT_C ] \n") 
        $$ = new Variable(std::string($1), yylval.as_int); 
        setParserInfo($$);}
  ;

arguments
  : arguments ',' expression
      { yTRACE("arguments -> arguments , expression \n") 
        $$ = new Arguments(static_cast<Arguments*>($1), static_cast<Expression*>($3)); 
        setParserInfo($$);}
  | expression
      { yTRACE("arguments -> expression \n") 
        $$ = new Arguments(nullptr, static_cast<Expression*>($1)); 
        setParserInfo($$);}
  ;

arguments_opt
  : arguments
      { yTRACE("arguments_opt -> arguments \n") 
        $$ = new Arguments(static_cast<Arguments*>($1)); 
        setParserInfo($$);}
  |
      { yTRACE("arguments_opt -> \n") 
        $$ = new Arguments(); 
        setParserInfo($$);}
  ;

%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
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

void setParserInfo(Node* node)
{
    node->setParserInfo(yyline);
}
