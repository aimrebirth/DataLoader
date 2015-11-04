#pragma once

#include <assert.h>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "context.h"
#include "ast.h"

DataType dataTypeFromName(const Name &name);

class Schema;

class ObjectWithFlags
{
public:
    const ObjectFlags &getFlags() const { return flags; }

    bool hasFlags(const std::vector<ObjectFlag> &in_flags, bool revert = false) const
    {
        ObjectFlags flags;
        for (auto &f : in_flags)
            flags.set(f);
        ObjectFlags r = this->flags;
        if (revert)
            r = r ^ flags;
        r = r & flags;
        r = r ^ flags;
        return r.none();
    }
    bool hasFlags(const std::initializer_list<ObjectFlag> &in_flags, bool revert = false) const
    {
        return hasFlags(std::vector<ObjectFlag>(in_flags), revert);
    }
    bool hasFlags(ObjectFlags flags, bool revert = false) const
    {
        if (revert)
            flags.flip();
        return flags == this->flags;
    }

    void setFlags(const ObjectFlags &flags) { this->flags = flags; }

protected:
    ObjectFlags flags;
};

template <class Container>
class ObjectArray : public Container
{
public:
    using Parent = Container;

public:
    template <class T>
    ObjectArray filter(const T &flags, bool revert = false, bool sort = true) const
    {
        ObjectArray objects;
        for (auto &c : *this)
        {
            if (c.hasFlags(flags, revert))
                objects.push_back(c);
        }
        auto f = { fFirst };
        auto l = { fLast };
        if (sort)
        {
            this->sort(objects, [&](const auto &c1, const auto &c2)
            {
                if (c1.hasFlags(f))
                {
                    if (c2.hasFlags(f))
                        return c1 < c2;
                    return true;
                }
                if (c2.hasFlags(f))
                    return false;
                if (c1.hasFlags(l))
                {
                    if (c2.hasFlags(l))
                        return c1 < c2;
                    return false;
                }
                if (c2.hasFlags(l))
                    return true;
                return c1 < c2;
            });
        }
        return objects;
    }

    ObjectArray operator()(const std::initializer_list<ObjectFlag> &flags, bool revert = false, bool sort = true) const
    {
        return filter(std::vector<ObjectFlag>(flags), revert, sort);
    }
    ObjectArray operator()(const std::vector<ObjectFlag> &flags, bool revert = false, bool sort = true) const
    {
        return filter<std::vector<ObjectFlag>>(flags, revert, sort);
    }
    ObjectArray operator()(ObjectFlags flags, bool revert = false, bool sort = true) const
    {
        return filter<ObjectFlags>(flags, revert, sort);
    }

    template <typename F>
    static void sort(std::vector<typename Container::value_type> &v, F f)
    {
        std::sort(v.begin(), v.end(), f);
    }
    template <typename F>
    static void sort(std::list<typename Container::value_type> &v, F f)
    {
        v.sort(f);
    }
};

class Type : public ObjectWithFlags
{
public:
    Name getName() const
    {
        return name;
    }
    Name getCppName() const
    {
        return name;
    }
    Name getCppArrayName() const
    {
        return name + "s";
    }
    Name getCppArrayVariableName() const
    {
        auto n = getCppArrayName();
        n[0] = tolower(n[0]);
        return n;
    }
    Name getCppVariableName() const
    {
        auto n = name;
        n[0] = tolower(n[0]);
        return n;
    }
    Name getHumanName() const
    {
        return humanName.empty() ? getCppName() : humanName;
    }
    Name getSqlName() const { return getCppArrayName(); }
    virtual Type *getParent() const { return nullptr; }
    DataType getDataType() const { return dataType; }

    virtual bool isSimple() const { return true; }
    bool isComplex() const { return !isSimple(); }

    virtual void printVariables(ModuleContext &mc) const {}

    bool operator<(const Type &rhs) const
    {
        return name < rhs.name;
    }

protected:
    Name name;
    Name humanName;
    DataType dataType;

    friend Schema convert(const ast::Schema &schema);
};

