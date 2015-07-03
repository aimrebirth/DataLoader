#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

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

    fs::path p = "DatabaseManager";
    auto header = p / "include" / "Polygon4" / "detail";
    auto src = p / "src" / "detail";

    fs::create_directories(header);
    fs::create_directories(src);

    std::string impl;
    ofstream(fs::path(header / "Types.h").string()) << db.printTypes(impl);
    ofstream(fs::path(src / "Types.cpp").string()) << impl;
    //ofstream(fs::path(header / "TypesUsing.h").string()) << db.printTypesUsing();
    ofstream(fs::path(header / "Storage.h").string()) << db.printStorage();
    impl.clear();
    ofstream(fs::path(header / "StorageImpl.h").string()) << db.printStorageImpl(impl);
    ofstream(fs::path(src / "StorageImpl.cpp").string()) << impl;
    impl.clear();
    ofstream(fs::path(header / "Helpers.h").string()) << db.printHelpers(impl);
    ofstream(fs::path(src / "Helpers.cpp").string()) << impl;

    ofstream("dbmgr_data.py") << db.printPy();

    return 0;
}