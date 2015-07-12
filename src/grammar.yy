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
PrimaryKeys pks;
ForeignKeys fks;

std::string sqlColumn;
std::string sqlTable;
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
%token PRIMARY FOREIGN DEFAULT
%token <strVal> STRING

%type <strVal> quoted_string bracket_quoted_string var_end

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
        sqlTable = "create table \"" + string($3) + "\" (\n" + sqlTable.substr(0, sqlTable.size() - 2) + "\n);";

        Table t;
        t.name = $3;
        t.columns = columns;
        t.pks = pks;
        t.fks = fks;
        t.sql = sqlTable;
        db.tables[$3] = t;

        columns.clear();
        pks.clear();
        fks.clear();
        col_id = 0;
        sqlTable.clear();
    }
    ;

vars: var
    | var COMMA vars
    ;
    
var: quoted_string STRING var_end
    {
        sqlColumn = string("\"") + $1 + "\" " + $2 + (sqlColumn.empty() ? "" : " " + sqlColumn) + ",\n";
        sqlTable += sqlColumn;
        sqlColumn.clear();

        Column column;
        column.id = col_id++;
        column.name = $1;
        column.defaultValue = $3;
        if ($2 == string("INTEGER"))
        {
            column.type = ColumnType::Integer;
        }
        else if ($2 == string("REAL"))
        {
            column.type = ColumnType::Real;
        }
        else if ($2 == string("TEXT"))
        {
            column.type = ColumnType::Text;
        }
        else if ($2 == string("BLOB"))
            column.type = ColumnType::Blob;
        else
            assert(false);
        columns[$1] = column;
    }
    | PRIMARY STRING L_BRACKET primary_keys R_BRACKET
    {
        sqlColumn = string("PRIMARY ") + $2 + " (" + sqlColumn.substr(0, sqlColumn.size() - 2) + "),\n";
        sqlTable += sqlColumn;
        sqlColumn.clear();
    }
    | FOREIGN STRING bracket_quoted_string STRING quoted_string bracket_quoted_string /* FK */
    {
        sqlColumn = string("FOREIGN ") + $2 + " (\"" + $3 + "\") " + $4 + " \"" + $5 + "\"" + " (\"" + $6 + "\"),\n";
        sqlTable += sqlColumn;
        sqlColumn.clear();

        ForeignKey fk;
        fk.table_name = $5;
        fk.column_name = $6;
        fks[$3] = fk;
    }
    | STRING bracket_quoted_string /* UNIQUE */
    ;

var_end: not_null
    {
        $$ = "";
    }
    | DEFAULT STRING
    {
        $$ = $2;
        sqlColumn = string("DEFAULT ") + $2;
    }
    ;

primary_keys: primary_key
    | primary_key COMMA primary_keys
    ;
primary_key: quoted_string
    {
        sqlColumn += string("\"") + $1 + "\", ";
        pks.push_back($1);
    }
    ;

not_null: /* empty */
    | STRING STRING
    {
        sqlColumn = string($1) + " " + $2;
    }
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
