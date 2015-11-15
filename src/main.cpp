#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

#include "data.h"
#include "parser.h"

int main(int argc, char *argv[])
try
{
    if (argc < 2)
    {
        printf("Usage: %s schema.txt out_dir\n", argv[0]);
        return 1;
    }

    Tokens ts;
    auto schema = parse_file(argv[1], &ts);

    fs::path p = "DataManager1";
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
    printModule("Tokens", print(ts));

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
