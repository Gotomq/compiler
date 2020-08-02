%option noyywrap
%{
#include <iostream>
#include <cstdlib>
#include <string>
#include "ast.h"
#include "parser.tab.h"

#define SAVE_TOKEN yylval.str = new std::string(yytext, yyleng)
#define TOKEN(t) (yylval.token = t)
extern FILE* yyin;
int lineno = 1;
%}
all		        [ -~]
CONSTSTRING		\"{all}*\"
%%
"int"                       {return TOKEN(INT);}
"double"                    {return TOKEN(DOUBLE);}
"string"                    {return TOKEN(STRING);}
"return"                    {return TOKEN(RETURN);}
[a-zA-Z_][a-zA-Z0-9_]*      {SAVE_TOKEN; return identifier;}
[0-9]+                      {SAVE_TOKEN; return CINT;}
([0-9]*[.])?[0-9]+          {SAVE_TOKEN; return CDOUBLE;}
{CONSTSTRING}               {SAVE_TOKEN; return CSTRING;}
"**"                        {return TOKEN(o_pow);}
"+"                         {return TOKEN(o_add);}
"-"                         {return TOKEN(o_sub);}
"/"                         {return TOKEN(o_div);}
"*"                         {return TOKEN(o_mult);}
"="                         {return TOKEN(o_assign);}
[,({});]                    {return *yytext;}
[ \t]                       {;}
"\n"                        {lineno++;}
.                           {printf("undefined token :c \n"); yyterminate();}
%%