%{
#include <assert.h>
#include <iostream>
#include <string>
#include <type_traits>

#include <parser_mm.h>
#include <ast.h>

using namespace ast;

// Prevent using <unistd.h> because of bug in flex.
#define YY_NO_UNISTD_H 1
#include "lexer.h"
#include "grammar.hpp"

void yyerror(const YYLTYPE *yylloc, const std::string &msg);

extern Schema *schema;
extern ParserMemoryManager *mm;

#define CREATE(type, ...) mm->create<type>(__VA_ARGS__)
#define CREATE_IF_NULL(v, type, ...) if (v == nullptr) v = mm->create<type>(__VA_ARGS__)
#define SET_NULL(v) v = nullptr
#define RESET(v) v = decltype(v)()

//
// variables
//

Types types;
Database database;
Databases databases;
Properties properties;
Specifier specifier;
Specifiers specifiers;
Class class_;
Classes classes;
int variable_id = 0;

//
// variables
//
%}

%require "2.3"
%debug
%start file
%locations
%verbose
//%no-lines
%error-verbose

%define api.pure full
%define api.push-pull push

%union {
    // lexer vars
    int intVal;
    double doubleVal;
    char *rawStrVal;

    // parser vars
    std::string *strVal;
}

%token EOQ 0 "end of file"
%token ERROR_SYMBOL
%token  L_BRACKET R_BRACKET COMMA QUOTE SEMICOLON COLON POINT
        L_CURLY_BRACKET R_CURLY_BRACKET SHARP R_ARROW EQUAL
%token GLOBALS CLASS FIELD TYPES PROPERTIES DATABASE
%token <rawStrVal> STRING
%token <intVal> INTEGER
%token <doubleVal> FLOAT NUMBER

%type <strVal> string name type key value specifier quoted_string any_value

%%

file: file_contents EOQ
    ;

file_contents: /* empty */
    | globals
    | globals classes
    {
        schema->classes = classes;
        RESET(classes);
    }
    ;

globals: global
    | global globals
    ;

global: GLOBALS L_CURLY_BRACKET globals1 R_CURLY_BRACKET SEMICOLON
    | TYPES L_CURLY_BRACKET type_decls R_CURLY_BRACKET SEMICOLON
    {
        schema->types = types;
        RESET(types);
    }
    | databases
    {
        schema->databases = databases;
        RESET(databases);
    }
    ;

globals1: string COLON value
    {
        schema->version = *$3;
    }
    ;

databases: database
    | database databases
    ;
database: DATABASE name L_CURLY_BRACKET database_contents R_CURLY_BRACKET SEMICOLON
    {
        database.name = *$2;
        databases.push_back(database);
        RESET(database);
    }
    ;
database_contents: database_content
    | database_content database_contents
    ;
database_content: TYPES L_CURLY_BRACKET type_decls R_CURLY_BRACKET
    {
        database.types = types;
        RESET(types);
    }
    ;

type_decls: type_decl
    | type_decl type_decls
    ;
type_decl: key R_ARROW value SEMICOLON
    {
        types.insert(Type(*$1, *$3));
    }
    ;

classes: class
    | class classes
    ;
class: CLASS name L_CURLY_BRACKET class_contents R_CURLY_BRACKET SEMICOLON
    {
        class_.name = *$2;
        auto p = classes.insert(class_);
        RESET(class_);
        variable_id = 0;
        assert(p.second);
    }
    ;
class_contents: class_content
    | class_content class_contents
    ;
class_content: field
    | specifier L_CURLY_BRACKET specifiers R_CURLY_BRACKET
    {
        ClassSpecifier v;
        v.name = *$1;
        v.specifiers = specifiers;
        class_.classSpecifiers.push_back(v);
        RESET(specifiers);
    }
    ;

field: FIELD L_CURLY_BRACKET field_contents R_CURLY_BRACKET
    {
        Variable variable;
        variable.properties = properties;
        class_.variables.push_back(variable);
        RESET(properties);
    }
    | type name SEMICOLON
    {
        Variable variable;
        variable.id = variable_id++;
        variable.type = *$1;
        variable.name = *$2;
        class_.variables.push_back(variable);
    }
    | type name EQUAL any_value SEMICOLON
    {
        Variable variable;
        variable.id = variable_id++;
        variable.type = *$1;
        variable.name = *$2;
        variable.defaultValue = *$4;
        class_.variables.push_back(variable);
    }
    | type name L_CURLY_BRACKET properties R_CURLY_BRACKET SEMICOLON
    {
        Variable variable;
        variable.id = variable_id++;
        variable.type = *$1;
        variable.name = *$2;
        variable.properties = properties;
        RESET(properties);
        class_.variables.push_back(variable);
    }
    | PROPERTIES L_CURLY_BRACKET properties R_CURLY_BRACKET
    {
        class_.properties.push_back(properties);
        RESET(properties);
    }
    ;
field_contents: field_content
    | field_content field_contents
    ;
field_content: key_value_pair SEMICOLON
    ;

properties: property
    | property properties
    ;
property: key SEMICOLON
    {
        properties[*$1];
    }
    | key_value_pair SEMICOLON
    ;

specifiers: /* empty */
    | specifiers1
    ;
specifiers1: specifier
    | specifier specifiers1
    ;

key_value_pair: key COLON value
    {
        properties[*$1] = *$3;
    }
    ;

specifier: string
    {
        specifiers.insert(*$1);
    }
    ;
name: string
    ;
type: string
    ;
key: string
    ;
any_value: value
    | INTEGER
    {
        $$ = CREATE(std::string, std::to_string($1));
    }
    | FLOAT
    {
        $$ = CREATE(std::string, std::to_string($1));
    }
    ;
value: string
    | quoted_string
    ;
quoted_string: QUOTE string QUOTE
    { $$ = $2; }
    ;
string: STRING
    {
        auto p = CREATE(std::string, $1);
        free($1);
        $$ = p;
    }
    ;

%%

void yyerror(const YYLTYPE *yylloc, const std::string &msg)
{
    std::cerr << yylloc->first_line << ":" << yylloc->first_column << " " << msg << std::endl;
}
