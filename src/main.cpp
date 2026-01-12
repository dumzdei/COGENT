#include "Parser.h"
#include "Exporter.h"
#include "CmdLine.hpp"
#include <filesystem>
#include <vector>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    CmdLine cmdLine(argc, argv);
    if (!cmdLine.ok())
        return 1;
    if (!cmdLine.canContinue())
        return 0;

    Parser      parser;
    Exporter    exporter;

    std::vector<Module> modules;

    try
    {
        std::filesystem::path path = cmdLine.getPath();

        if (!std::filesystem::exists(path))
        {
            std::cerr << "\033[31m__err__\033[0m : path \"" << path.string() << "\" does not exist.\n";
            return 3;
        }

        if (!std::filesystem::is_directory(path))
        {
            std::cerr << "\033[31m__err__\033[0m : the specified path \"" << path.string() << "\" is not a directory.\n";
            return 3;
        }

        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();
            if (ext != ".v" && ext != ".sv")
                continue;

            std::cout << "Processing file: " << entry.path() << "\n";

            if (parser.loadFile(entry.path().string()))
            {
                auto parsed_modules = parser.parse(entry.path().string());
                modules.insert(modules.end(), parsed_modules.begin(), parsed_modules.end());
            }
            else
            {
                std::cerr << "\033[33m__wrn__\033[0m : Failed to load file.\n";
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << "\033[31m__err__\033[0m : " << e.what() << std::endl;
        return 3;
    }

    switch (cmdLine.getOutputFormat()) {
        case OutputFormat::html:
            exporter.Export_to_HTML(modules, cmdLine.getThemeName());
            break;
        case OutputFormat::markdown:
            exporter.Export_to_MD(modules);
            break;
        case OutputFormat::asciidoc:
            exporter.Export_to_ADOC(modules);
            break;
    }

    return 0;
}
