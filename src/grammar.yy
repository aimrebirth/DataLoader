%{
#include <assert.h>
#include <iostream>

#include <data.h>

// Prevent using <unistd.h> because of bug in flex.
#define YY_NO_UNISTD_H 1
#include "lexer.h"
#include "grammar.hpp"

void yyerror(const std::string &msg);

Database db;
Columns columns;
int col_id = 0;
ForeignKeys fks;
%}

%require "2.3"
%debug
%start file
%locations
%verbose
//%no-lines
%error-verbose

%union {
    const char *strVal;
}

%token EOQ 0 "end of file"
%token ERROR_SYMBOL
%token L_BRACKET R_BRACKET COMMA QUOTE SEMICOLON
%token PRIMARY FOREIGN
%token <strVal> STRING

%type <strVal> quoted_string bracket_quoted_string

%%

file: EOQ
    | tables EOQ
    ;

tables: table
    | table tables 
    ;

table:
    STRING STRING quoted_string L_BRACKET vars R_BRACKET SEMICOLON
    {
        Table t;
        t.name = $3;
        t.columns = columns;
        t.fks = fks;
        db.tables[$3] = t;

        columns.clear();
        fks.clear();
        col_id = 0;
    }
    ;

vars: var
    | var COMMA vars
    ;
    
var: quoted_string STRING not_null
    {
        Column column;
        column.id = col_id++;
        column.name = $1;
        if ($2 == string("INTEGER"))
            column.type = ColumnType::Integer;
        else if ($2 == string("REAL"))
            column.type = ColumnType::Real;
        else if ($2 == string("TEXT"))
            column.type = ColumnType::Text;
        else if ($2 == string("BLOB"))
            column.type = ColumnType::Blob;
        else
            assert(false);
        columns[$1] = column;
    }
    | PRIMARY STRING L_BRACKET primary_keys R_BRACKET
    | FOREIGN STRING bracket_quoted_string STRING quoted_string bracket_quoted_string /* FK */
    {
        ForeignKey fk;
        fk.table_name = $5;
        fk.column_name = $6;
        fks[$3] = fk;
    }
    | STRING bracket_quoted_string /* UNIQUE */
    ;

primary_keys: primary_key
    | primary_key COMMA primary_keys
    ;
primary_key: quoted_string
    ;

not_null: /* empty */
    | STRING STRING
    ;

bracket_quoted_string: L_BRACKET quoted_string R_BRACKET
    { $$ = $2; }
    ;

quoted_string: QUOTE STRING QUOTE
    { $$ = $2; }
    ;

%%

void yyerror(const std::string &msg)
{
    std::cerr << yylloc.first_line << ":" << yylloc.first_column << " " << msg << std::endl;
}
