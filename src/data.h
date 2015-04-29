#pragma once

#include <map>
#include <set>
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

struct ForeignKey
{
    string table_name;
    string column_name;
};
typedef map<string, ForeignKey> ForeignKeys;

struct Column
{
    int id;
    string name;
    ColumnType type;
    ForeignKey *fk = 0;

    string printVar() const;
    string printVarName() const;
    string printLoad() const;
    string printSet() const;
    string printSave(string var) const;

    bool operator<(const Column &rhs) const
    {
        return id < rhs.id;
    }
};
typedef map<string, Column> Columns;

struct Table
{
    typedef map<string, Table> Tables;

    string name;
    Columns columns;
    ForeignKeys fks;
    set<string> linksToThisTable;

    bool hasIdField() const;

    string printForward() const;
    string print(const Tables &tables);
    string printIo(const Tables &tables);
    string printLoad();
    string printLoadPtrs();
    string printLoadArrays(const Tables &tables);
    string printSave();
    string printUsing();

    bool operator<(const Table &rhs) const
    {
        if (linksToThisTable.find(rhs.name) != linksToThisTable.end())
            return true;
        if (rhs.linksToThisTable.find(name) != rhs.linksToThisTable.end())
            return false;
        if (linksToThisTable.size() != rhs.linksToThisTable.size())
            return linksToThisTable.size() > rhs.linksToThisTable.size();
        return name < rhs.name;
    }
};
typedef Table::Tables Tables;

struct Database
{
    Tables tables;

    void assignFks();
    string printTypes();
    string printTypesUsing();
    string printStorage();
    string printStorageImpl();
    string printHelpers();
};