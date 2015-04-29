#include "data.h"

#include <algorithm>

string space = "    ";
string String = "String";
string TextType = "Text";

set<string> containers = 
{
    "Configuration",
    "MapBuilding",
    "QuestReward",
    "Modification",
    "Clan",
    "Save",
};

map<string, string> containers1n = 
{
    { "Save", "ScriptVariable" }
};

bool replaceAll(std::string &str, const std::string &from, const std::string &to)
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

string getTableName(string s)
{
    if (s.empty())
        return s;
    if (s[s.size() - 1] == 's')// && s.find("Goods") == -1)
        s = s.substr(0, s.size() - 1);
    return s;
}

string removeId(string s)
{
    replaceAll(s, "_id", "");
    //if (s.find("goods") != -1)
    //    s = s.substr(0, s.size() - 1);
    return s;
}

string toVarName(string s)
{
    s[0] = tolower(s[0]);
    return s;
}

template<typename T, typename K>
set<T> mapToSet(const map<K, T> &map)
{
    set<T> s;
    for (auto &v : map)
        s.insert(v.second);
    return s;
}

string Column::printVar() const
{
    string s;
    if (fk)
    {
        s += "IdPtr<" + getTableName(fk->table_name) + "> " + printVarName();
    }
    else
    {
        switch (type)
        {
        case ColumnType::Integer:
            s += "int";
            break;
        case ColumnType::Real:
            s += "float";
            break;
        case ColumnType::Text:
            s += TextType;
            break;
        }
        s += " " + printVarName();
        if (type == ColumnType::Integer || type == ColumnType::Real)
        {
            s += " = ";
            switch (type)
            {
            case ColumnType::Integer:
                s += "0";
                break;
            case ColumnType::Real:
                s += "0.0f";
                break;
            }
        }
    }
    return s;
}

string Column::printVarName() const
{
    string s;
    if (fk)
    {
        s += toVarName(removeId(name));
    }
    else
    {
        s += name;
    }
    return s;
}

string Column::printLoad() const
{
    string s;
    if (fk)
    {
        s += toVarName(removeId(name)) + ".id = std::stoi(cols[" + to_string(id) + "])";
    }
    else
    {
        s += name + " = ";
        switch (type)
        {
        case ColumnType::Integer:
            s += "std::stoi(cols[" + to_string(id) + "])";
            break;
        case ColumnType::Real:
            s += "std::stof(cols[" + to_string(id) + "])";
            break;
        case ColumnType::Text:
            s += "cols[" + to_string(id) + "]";
            break;
        }
    }
    return s;
}

string Column::printSet() const
{
    string s;
    if (fk)
    {
        s += toVarName(removeId(name)) + ".id = std::stoi(text.string())";
    }
    else
    {
        s += name + " = ";
        switch (type)
        {
        case ColumnType::Integer:
            s += "std::stoi(text.string())";
            break;
        case ColumnType::Real:
            s += "std::stof(text.string())";
            break;
        case ColumnType::Text:
            s += "text.string()";
            break;
        }
    }
    return s;
}

string Column::printSave(string var) const
{
    string s;
    if (fk)
    {
        s += "std::to_string(" + var + "->" + removeId(name) + ".id)";
    }
    else
    {
        switch (type)
        {
        case ColumnType::Integer:
        case ColumnType::Real:
            s += "std::to_string(" + var + "->" + name + ")";
            break;
        case ColumnType::Text:
            s += var + "->" + name + ".string()";
            break;
        }
    }
    return s;
}

string Table::printForward() const
{
    string s;
    s += "struct " + getTableName(this->name) + ";\n";
    return s;
}

