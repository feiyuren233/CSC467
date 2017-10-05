/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INT = 258,
    FLOAT = 259,
    ID = 260,
    WHOLEN = 261,
    REALN = 262,
    BOOL = 263,
    VEC2 = 264,
    VEC3 = 265,
    VEC4 = 266,
    BVEC2 = 267,
    BVEC3 = 268,
    BVEC4 = 269,
    IVEC2 = 270,
    IVEC3 = 271,
    IVEC4 = 272,
    CONST = 273,
    DP3 = 274,
    LIT = 275,
    RSQ = 276,
    LP = 277,
    RP = 278,
    LB = 279,
    RB = 280,
    LSB = 281,
    RSB = 282,
    COMMA = 283,
    SEMI = 284,
    PLUS = 285,
    MINUS = 286,
    MULT = 287,
    DIV = 288,
    EXP = 289,
    EQ = 290,
    OR = 291,
    AND = 292,
    NOT = 293,
    NE = 294,
    LT = 295,
    LE = 296,
    GT = 297,
    GE = 298,
    ASSIGN = 299,
    TRUEV = 300,
    FALSEV = 301,
    IF = 302,
    ELSE = 303,
    WHILE = 304
  };
#endif
/* Tokens.  */
#define INT 258
#define FLOAT 259
#define ID 260
#define WHOLEN 261
#define REALN 262
#define BOOL 263
#define VEC2 264
#define VEC3 265
#define VEC4 266
#define BVEC2 267
#define BVEC3 268
#define BVEC4 269
#define IVEC2 270
#define IVEC3 271
#define IVEC4 272
#define CONST 273
#define DP3 274
#define LIT 275
#define RSQ 276
#define LP 277
#define RP 278
#define LB 279
#define RB 280
#define LSB 281
#define RSB 282
#define COMMA 283
#define SEMI 284
#define PLUS 285
#define MINUS 286
#define MULT 287
#define DIV 288
#define EXP 289
#define EQ 290
#define OR 291
#define AND 292
#define NOT 293
#define NE 294
#define LT 295
#define LE 296
#define GT 297
#define GE 298
#define ASSIGN 299
#define TRUEV 300
#define FALSEV 301
#define IF 302
#define ELSE 303
#define WHILE 304

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 59 "parser.y" /* yacc.c:1909  */

  int num;
  float fVal;

#line 157 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
