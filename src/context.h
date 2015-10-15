#pragma once

#include <string>
#include <stack>

class Context
{
public:
    Context(const std::string &indent = "    ", const std::string &newline = "\n");

    void addLine();
    void addLine(const std::string &s);
    void addNoNewLine(const std::string &s);
    void addLineNoSpace(const std::string &s);
    void addText(const std::string &s);

    void decreaseIndent();
    void increaseIndent();

    void beginBlock(const std::string &s = "", bool indent = true);
    void endBlock(bool semicolon = false);
    void beginFunction(const std::string &s = "");
    void endFunction();
    void beginNamespace(const std::string &s);
    void endNamespace();

    void ifdef(const std::string &s);
    void endif();

    void trimEnd(size_t n);

    std::string getText() const { return text; }

    Context &operator+=(const Context &rhs)
    {
        text += rhs.text;
        return *this;
    }

private:
    std::string text;
    std::string space;
    std::string indent;
    std::string newline;
    std::stack<std::string> namespaces;
};

struct ModuleContext
{
    Context cpp;
    Context hpp;

    ModuleContext() {}
    ModuleContext &operator+=(const ModuleContext &rhs)
    {
        cpp += rhs.cpp;
        hpp += rhs.hpp;
        return *this;
    }
};
