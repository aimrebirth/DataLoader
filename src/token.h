#pragma once

#include <memory>
#include <vector>

#include <boost/variant.hpp>

#include "context.h"

union YYSTYPE;
struct YYLTYPE;

using BisonToken = int;

struct Token
{
    enum Type
    {
        Integer,
        Float,
        String,
        None,
    };

    using Value = boost::variant<int, double, std::string>;

    BisonToken token;
    Value value;

    void print(ModuleContext &mc)
    {
        std::string s = "{ " + std::to_string(token) + ", ";
        switch (value.which())
        {
        case Integer:
            s += std::to_string(boost::get<int>(value));
            break;
        case Float:
            s += std::to_string(boost::get<double>(value));
            break;
        case String:
            s += "\"" + boost::get<std::string>(value) + "\"";
            break;
        default:
            break;
        }
        s += " },";
        mc.cpp.addLine(s);
    }
};
using Tokens = std::vector<Token>;

ModuleContext print(Tokens &tokens);
