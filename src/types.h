#pragma once

#include <bitset>
#include <string>

using Name = std::string;
using Key = std::string;
using Value = std::string;

enum ObjectFlag
{
    fAll,

    // classes
    fFirst,
    fLast,
    fCreateEnum,
    fEnumOnly,
    fService,
    fInline,
    fProxy,
    fTreeView,

    // fields
    fPrimaryKey,
    fEnumItem,
    fContainer,

    fMaxFlag,
};
using ObjectFlags = std::bitset<fMaxFlag>;

using Version = std::string;

enum class DataType
{
    Bool,
    Integer,
    Real,
    Text,
    Blob,
    Complex,
};

enum class ObjectName
{
    Text,
    Name,
    Parent,
    Custom,
};

template <class T>
bool replaceAll(T &str, const T &from, const T &to)
{
    bool replaced = false;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != T::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
        replaced = true;
    }
    return replaced;
}

inline bool replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    return replaceAll<std::string>(str, from, to);
}
