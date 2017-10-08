%{
/**********************************************************************
 *
 * 	Eva Bernal 1002252313 bernalev
 *  Ramon Sibello 999753253 sibellov
 *
 *  Interface to the scanner module for CSC467 course project


Note:
Still to do:
-identify errors & output with yERROR(X)
		-out of bound ints -> [-2147483648,2147483647] => need look ahead when see neg sign?
		-too long ids -> ambiguously 31 chars
-associate info w/ token eg. id name, type, first used -> yyval use?
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"


#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }
/* CASE_TYPE_RETURN_NAME */
#define CTRN(t) case t: return #t;

int yyline = 1;
int tokenCount = 0;

const char* tokenStr(int token_t);
int smartTRACE(int token_t);

%}
%option noyywrap

WS          [ \t]
id 	    [a-zA-Z][a-zA-Z_]*
integer     0|([1-9][0-9]*)
float       {integer}\.{integer}([eE][+\-]?{integer})? 

%%

{WS} {/* No action taken */}

"\n" {yyline++;}

bool {return smartTRACE(BOOL);}
int {return smartTRACE(INT);}
float {return smartTRACE(FLOAT);}

bvec2 {return smartTRACE(BVEC2);}
bvec3 {return smartTRACE(BVEC3);}
bvec4 {return smartTRACE(BVEC4);}
ivec2 {return smartTRACE(IVEC2);}
ivec3 {return smartTRACE(IVEC3);}
ivec4 {return smartTRACE(IVEC4);}
vec2 {return smartTRACE(VEC2);}
vec3 {return smartTRACE(VEC3);}
vec4 {return smartTRACE(VEC4);}

dp3 {return smartTRACE(DP3);}
lit {return smartTRACE(LIT);}
rsq {return smartTRACE(RSQ);}

"(" {return smartTRACE(LP);} /*LeftParentheses*/
")" {return smartTRACE(RP);}
"{" {return smartTRACE(LB);} /*LeftBrace*/
"}" {return smartTRACE(RB);}
"[" {return smartTRACE(LSB);} /*LeftSquareBracket*/
"]" {return smartTRACE(RSB);}

"," {return smartTRACE(COMMA);}
";" {return smartTRACE(SEMI);}

"+" {return smartTRACE(PLUS);}
"-" {return smartTRACE(MINUS);}
"*" {return smartTRACE(MULT);}
"/" {return smartTRACE(DIV);}
"^" {return smartTRACE(EXP);}

"==" {return smartTRACE(EQ);}
"||" {return smartTRACE(OR);}
"&&" {return smartTRACE(AND);}
"!" {return smartTRACE(NOT);}
"!=" {return smartTRACE(NE);}
"<" {return smartTRACE(LT);}
"<=" {return smartTRACE(LE);}
">" {return smartTRACE(GT);}
">=" {return smartTRACE(GE);}

"=" {return smartTRACE(ASSIGN);}

if {return smartTRACE(IF);}
else {return smartTRACE(ELSE);}
while {return smartTRACE(WHILE);}
const {return smartTRACE(CONST);}

true {return smartTRACE(TRUEV);} /*TRUE VALUE*/
false {return smartTRACE(FALSEV);}

{integer} {
    yylval.ival = atoi(yytext);
    return smartTRACE(WHOLEN);
}

{float} {
    yylval.fval = atof(yytext);
    return smartTRACE(REALN);
}

{id} {
    yylval.sval = yytext; 
    return smartTRACE(ID);
}


"/*"        {
              int c1      = 0;
              int c2      = yyinput();
              int curline = yyline;
              for(;;) {
                if (c2 == EOF) {
                  fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: Unmatched /*\n", curline);
                  errorOccurred = TRUE;
                  yyterminate();
                }
                if (c1 == '*' && c2 == '/')
                  break;
                c1 = c2;
                c2 = yyinput();
                if (c1 == '\n' && c2 != EOF)
                  yyline++;
              }
            }

.           { yERROR("Unknown token"); }

%%

const char* tokenStr(int token_t)
{
   /* String literals have const char* type and it's legal to return them */
    switch(token_t) {
        CTRN(BOOL) CTRN(INT) CTRN(FLOAT)
	CTRN(BVEC2) CTRN(BVEC3) CTRN(BVEC4)
	CTRN(IVEC2) CTRN(IVEC3) CTRN(IVEC4)
	CTRN(VEC2) CTRN(VEC3) CTRN(VEC4)
	CTRN(DP3) CTRN(LIT) CTRN(RSQ) CTRN(LP)
	CTRN(RP) CTRN(LB) CTRN(RB) CTRN(LSB) CTRN(RSB) CTRN(COMMA) CTRN(SEMI) 
	CTRN(PLUS) CTRN(MINUS) CTRN(MULT) CTRN(DIV) CTRN(EXP) 
	CTRN(EQ) CTRN(OR) CTRN(AND) CTRN(NOT) CTRN(NE) CTRN(LT) CTRN(LE) CTRN(GT) CTRN(GE) 
	CTRN(ASSIGN)
	CTRN(IF) CTRN(ELSE) CTRN(WHILE) CTRN(CONST)
	CTRN(TRUEV) CTRN(FALSEV) CTRN(WHOLEN) CTRN(REALN) CTRN(ID)

	default: return "UNKNWN";
    }
}

int smartTRACE(int token_t) 
{if (traceScanner) {

    char msg[1024];
    int pos = 0;
    pos += sprintf(&msg[pos], "TOKEN %3d: %-6s '%s'", ++tokenCount, tokenStr(token_t), yytext);
    if (token_t == WHOLEN) {
       pos += sprintf(&msg[pos], " %d", yylval.ival);
    }
    else if (token_t == REALN) {
       pos += sprintf(&msg[pos], " %f", yylval.fval);
    }
    else if (token_t == ID) {
       pos += sprintf(&msg[pos], " %s", yylval.sval);
    }
    fprintf(traceFile, "%s\n", msg);

} return token_t; }