string Table::print(const Tables &tables)
{
    string s;
    string name = getTableName(this->name);
    set<Column> cols = mapToSet(columns);
    if (name == String)
    {
        s += "enum class LocalizationType : EnumType\n";
        s += "{\n";
        for (auto i = next(cols.begin()); i != cols.end(); i++)
            s += space + i->name + ",\n";
        s += "\n";
        s += space + "max\n";
        s += "};\n\n";
    }
    s += "struct " + name + " : public IObject\n";
    s += "{\n";
    for (auto &col : cols)
        s += space + col.printVar() + ";\n";
    if (containers.find(name) != containers.end())
    {
        s += "\n";
        for (auto &table : tables)
        {
            if (table.first.find(name) == 0 && isupper(table.first[name.size()]))
            {
                string var = table.first.substr(name.size());
                s += space + "CVector<Ptr<" + getTableName(table.first) + ">> " + toVarName(var) + ";\n";
            }
        }
    }
    auto iter1n = containers1n.find(name);
    if (iter1n != containers1n.end())
    {
        s += "\n";
        s += space + "CVector<Ptr<" + getTableName(iter1n->second) + ">> " + toVarName(iter1n->second) + "s" + ";\n";
    }
    s += "\n";

    s += space + "virtual EObjectType getType() const\n";
    s += space + "{\n";
    s += space + space + "return EObjectType::" + name + ";\n";
    s += space + "}\n";

    s += space + "virtual Text getVariableString(int columnId) const\n";
    s += space + "{\n";
    {
        string space = ::space + ::space;
        s += space + "switch (columnId)\n";
        s += space + "{\n";
        auto cols = mapToSet(columns);
        for (auto &col : cols)
        {
            s += space + "case " + to_string(col.id) + ":\n";
            if (col.type == ColumnType::Real)
            {
                s += space + ::space + "{ std::stringstream ss; ss << " + col.printVarName() + "; return ss.str(); }\n";
                continue;
            }
            s += space + ::space + "return to_string(" + col.printVarName() + ");\n";
        }
        s += space + "default:\n";
        s += space + ::space + "return \"\";\n";
        s += space + "}\n";
        s += space + "return \"\";\n";
    }
    s += space + "}\n";

    s += space + "virtual void setVariableString(int columnId, Text text)\n";
    s += space + "{\n";
    {
        string space = ::space + ::space;
        s += space + "switch (columnId)\n";
        s += space + "{\n";
        auto cols = mapToSet(columns);
        for (auto &col : cols)
        {
            s += space + "case " + to_string(col.id) + ":\n";
            s += space + ::space + col.printSet() + ";\n";
            s += space + ::space + "break;\n";
        }
        s += space + "default:\n";
        s += space + ::space + "break;\n";
        s += space + "}\n";
    }
    s += space + "}\n";

    if (name == String)
    {
        s += "\n";
        s += space;
        s += TextType + " get(int loc_id = 0) const { return ";
        s += "((" + TextType + " *)&" + next(cols.begin())->name + ")[loc_id]";
        s += "; }\n";
    }
    s += "};\n\n";
    return s;
}

bool Table::hasIdField() const
{
    auto iter = columns.find("id");
    return iter != columns.end() && iter->second.type == ColumnType::Integer;
}

string Table::printIo(const Tables &tables)
{
    string s;
    s += printLoad();
    s += printLoadPtrs();
    s += printLoadArrays(tables);
    s += printSave();
    return s;
}

string Table::printLoad()
{
    string s;
    s += space + "void _load" + name + "()\n";
    s += space + "{\n";
    {
        string space = ::space + ::space;
        s += space + "auto callback = [](void *o, int ncols, char **cols, char **names)" + "\n";
        s += space + "{" + "\n";
        {
            string space = ::space + ::space + ::space;
            set<Column> cols = mapToSet(columns);
            string name = getTableName(this->name);
            string var = toVarName(name);
            s += space + "Ptr<" + name + "> " + var + " = std::make_shared<" + name + ">();" + "\n";
            for (auto &col : cols)
                s += space + "if (cols[" + to_string(col.id) + "]) " + var + "->" + col.printLoad() + ";\n";
            s += "\n";
            s += space + "Storage *storage = (Storage *)o;" + "\n";
            name = toVarName(this->name);
            if (hasIdField())
                s += space + "storage->" + name + "[" + var + "->id] = " + var + ";" + "\n";
            else
                s += space + "storage->" + name + ".push_back(" + var + ");" + "\n";
            s += space + "return 0;" + "\n";
        }
        s += space + "};" + "\n";
        s += space + "db->execute(\"select * from " + name + ";\", this, callback);\n";
    }
    s += space + "}\n";
    return s;
}

string Table::printLoadPtrs()
{
    string s;
    s += space + "void _load" + name + "Ptrs()\n";
    s += space + "{\n";
    {
        set<Column> cols = mapToSet(columns);
        bool found = false;
        for (auto &col : cols)
        {
            auto iter = fks.find(col.name);
            if (iter != fks.end())
            {
                found = true;
            }
        }
        if (found)
        {
            string space = ::space + ::space;
            s += space + "for (auto &" + getTableName(toVarName(name)) + " : " + toVarName(name) + ")\n";
            s += space + "{\n";
            int col_id = 0;
            for (auto &col : cols)
            {
                string space = ::space + ::space + ::space;
                string name = getTableName(this->name);
                string var = toVarName(name);
                auto iter = fks.find(col.name);
                if (iter != fks.end())
                {
                    s += space;
                    string name2 = toVarName(iter->second.table_name);
                    if (hasIdField())
                        s += getTableName(toVarName(name)) + ".second->" + removeId(col.name) + ".ptr = " +
                            name2 + "[" + getTableName(toVarName(name)) + ".second->" + removeId(col.name) + ".id]";
                    else
                        s += getTableName(toVarName(name)) + "->" + removeId(col.name) + ".ptr = " +
                            name2 + "[" + getTableName(toVarName(name)) + "->" + removeId(col.name) + ".id]";
                    s += ";\n";
                }
                col_id++;
            }
            s += space + "}\n";
        }
    }
    s += space + "}\n";
    return s;
}

