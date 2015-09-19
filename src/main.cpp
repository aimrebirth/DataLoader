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

Database db;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s file.sql out_dir", argv[0]);
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

    fs::path p = "DatabaseManager";
    if (argc == 3)
        p = argv[2] / p;
    auto header = p / "include" / "Polygon4" / "detail";
    auto src = p / "src" / "detail";

    fs::create_directories(header);
    fs::create_directories(src);

    auto printModule = [&header, &src](const std::string &name, const auto &module)
    {
        auto t = module.hpp.getText();
        if (!t.empty())
            ofstream(fs::path(header / (name + ".h")).string()) << t;
        t = module.cpp.getText();
        if (!t.empty())
            ofstream(fs::path(src / (name + ".cpp")).string()) << t;
    };

    db.init();
    printModule("ObjectTypes", db.printObjectTypes());
    printModule("Types", db.printTypes());
    printModule("Storage", db.printStorage());
    printModule("StorageImpl", db.printStorageImpl());
    printModule("Helpers", db.printHelpers());

    return 0;
}