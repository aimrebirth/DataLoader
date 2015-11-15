%{
#pragma warning(disable: 4005)
#include <string>

#include <parser_mm.h>
#include <ast.h>

#include "grammar.hpp"
#define yyterminate() return EOQ 

#define YY_USER_ACTION yylloc.first_column = yylloc.last_column; yylloc.last_column += yyleng;

#define CREATE_STRING yylval.rawStrVal = strdup(yytext)

//#define PUSH_STATE(x) yy_push_state(x)
#define PUSH_STATE(x) BEGIN(x)
//#define POP_STATE yy_pop_state
#define POP_STATE() BEGIN(0)

#define YY_DECL int yylex (yyscan_t yyscanner, YYSTYPE &yylval, YYLTYPE &yylloc)
%}

%option nounistd
%option yylineno
%option nounput
%option batch
%option never-interactive
%option reentrant
%option noyywrap


DIGIT       [0-9]
DIGITS      {DIGIT}{DIGIT}*
INTEGER     [+-]?{DIGITS}
FLOAT       [+-]?{DIGITS}*\.?{DIGITS}+
NUMBER      {FLOAT}

STRING      [[:alpha:]_][[:alnum:]_]*


%x user_string


%%

#.*/\n                  ; // ignore comments

[ \t]+                  ;
\r                      ;
\n                      {
                            yylloc.first_line = ++yylloc.last_line; 
                            yylloc.first_column = yylloc.last_column = 1;
                        }
                        
";"                     return SEMICOLON;
":"                     return COLON;
"("                     return L_BRACKET;
")"                     return R_BRACKET;
"{"                     return L_CURLY_BRACKET;
"}"                     return R_CURLY_BRACKET;
","                     return COMMA;
"\."                    return POINT;
"->"                    return R_ARROW;
"="                     return EQUAL;

globals                 { return GLOBALS; }
types                   { return TYPES; }
class                   { return CLASS; }
database                { return DATABASE; }

field                   return FIELD;
properties              return PROPERTIES;

{NUMBER}                | { yylval.doubleVal = std::stoi(yytext); return FLOAT; }
{STRING}                { CREATE_STRING; return STRING;}

\"                      { PUSH_STATE(user_string);  return QUOTE; }
<user_string>\"         { POP_STATE();              return QUOTE; }
<user_string>(?:[^"\\]|\\.)*/\" {
    CREATE_STRING;
    int n = 0;
    char *p = yytext;
    while ((p = strstr(p, "\n"))++ != 0)
        n++;
    if (n)
    {
        yylloc.last_line += n;
        yylloc.first_line = yylloc.last_line;
    }
    return STRING;
}

.                       { /*ECHO;*/ return ERROR_SYMBOL; }

%%
