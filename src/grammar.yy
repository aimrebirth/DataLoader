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
}

%token EOQ 0 "end of file"
%token ERROR_SYMBOL
%token  L_BRACKET R_BRACKET COMMA QUOTE SEMICOLON COLON POINT
        L_CURLY_BRACKET R_CURLY_BRACKET SHARP R_ARROW
%token VERSION CLASS FIELD TYPES PROPERTIES DATABASE
%token <rawStrVal> STRING
%token <intVal> INTEGER

%type <strVal> string name type key value specifier

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

global: VERSION COLON INTEGER POINT INTEGER POINT INTEGER
    {
        schema->version.major = $3;
        schema->version.minor = $5;
        schema->version.patch = $7;
    }
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
    | type name specifiers SEMICOLON
    {
        Variable variable;
        variable.type = *$1;
        variable.name = *$2;
        variable.specifiers = specifiers;
        class_.variables.push_back(variable);
        RESET(specifiers);
    }
    | PROPERTIES L_CURLY_BRACKET class_properties R_CURLY_BRACKET
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

class_properties: class_property
    | class_property class_properties
    ;
class_property: key SEMICOLON
    {
        Property property;
        property.key = *$1;
        properties.push_back(property);
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
        Property property;
        property.key = *$1;
        property.value = *$3;
        properties.push_back(property);
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
