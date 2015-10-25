%{
#include "grammar.hpp"
#define yyterminate() return EOQ 

#define YY_USER_ACTION yylloc.first_column = yylloc.last_column; yylloc.last_column += yyleng;

#define CREATE_STRING yylval.strVal = strdup(yytext)
%}

%option nounistd
%option yylineno
%option nounput
%option batch
%option never-interactive

%x USER_STRING

%%

[ \t]+                  ;
\r                      |
\n                      {
                            yylloc.first_line = ++yylloc.last_line; 
                            yylloc.first_column = yylloc.last_column = 1;
                        }

";"                     return SEMICOLON;
"("                     return L_BRACKET;
")"                     return R_BRACKET;
","                     return COMMA;

DEFAULT                 return DEFAULT;
PRIMARY                 return PRIMARY;
FOREIGN                 return FOREIGN;
UNIQUE                  return UNIQUE;

[A-Za-z_0-9][A-Za-z_0-9]*  { CREATE_STRING; return STRING; }

\"                      { BEGIN(USER_STRING); return QUOTE; }
<USER_STRING>\"         { BEGIN(0);           return QUOTE; }
<USER_STRING>[^"]*/\"   { CREATE_STRING;      return STRING; }

.                       return ERROR_SYMBOL;

%%

int yywrap()
{
    return 1;
}