using Types = std::set<Type>;

struct Database
{
    Name name;
    Types types;
};

using Databases = std::vector<Database>;

class Variable : public ObjectWithFlags
{
public:
    Name getName() const { return name; }
    Name getSqlName() const
    {
        if (type->getDataType() == DataType::Complex)
            return getName() + "_id";
        return getName();
    }
    Name getNameWithCaptitalLetter() const
    {
        auto n = name;
        n[0] = toupper(n[0]);
        return n;
    }
    const Type *getType() const { return type; }
    int getId() const { return id; }
    void setId(int id) { this->id = id; }
    std::string getDefaultValue() const;
    std::string getRawDefaultValue() const { return defaultValue; }

    bool isId() const { return name == "id"; }
    bool isFk() const { return type->isComplex(); }

    bool operator<(const Variable &rhs) const
    {
        return name < rhs.name;
    }

    std::string print() const;
    std::string printSet() const;
    std::string printSetPtr() const;
    std::string printLoadSqlite3(std::string var) const;
    std::string printSaveSqlite3(std::string var) const;

private:
    int id;
    Name name;
    Type *type;
    std::string defaultValue;

    friend Schema convert(const ast::Schema &schema);
};

using Variables = ObjectArray<std::vector<Variable>>;

class Class : public Type
{
public:
    void addVariable(const Variable &v)
    {
        if (v.isId())
            hasIdField = true;
        variables.push_back(v);
    }
    
    Variables getVariables(bool container = false) const;
    Name getEnumName() const { return enumName; }
    virtual Class *getParent() const override { return parent; }
    
    const Variable *getVariable(const std::string &name) const
    {
        Variables::const_iterator i;
        i = find_if(variables.begin(), variables.end(), [name](const auto &p)
        {
            return p.getName() == name;
        });
        if (i != variables.end())
            return &*i;
        i = find_if(variables.begin(), variables.end(), [name](const auto &p)
        {
            return p.getName() == name + "_id";
        });
        if (i != variables.end())
            return &*i;
        i = find_if(variables.begin(), variables.end(), [name](const auto &p)
        {
            return p.getName().find(name) != -1;
        });
        if (i != variables.end())
            return &*i;
        return nullptr;
    }
    const Variable *getNameVariable() const { return getVariable("name"); }
    const Variable *getTextVariable() const { return getVariable("text"); }

    virtual bool isSimple() const override { return false; }

    std::string printSql() const;
    virtual void printVariables(ModuleContext &mc) const override;

    ModuleContext print() const;
    ModuleContext printIo() const;
    ModuleContext printAddDeleteRecordVirtual() const;
    ModuleContext printAddDeleteRecord() const;
    
private:
    Variables variables;
    Name enumName;
    std::vector<ObjectName> namesOrder =
    {
        ObjectName::Custom,
        ObjectName::Name,
        ObjectName::Text,
        ObjectName::Parent
    };
    std::string objectName;
    Class *parent = nullptr;
    std::vector<Class *> children;
    bool hasIdField = false;

    friend Schema convert(const ast::Schema &schema);
};

using Classes = ObjectArray<std::list<Class>>;

class Schema
{
public:
    Class &getClass(const Name &name)
    {
        auto i = std::find_if(classes.begin(), classes.end(), [&name](auto &c)
            { return name == c.getCppName(); });
        if (i == classes.end())
            throw std::runtime_error("No class with such name: " + name);
        return *i;
    }
    Classes getClasses() const
    {
        return classes({ fInline, fEnumOnly }, true);
    }

    ModuleContext printObjectTypes() const;
    ModuleContext printHelpers() const;
    ModuleContext printObjectInterfaces() const;
    ModuleContext printTypes() const;
    ModuleContext printStorage() const;
    ModuleContext printStorageImplementation() const;

private:
    Version version;
    Types types;
    Classes classes;
    Databases databases;
    std::unordered_map<Name, Type*> typePtrs;

    friend Schema convert(const ast::Schema &schema);
};

Schema convert(const ast::Schema &schema);











/*
class Column;
class Table;


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
*/
