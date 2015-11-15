#include "parser.h"

#include <algorithm>
#include <fstream>

// Prevent using <unistd.h> because of bug in flex.
#include <grammar.hpp>
#define YY_NO_UNISTD_H 1
#define YY_DECL 1
#include <lexer.h>
extern int yylex(yyscan_t yyscanner, YYSTYPE &yylval, YYLTYPE &yylloc);

#include "ast.h"
#include "context.h"
#include "parser_mm.h"

ast::Schema *schema;
ParserMemoryManager *mm;

YYSTYPE convert(Token &t)
{
    YYSTYPE yylval = { 0 };
    switch (t.value.which())
    {
    case Token::Integer:
        yylval.intVal = boost::get<int>(t.value);
        break;
    case Token::Float:
        yylval.doubleVal = boost::get<double>(t.value);
        break;
    case Token::String:
        yylval.rawStrVal = strdup(boost::get<std::string>(t.value).c_str());
        break;
    default:
        break;
    }
    return yylval;
}

Token convert(BisonToken token, YYSTYPE &yylval)
{
    Token t;
    t.token = token;
    switch (token)
    {
    case INTEGER:
        t.value = yylval.intVal;
        break;
    case FLOAT:
        t.value = yylval.doubleVal;
        break;
    case STRING:
        t.value = yylval.rawStrVal;
        break;
    default:
        break;
    }
    return t;
}

std::string read_file(const std::string &filename)
{
    std::ifstream ifile(filename);
    if (!ifile)
        throw std::runtime_error("Cannot open file " + filename);
    std::string f, s;
    while (std::getline(ifile, s))
        f += s + "\n";
    return f;
}

Schema parse(Tokens &tokens)
{
    ast::Schema astSchema;
    ::schema = &astSchema;

    ParserMemoryManager mm;
    ::mm = &mm;

    int ret;
    YYSTYPE yylval = { 0 };
    YYLTYPE yylloc = { 0 };
    yyscan_t scanner;
    yydebug = 0;
    yypstate *ps = yypstate_new();
    auto t = tokens.begin();
    do {
        yylval = convert(*t);
        ret = yypush_parse(ps, t->token, &yylval, &yylloc);
        t++;
    } while (ret == YYPUSH_MORE);
    yypstate_delete(ps);

    if (ret)
        throw std::runtime_error("Error during parsing file");

    return convert(astSchema);
}

Schema parse_string(const std::string &s, Tokens *tokens)
{
    if (tokens)
    {
        tokens->clear();
        tokens->reserve(10000);
    }

    ast::Schema astSchema;
    ::schema = &astSchema;

    ParserMemoryManager mm;
    ::mm = &mm;

    int ret;
    YYSTYPE yylval = { 0 };
    YYLTYPE yylloc = { 0 };
    yyscan_t scanner;
    yylex_init(&scanner);
    yy_scan_string(s.c_str(), scanner);
    yydebug = 0;
    yypstate *ps = yypstate_new();
    do
    {
        ret = yylex(scanner, yylval, yylloc);
        if (tokens)
            tokens->push_back(convert(ret, yylval));
        ret = yypush_parse(ps, ret, &yylval, &yylloc);
    } while (ret == YYPUSH_MORE);
    yypstate_delete(ps);
    yylex_destroy(scanner);

    return convert(astSchema);
}

Schema parse_file(const std::string &filename, Tokens *tokens)
{
    auto s = read_file(filename);
    return parse_string(s, tokens);
}
