#pragma once

#include <assert.h>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "context.h"

using namespace std;

inline bool replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    bool replaced = false;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
         str.replace(start_pos, from.length(), to);
         start_pos += to.length();
         replaced = true;
    }
    return replaced;
}

class Column;
class Table;

enum class ColumnType
{
    Integer,
    Real,
    Text,
    Blob
};

typedef vector<string> PrimaryKeys;

struct ForeignKey
{
    string table_name;
    string column_name;

    Table *table;
    Column *column;
};
typedef map<string, ForeignKey> ForeignKeys;

class Column
{
    friend int yyparse();

public:
    string getName() const { return name; }
    string getCleanName() const { return removeId(name); }

    int getId() const { return id; };
    bool isId() const { return name == "id" && type == ColumnType::Integer; }

    ColumnType getType() const { return type; }
    string getCppType() const;
    
    void setPk() { pk = true; }
    void setFk(ForeignKey *fk) { this->fk = fk; }
    ForeignKey *getFk() const { return fk; }

    string getDefaultValue() const;

    string printVar() const;
    string printVarName() const;
    string printSet() const;
    string printSetPtr() const;
    
    string printLoadSqlite3(string var) const;

    string printSaveSqlite3(string var) const;

    bool operator<(const Column &rhs) const { return id < rhs.id; }

private:
    int id;
    string name;
    ColumnType type;
    string defaultValue;
    bool pk = false;
    ForeignKey *fk = 0;

    static string removeId(string s)
    {
        replaceAll(s, "_id", "");
        return s;
    }
};
typedef map<string, Column> Columns;

class Table
{
    friend int yyparse();

public:
    typedef map<string, Table> Tables;
    
public:
    void init(Tables &tables);

    string getName() const { return name; }
    string getCppName() const { return name.substr(0, name.size() - 1); }
    string getArrayName() const { return getCppName() + "s"; }
    string getVariableName() const { return toVarName(getCppName()); }
    string getVariableArrayName() const { return getVariableName() + "s"; }
        
    string getParentName() const;
    string getParentArrayName() const { return getParentName() + "s"; }
    string getParentVariableName() const { return toVarName(getParentName()); }
    string getParentVariableArrayName() const { return toVarName(getParentName()) + "s"; }

    string getChildName(const string &parentName) const { return getCppName().substr(getParentName().size()); }
    string getChildArrayName(const string &parentName) const { return getChildName(parentName) + "s"; }
    string getChildVariableName(const string &parentName) const { return toVarName(getChildName(parentName)); }
    string getChildVariableArrayName(const string &parentName) const { return getChildVariableName(parentName) + "s"; }
    
    const Column *getColumn(const string &name) const
    {
        Columns::const_iterator i;
        i = find_if(columns.begin(), columns.end(), [name](const Columns::value_type &p)
        {
            return p.second.getCleanName() == name;
        });
        if (i != columns.end())
            return &i->second;
        i = find_if(columns.begin(), columns.end(), [name](const Columns::value_type &p)
        {
            return p.second.getName() == name + "_id";
        });
        if (i != columns.end())
            return &i->second;
        auto iter = find_if(columns.begin(), columns.end(), [name](const Columns::value_type &p)
        {
            return p.second.getName().find(name) != -1;
        });
        if (iter != columns.end())
            return &iter->second;
        return 0;
    }
    const Column *getNameColumn() const { return getColumn("name"); }
    const Column *getTextColumn() const { return getColumn("text"); }

    const Columns &getColumns() const { return columns; }

    ModuleContext print() const;
    ModuleContext printIo() const;
    ModuleContext printAddDeleteRecord() const;
    ModuleContext printAddDeleteRecordVirtual() const;

    bool isVisibleInTreeView() const;
    bool isMap() const { return isMapTable; }
    bool hasParentTable() const { return hasParent; }

    bool operator<(const Table &rhs) const { return getCppName() < rhs.getCppName(); }

private:
    string name;
    Columns columns;
    PrimaryKeys pks;
    ForeignKeys fks;
    string sql;
    set<string> linksToThisTable;
    bool hasIdField = false;
    bool isProxy = false;
    bool container = false;
    bool hasParent = false;
    bool hasChild = false;
    bool isMapTable = false;
    vector<Table *> contains;
    
    static string toVarName(string s)
    {
        s[0] = tolower(s[0]);
        return s;
    }
};
typedef Table::Tables Tables;

class Database
{
    friend int yyparse();

public:
    void init();

    ModuleContext printObjectTypes();
    ModuleContext printObjectInterfaces();
    ModuleContext printTypes();
    ModuleContext printStorage();
    ModuleContext printStorageImpl();
    ModuleContext printHelpers();

private:
    Tables tables;
};
