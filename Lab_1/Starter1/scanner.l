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

int CheckInt(void);
int yyline = 1;
int tokenCount = 0;

%}
%option noyywrap

WS          [ \t]
letter 		[A-Za-z]
integer 	[0-9]+
id 			{letter}({letter}|{integer}|_)*
float 		{integer}(\.{integer})?(E{integer})?

%%

{WS} {/* No action taken */}

"\n" {yyline++;}

int {yTRACE(++tokenCount); return(INT);}
float {yTRACE(++tokenCount); return(FLOAT);}
bool {yTRACE(++tokenCount); return(BOOL);}
vec2 {yTRACE(++tokenCount); return(VEC2);}
vec3 {yTRACE(++tokenCount); return(VEC3);}
vec4 {yTRACE(++tokenCount); return(VEC4);}
bvec2 {yTRACE(++tokenCount); return(BVEC2);}
bvec3 {yTRACE(++tokenCount); return(BVEC3);}
bvec4 {yTRACE(++tokenCount); return(BVEC4);}
ivec2 {yTRACE(++tokenCount); return(IVEC2);}
ivec3 {yTRACE(++tokenCount); return(IVEC3);}
ivec4 {yTRACE(++tokenCount); return(IVEC4);}
const {yTRACE(++tokenCount); return(CONST);}

dp3 {yTRACE(++tokenCount); return(DP3);}
lit {yTRACE(++tokenCount); return(LIT);}
rsq {yTRACE(++tokenCount); return(RSQ);}

"(" {yTRACE(++tokenCount); return(LP);/*LeftParentheses*/}
")" {yTRACE(++tokenCount); return(RP);}
"{" {yTRACE(++tokenCount); return(LB);/*LeftBrace*/}
"}" {yTRACE(++tokenCount); return(RB);}
"[" {yTRACE(++tokenCount); return(LSB);/*LeftSquareBracket*/}
"]" {yTRACE(++tokenCount); return(RSB);}

"," {yTRACE(++tokenCount); return(COMMA);}
";" {yTRACE(++tokenCount); return(SEMI);}

"+" {yTRACE(++tokenCount); return(PLUS);}
"-" {yTRACE(++tokenCount); return(MINUS);}
"*" {yTRACE(++tokenCount); return(MULT);}
"/" {yTRACE(++tokenCount); return(DIV);}
"^" {yTRACE(++tokenCount); return(EXP);}

"==" {yTRACE(++tokenCount); return(EQ);}
"||" {yTRACE(++tokenCount); return(OR);}
"&&" {yTRACE(++tokenCount); return(AND);}
"!" {yTRACE(++tokenCount); return(NOT);}
"!=" {yTRACE(++tokenCount); return(NE);}
"<" {yTRACE(++tokenCount); return(LT);}
"<=" {yTRACE(++tokenCount); return(LE);}
">" {yTRACE(++tokenCount); return(GT);}
">=" {yTRACE(++tokenCount); return(GE);}

"=" {yTRACE(++tokenCount); return(ASSIGN);}

if {yTRACE(++tokenCount); return(IF);}
else {yTRACE(++tokenCount); return(ELSE);}
while {yTRACE(++tokenCount); return(WHILE);}

true {yTRACE(++tokenCount); return(TRUEV);/*TRUE VALUE*/}
false {yTRACE(++tokenCount); return(FALSEV);}
{integer} {yTRACE(++tokenCount);
	   yylval.iVal = atoi(yytext);
           /*Note: Code suggested by TA yyval.ival=atoi(yytext);*/
           printf("%s%d%s", "yylval.iVal == ", yylval.iVal, "\n");
           return(WHOLEN);}

{float} {yTRACE(++tokenCount); return(REALN);}
{id} {yTRACE(++tokenCount); return(ID);}




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

int installID(){/*to specify lexeme (token's value) 
				including its variable type, value and location of first use?
				returns pointer to symbol table where info stored*/ return 0;}

int installNum(){/*to specify lexeme note: why is this necessary?*/ return 0;}

/**/
