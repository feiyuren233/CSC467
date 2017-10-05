%{
/**********************************************************************
 *
 * 	Eva Bernal 1002252313 bernalev
 *  Ramon Sibello 999753253 sibellov
 *
 *  Interface to the scanner module for CSC467 course project


Note: token seperation: identifiers and keywords must be seperated from 
following identifier, keyword or integer; otherwise no seperation needed
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
int tokenCount = 1;

%}
%option noyywrap

WS          [ \t]
letter 		[A-Za-z]
digit 		[0-9]
sign		[+|-]
id 			{letter}({letter}|{digit})*
integer 	{sign}?({digit}+)
float 		{integer}(\.{digit}+)?(E{integer})?

%%

test {yTRACE(tokenCount++);}

{WS} {/* No action taken */}

"\n" {yyline++;}

{integer} {printf("is int "); yTRACE(tokenCount++);
			/*yylval.num=yytext;*/ return(INT);}

{float} {printf("is float "); yTRACE(tokenCount++);
			/*yylval.fVal=yytext;*/ return(FLOAT);}

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
				returns pointer to symbol table where info stored*/}

int installNum(){/*to specify lexeme -> why is this necessary?*/}

/**/
