%{
#include <assert.h>
#include <iostream>
#include <string>
#include <type_traits>

#include <parser_mm.h>
#include <data.h>
#include <ast.h>

// Prevent using <unistd.h> because of bug in flex.
#define YY_NO_UNISTD_H 1
#include "lexer.h"
#include "grammar.hpp"

void yyerror(const std::string &msg);

extern Schema *schema;
ParserMemoryManager *mm;

#define CREATE(type, ...) mm->create<type>(__VA_ARGS__)
#define CREATE_IF_NULL(v, type, ...) if (v == nullptr) v = mm->create<type>(__VA_ARGS__)
#define SET_NULL(v) v = nullptr

//
// variables
//

//Database database;
//Databases databases;

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

%union {
    int intVal;
    char *rawStrVal;
    std::string *strVal;
    struct t
    {
        char _[1024]; // gap
        Type *type;
        Types *types;
        Database *database;
        Databases *databases;
        Property *property;
        Properties *properties;
        Specifier *specifier;
        Specifiers *specifiers;
        Variable *variable;
        Variables *variables;
        ClassSpecifier *classSpecifier;
        ClassSpecifiers *classSpecifiers;
        Class *class_;
        Classes *classes;
    } complexVal;
}

%token EOQ 0 "end of file"
%token ERROR_SYMBOL
%token  L_BRACKET R_BRACKET COMMA QUOTE SEMICOLON COLON POINT
        L_CURLY_BRACKET R_CURLY_BRACKET SHARP R_ARROW
%token VERSION CLASS FIELD TYPES PROPERTIES DATABASE
%token <rawStrVal> STRING
%token <intVal> INTEGER

%type <strVal> string name type key value specifier

%type <complexVal.type> type_decl
%type <complexVal.types> type_decls
%type <complexVal.database> database database_content database_contents
%type <complexVal.databases> databases
%type <complexVal.class_> class class_content class_contents field
%type <complexVal.classes> classes
%type <complexVal.property> key_value_pair class_property field_content
%type <complexVal.properties> class_properties field_contents
%type <complexVal.specifiers> specifiers specifiers1 names

%%

file: file_contents EOQ
    ;

file_contents: /* empty */
    | globals
    | globals classes
    {
        schema->classes = *$2;
        SET_NULL($2);
    }
    ;

globals: global
    | global globals
    ;

global: VERSION COLON INTEGER POINT INTEGER POINT INTEGER
    {
        schema->version.major = $3;
        schema->version.minor = $5;
        schema->version.patch = $7;
    }
    | TYPES L_CURLY_BRACKET type_decls R_CURLY_BRACKET SEMICOLON
    {
        schema->types = *$3;
        SET_NULL($3);
    }
    | databases
    {
        schema->databases = *$1;
        SET_NULL($1);
    }
    ;

databases: database
    {
        CREATE_IF_NULL($$, Databases);
        $$->push_back(*$1);
    }
    | database databases
    {
        $$ = $2;
        $$->push_back(*$1);
    }
    ;
database: DATABASE name L_CURLY_BRACKET database_contents R_CURLY_BRACKET SEMICOLON
    {
        $$ = $4;
        $$->name = *$2;
        SET_NULL($4);
    }
    ;
database_contents: database_content
    | database_content database_contents
    {
        $$ = $2;
    }
    ;
database_content: TYPES L_CURLY_BRACKET type_decls R_CURLY_BRACKET
    {
        CREATE_IF_NULL($$, Database);
        $$->types = *$3;
        SET_NULL($3);
    }
    ;

type_decls: type_decl
    {
        CREATE_IF_NULL($$, Types);
        $$->insert(*$1);
    }
    | type_decl type_decls
    {
        $$ = $2;
        $$->insert(*$1);
    }
    ;
type_decl: key R_ARROW value SEMICOLON
    {
        $$ = CREATE(Type, *$1, *$3);
    }
    ;

classes: class
    {
        CREATE_IF_NULL($$, Classes);
        auto p = $$->insert(*$1);
        assert(p.second);
        SET_NULL($1);
    }
    | class classes
    {
        $$ = $2;
        auto p = $$->insert(*$1);
        assert(p.second);
        SET_NULL($1);
    }
    ;
class: CLASS name L_CURLY_BRACKET class_contents R_CURLY_BRACKET SEMICOLON
    {
        CREATE_IF_NULL($$, Class);
        $$->name = *$2;
        $$->merge(*$4);
    }
    ;
class_contents: class_content
    | class_content class_contents
    {
        $$->merge(*$2);
    }
    ;
class_content: field
    | specifier L_CURLY_BRACKET names R_CURLY_BRACKET
    {
        CREATE_IF_NULL($$, Class);
        auto v = CREATE(ClassSpecifier);
        v->name = *$1;
        v->specifiers = *$3;
        $$->classSpecifiers.push_back(*v);
    }
    ;

field: FIELD L_CURLY_BRACKET field_contents R_CURLY_BRACKET
    {
        CREATE_IF_NULL($$, Class);
        $$->properties = *$3;
    }
    | type name specifiers SEMICOLON
    {
        CREATE_IF_NULL($$, Class);
        auto v = CREATE(Variable);
        v->type = *$1;
        v->name = *$2;
        v->specifiers = *$3;
        $$->variables.push_back(*v);
    }
    | PROPERTIES L_CURLY_BRACKET class_properties R_CURLY_BRACKET
    {
        CREATE_IF_NULL($$, Class);
        $$->properties = *$3;
    }
    ;
field_contents: field_content
    {
        CREATE_IF_NULL($$, Properties);
        $$->push_back(*$1);
    }
    | field_content field_contents
    {
        $$ = $2;
        $$->push_back(*$1);
    }
    ;
field_content: key_value_pair SEMICOLON
    {
        $$ = $1;
    }
    ;

class_properties: class_property
    {
        CREATE_IF_NULL($$, Properties);
        $$->push_back(*$1);
    }
    | class_property class_properties
    {
        $$ = $2;
        $$->push_back(*$1);
    }
    ;
class_property: key SEMICOLON
    {
        $$ = CREATE(Property);
        $$->key = *$1;
    }
    | key_value_pair SEMICOLON
    {
        $$ = $1;
    }
    ;

specifiers: /* empty */
    { 
        CREATE_IF_NULL($$, Specifiers);
    }
    | specifiers1
    ;
specifiers1: specifier
    {
        CREATE_IF_NULL($$, Specifiers);
        $$->insert(*$1);
    }
    | specifier specifiers1
    {
        $$ = $2;
        $$->insert(*$1);
    }
    ;

names: name
    {
        CREATE_IF_NULL($$, Specifiers);
        $$->insert(*$1);
    }
    | name COMMA names
    {
        $$ = $3;
        $$->insert(*$1);
    }
    ;

key_value_pair: key COLON value
    {
        $$ = CREATE(Property);
        $$->key = *$1;
        $$->value = *$3;
    }
    ;

specifier: string
    ;
name: string
    ;
type: string
    ;
key: string
    ;
value: string
    ;
string: STRING
    {
        auto p = CREATE(std::string, $1);
        free($1);
        $$ = p;
    }
    ;

%%

void yyerror(const std::string &msg)
{
    std::cerr << yylloc.first_line << ":" << yylloc.first_column << " " << msg << std::endl;
}
