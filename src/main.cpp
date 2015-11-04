#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

#include <ast.h>
#include <parser_mm.h>

namespace fs = boost::filesystem;

// Prevent using <unistd.h> because of bug in flex.
#define YY_NO_UNISTD_H 1
#include <lexer.h>
#include "data.h"

extern int yydebug;
extern ParserMemoryManager *mm;
int yyparse();

ast::Schema *schema;

int main(int argc, char *argv[])
try
{
    if (argc < 2)
    {
        printf("Usage: %s file.sql out_dir\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        printf("Cannot open file %s\n", argv[1]);
        return 2;
    }

    ast::Schema astSchema;
    ::schema = &astSchema;

    yydebug = 0;
    yyset_in(f);
    {
        ParserMemoryManager mm;
        ::mm = &mm;
        int ret = yyparse();
        if (ret)
        {
            printf("Error during parsing sql file\n");
            return 3;
        }
    }

    Schema schema = convert(astSchema);

    fs::path p = "DataManager";
    if (argc == 3)
        p = argv[2] / p;
    auto header = p / "include" / "Polygon4" / "DataManager" / "detail";
    auto src = p / "src" / "detail";

    fs::create_directories(header);
    fs::create_directories(src);

    auto printModule = [&header, &src](const std::string &name, const auto &module)
    {
        auto t = module.hpp.getText();
        if (!t.empty())
            std::ofstream(fs::path(header / (name + ".h")).string()) << t;
        t = module.cpp.getText();
        if (!t.empty())
            std::ofstream(fs::path(src / (name + ".cpp")).string()) << t;
    };

    printModule("ObjectTypes", schema.printObjectTypes());
    printModule("ObjectInterfaces", schema.printObjectInterfaces());
    printModule("Types", schema.printTypes());
    printModule("Storage", schema.printStorage());
    printModule("StorageImpl", schema.printStorageImplementation());
    printModule("Helpers", schema.printHelpers());

    return 0;
}
catch (std::exception &e)
{
    std::cerr << e.what() << "\n";
    return 1;
}
catch (...)
{
    std::cerr << "unknown exception" << "\n";
    return 1;
}
