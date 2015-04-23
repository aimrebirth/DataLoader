#pragma once

#include <map>
#include <string>
#include <vector>

using namespace std;

enum class ColumnType
{
    Integer,
    Real,
    Text,
    Blob
};

struct Column
{
    string name;
    ColumnType type;
};

struct ForeignKey
{
    string table_name;
    string column_name;
};
typedef map<string, ForeignKey> ForeignKeys;

struct Table
{
    vector<Column> columns;
    ForeignKeys fks;
};
typedef map<string, Table> Tables;