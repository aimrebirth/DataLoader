#include <fstream>
#include <iostream>

// Prevent using <unistd.h> because of bug in flex.
#define YY_NO_UNISTD_H 1
#include <lexer.h>
#include "data.h"

int yyparse();
extern int yydebug;

extern Database db;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s file.sql", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        printf("Cannot open file %s", argv[1]);
        return 2;
    }
    yyset_in(f);
    yydebug = 0;
    int ret = yyparse();
    if (ret)
    {
        printf("Error during parsing sql file");
        return 3;
    }
    db.assignFks();
    std::string impl;
    ofstream("Types.h") << db.printTypes(impl);
    ofstream("Types.cpp") << impl;
    //ofstream("TypesUsing.h") << db.printTypesUsing();
    ofstream("Storage.h") << db.printStorage();
    impl.clear();
    ofstream("StorageImpl.h") << db.printStorageImpl(impl);
    ofstream("StorageImpl.cpp") << impl;
    impl.clear();
    ofstream("Helpers.h") << db.printHelpers(impl);
    ofstream("Helpers.cpp") << impl;
    ofstream("dbmgr_data.py") << db.printPy();
    return 0;
}