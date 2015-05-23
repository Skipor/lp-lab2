/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TINTEND = 258,
     TTNAME = 259,
     TIDENTIFIER = 260,
     TINTEGER = 261,
     TDOUBLE = 262,
     TBOOL = 263,
     TCEQ = 264,
     TCNE = 265,
     TCLT = 266,
     TCLE = 267,
     TCGT = 268,
     TCGE = 269,
     TEQUAL = 270,
     TLPAREN = 271,
     TRPAREN = 272,
     TCOMMA = 273,
     TPLUS = 274,
     TMINUS = 275,
     TMUL = 276,
     TDIV = 277,
     TBLANK = 278,
     TTYPEOF = 279,
     TCASE = 280,
     TARROW = 281,
     TENDL = 282
   };
#endif
/* Tokens.  */
#define TINTEND 258
#define TTNAME 259
#define TIDENTIFIER 260
#define TINTEGER 261
#define TDOUBLE 262
#define TBOOL 263
#define TCEQ 264
#define TCNE 265
#define TCLT 266
#define TCLE 267
#define TCGT 268
#define TCGE 269
#define TEQUAL 270
#define TLPAREN 271
#define TRPAREN 272
#define TCOMMA 273
#define TPLUS 274
#define TMINUS 275
#define TMUL 276
#define TDIV 277
#define TBLANK 278
#define TTYPEOF 279
#define TCASE 280
#define TARROW 281
#define TENDL 282




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 20 "parser.y"
{
	NExpression *expr;
  ExpressionList * exprs;
	NStatement *stmt;
  StatementList * stmts;
	NIdentifier *ident;
  NPrimitiveType	* name_of_type;
	std::string *string;
  NPattern * pattern;
  PatternList * patterns;
  CaseToExpr * case_to_expr;
  CaseToExprList* case_list; 
  NDefinition * def;
  NType * type;
	int intval;
	int token;
}
/* Line 1529 of yacc.c.  */
#line 121 "parser.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