string Table::printLoadArrays(const Tables &tables)
{
    string s;
    s += space + "void _load" + name + "Arrays()\n";
    s += space + "{\n";
    {
        string space = ::space + ::space;
        string name = getTableName(this->name);
        set<Column> cols = mapToSet(columns);
        bool once = false;
        if (containers.find(name) != containers.end())
        {
            for (auto &table : tables)
            {
                if (table.first.find(name) == 0 && isupper(table.first[name.size()]))
                {
                    if (!once)
                    {
                        s += space + "for (auto &" + toVarName(getTableName(this->name)) + " : " + toVarName(this->name) + ")" + "\n";
                        s += space + "{\n";
                        once = true;
                    }
                    {
                        string space = ::space + ::space + ::space;
                        string var = table.first.substr(name.size());
                        s += space + "for (auto &" + toVarName(getTableName(table.first)) + " : " + toVarName(table.first) + ")\n";
                        {
                            string space = ::space + ::space + ::space + ::space;
                            s += space + "if (" + toVarName(getTableName(this->name)) + ".first == " + 
                                toVarName(getTableName(table.first)) + "->" + toVarName(getTableName(this->name)) + ".id)\n";
                            {
                                string space = ::space + ::space + ::space + ::space + ::space;
                                s += space + toVarName(getTableName(this->name)) + ".second->" + toVarName(var) + ".push_back(" + toVarName(getTableName(table.first)) + ");\n";
                            }
                        }
                    }
                }
            }
        }
        auto iter1n = containers1n.find(name);
        if (iter1n != containers1n.end())
        {
            if (!once)
            {
                s += space + "for (auto &" + toVarName(getTableName(this->name)) + " : " + toVarName(this->name) + ")" + "\n";
                s += space + "{\n";
                once = true;
            }
            string space = ::space + ::space + ::space;
            s += space + "for (auto &" + toVarName(iter1n->second) + " : " + toVarName(iter1n->second) + "s" + ")\n";
            {
                string space = ::space + ::space + ::space + ::space;
                s += space + "if (" + toVarName(getTableName(this->name)) + ".first == " + 
                    toVarName(getTableName(iter1n->second)) + "->" + toVarName(getTableName(this->name)) + ".id)\n";
                {
                    string space = ::space + ::space + ::space + ::space + ::space;
                    s += space + toVarName(getTableName(this->name)) + ".second->" + toVarName(iter1n->second) + "s" +
                        ".push_back(" + toVarName(getTableName(iter1n->second)) + ");\n";
                }
            }
        }
        if (once)
        {
            s += space + "}\n";
        }
    }
    s += space + "}\n";
    return s;
}

string Table::printSave()
{
    string s;
    s += space + "void _save" + name + "()\n";
    s += space + "{\n";
    {
        string space = ::space + ::space;
        s += space + "if (" + toVarName(name) + ".empty())" + "\n";
        s += space + ::space + "return;\n";
        s += space + "std::string query;" + "\n";
        s += space + "query += \"insert or replace into " + name + " values\\n\"" + ";\n";
        s += space + "for (auto &" + toVarName(getTableName(name)) + " : " + toVarName(name) + ")" + "\n";
        s += space + "{\n";
        {
            string space = ::space + ::space + ::space;
            s += space + "query += \"(\";\n";
            auto cols = mapToSet(columns);
            for (auto &col : cols)
                s += space + "query += \"'\" + " + col.printSave(toVarName(getTableName(name)) + (hasIdField() ? ".second" : "")) + " + \"',\";\n";
            s += space + "query.resize(query.size() - 1);\n";
            s += space + "query += \"),\\n\";\n";
        }
        s += space + "}\n";
        s += space + "query.resize(query.size() - 2);\n";
        s += space + "query += \";\";\n";
        s += space + "db->execute(query.c_str(), 0, 0);\n";
    }
    s += space + "}\n";
    return s;
}

