%{
#include <string>
#include "node.h"
#include "parser.hpp"
#define SAVE_STRING yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
%}

%option noyywrap
%option debug

%%
<<EOF>> { static int once = 0; return once++ ? 0 : TENDL; }

"//"[^\n]*              ;
^[ \t]*[\n]             ;
^[ \t]+                  yylval.intval = yyleng; return TINTEND;
[\n]                    return TOKEN(TENDL);
[ \t]                   ;
"true"|"false"          SAVE_STRING; return TBOOL;
[a-z][a-zA-Z0-9_]*      SAVE_STRING; return TIDENTIFIER;
[A-Z][a-zA-Z0-9_]*      SAVE_STRING; return TTNAME;
[0-9]+\.[0-9]*          SAVE_STRING; return TDOUBLE;
[0-9]+                  SAVE_STRING; return TINTEGER;
"_"                     return TOKEN(TBLANK);
"::"                    return TOKEN(TTYPEOF);
"|"                     return TOKEN(TCASE);
"->"                    return TOKEN(TARROW);
"=="                    return TOKEN(TCEQ);
"!="                    return TOKEN(TCNE);
"<"                     return TOKEN(TCLT);
"<="                    return TOKEN(TCLE);
">"                     return TOKEN(TCGT);
">="                    return TOKEN(TCGE);
"="                     return TOKEN(TEQUAL);
"("                     return TOKEN(TLPAREN);
")"                     return TOKEN(TRPAREN);
","                     return TOKEN(TCOMMA);
"+"                     return TOKEN(TPLUS);
"-"                     return TOKEN(TMINUS);
"*"                     return TOKEN(TMUL);
"/"                     return TOKEN(TDIV);

.                       printf("UNKNOWN TOKEN\n"); yyterminate();

%%
