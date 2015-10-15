#include "context.h"

Context::Context(const std::string &indent, const std::string &newline)
    : indent(indent), newline(newline)
{
}

void Context::addText(const std::string &s)
{
    text += s;
}

void Context::addNoNewLine(const std::string &s)
{
    text += space + s;
}

void Context::addLineNoSpace(const std::string & s)
{
    text += s + newline;
}

void Context::addLine()
{
    text += newline;
}

void Context::addLine(const std::string &s)
{
    text += space + s + newline;
}

void Context::decreaseIndent()
{
    if (space.size() >= indent.size())
        space.resize(space.size() - indent.size());
}

void Context::increaseIndent()
{
    space += indent;
}

void Context::beginBlock(const std::string &s, bool indent)
{
    if (!s.empty())
        addLine(s);
    addLine("{");
    if (indent)
        increaseIndent();
}

void Context::endBlock(bool semicolon)
{
    decreaseIndent();
    addLine(semicolon ? "};" : "}");
}

void Context::beginFunction(const std::string &s)
{
    beginBlock(s);
}

void Context::endFunction()
{
    endBlock();
    addLine();
}

void Context::beginNamespace(const std::string &s)
{
    addLineNoSpace("namespace " + s);
    addLineNoSpace("{");
    addLine();
    namespaces.push(s);
}

void Context::endNamespace()
{
    auto s = namespaces.top();
    namespaces.pop();
    addLineNoSpace("} // namespace " + s);
}

void Context::ifdef(const std::string &s)
{
    addLineNoSpace("#ifdef " + s);
}

void Context::endif()
{
    addLineNoSpace("#endif");
}

void Context::trimEnd(size_t n)
{
    auto sz = text.size();
    if (n > sz)
        n = sz;
    text.resize(sz - n);
}