string Table::printUsing()
{
    string s;
    s += "using detail::" + getTableName(this->name) + ";\n";
    return s;
}

void Database::assignFks()
{
    for (auto &table : tables)
    {
        for (auto &fk : table.second.fks)
        {
            table.second.columns[fk.first].fk = &fk.second;
            tables[fk.second.table_name].linksToThisTable.insert(table.first);
        }
    }
}

string Database::printTypes()
{
    string s;
    s += "/* DO NOT EDIT! This is an autogenerated file. */\n\n";
    s += "enum class EObjectType : EnumType\n";
    s += "{\n";
    for (auto &table : tables)
        s += space + getTableName(table.first) + ",\n";
    s += "};\n\n";
    for (auto &table : tables)
        s += table.second.printForward();
    s += "\n";
    s += "struct IObject\n";
    s += "{\n";
    s += space + "virtual ~IObject(){}\n";
    s += "\n";
    s += space + "virtual EObjectType getType() const = 0;\n";
    s += space + "virtual Text getVariableString(int columnId) const = 0;\n";
    s += space + "virtual void setVariableString(int columnId, Text text) = 0;\n";
    s += "};\n\n";
    for (auto &table : tables)
        s += table.second.print(tables);
    return s;
}

string Database::printStorage()
{
    string s;
    s += "/* DO NOT EDIT! This is an autogenerated file. */\n\n";
    s += "class Storage\n";
    s += "{\n";
    s += "public:\n";
    for (auto &table : tables)
    {
        string name = toVarName(table.first);
        string container = table.second.hasIdField() ? "CMap" : "CVector";
        s += space + container + "<Ptr<" + getTableName(table.first) + ">> " + name + ";\n";
    }
    s += "\n";
    s += space + "virtual void clear() = 0;\n";
    s += space + "virtual void load() = 0;\n";
    s += space + "virtual void save() = 0;\n";
    s += "};\n\n";
    return s;
}

string Database::printStorageImpl()
{
    string s;
    s += "/* DO NOT EDIT! This is an autogenerated file. */\n\n";
    s += "class StorageImpl : public Storage\n";
    s += "{\n";
    s += "private:\n";
    s += space + "Ptr<Database> db;" + "\n";
    s += "\n";
    s += "private:\n";
    for (auto &table : tables)
        s += table.second.printIo(tables) + "\n";
    s += "public:\n";
    s += space + "StorageImpl(Ptr<Database> db) : db(db) {}" + "\n";
    s += "\n";
    s += space + "virtual void clear()\n";
    s += space + "{\n";
    for (auto &table : tables)
        s += space + space + toVarName(table.first) + ".clear();\n";
    s += space + "}\n";
    s += space + "virtual void load()\n";
    s += space + "{\n";
    for (auto &table : tables)
        s += space + space + "_load" + table.first + "();\n";
    s += "\n";
    for (auto &table : tables)
        s += space + space + "_load" + table.first + "Ptrs();\n";
    s += "\n";
    for (auto &table : tables)
        s += space + space + "_load" + table.first + "Arrays();\n";
    s += space + "}\n";
    s += space + "virtual void save()\n";
    s += space + "{\n";
    auto tbls = mapToSet(tables);
    for (auto &table : tbls)
        s += space + space + "_save" + table.name + "();\n";
    s += space + "}\n";
    s += "};\n\n";
    return s;
}

string Database::printTypesUsing()
{
    string s;
    s += "/* DO NOT EDIT! This is an autogenerated file. */\n\n";
    for (auto &table : tables)
        s += table.second.printUsing();
    return s;
}

string Database::printHelpers()
{
    string s;
    s += "/* DO NOT EDIT! This is an autogenerated file. */\n\n";

    s += "EObjectType getTableType(const std::string &table)\n";
    s += "{\n";
    s += space + "static std::map<std::string, EObjectType> types =\n";
    s += space + "{\n";
    {
        string space = ::space + ::space;
        for (auto &table : tables)
            s += space + "{ \"" + table.first + "\", EObjectType::" + getTableName(table.first) + " },\n";
    }
    s += space + "};\n";
    s += space + "return types[table];\n";
    s += "};\n\n";
    
    s += "std::string getTableNameByType(EObjectType type)\n";
    s += "{\n";
    s += space + "static std::map<EObjectType, std::string> tables =\n";
    s += space + "{\n";
    {
        string space = ::space + ::space;
        for (auto &table : tables)
            s += space + "{ EObjectType::" + getTableName(table.first) + ", \"" + table.first + "\" },\n";
    }
    s += space + "};\n";
    s += space + "return tables[type];\n";
    s += "};\n\n";

    return s;
}