#include <filesystem>
#include <vector>
#include <iostream>
#include <fstream>

#include "Parser.h"
#include "Exporter.h"
#include "CmdLine.hpp"
#include "Colors.hpp"

int main(int argc, char* argv[])
{
    std::cout << "Parsing command line ..." << std::endl;
    CmdLine cmdLine(argc, argv);
    if (!cmdLine.ok())
        return 1;
    if (!cmdLine.canContinue())
        return 0;
    std::cout << std::endl;

    Parser     *parser = nullptr;
    Exporter    exporter;

    std::vector<Module> modules;

    std::cout << "Reading input file(s) ..." << std::endl;
    try
    {
        std::filesystem::path path = cmdLine.getPath();

        if (!std::filesystem::exists(path))
        {
            std::cerr << FORMAT_ERROR "path \"" << path.string() << "\" does not exist.\n";
            return 3;
        }

        if (!std::filesystem::is_directory(path))
        {
            std::cerr << FORMAT_ERROR "the specified path \"" << path.string() << "\" is not a directory.\n";
            return 3;
        }

        for (const auto &entry : std::filesystem::directory_iterator(path))
        {
            if (!entry.is_regular_file()) continue;

            // Эту работу у нас теперь делает GetParser
            //std::string ext = entry.path().extension().string();
            //if (ext != ".v" && ext != ".sv")
            //    continue;
            
            std::cout << FORMAT_INFO "processing file '" << entry.path().string() << "'\n";

            parser = GetParser(entry.path().string());
            if (!parser) {
                std::cerr << FORMAT_WARNING << "don't know how to read file '" << entry.path().string() << "', it will be skipped\n";
                continue;
            }

            if (parser->LoadFile(entry.path().string()))
            {
                auto parsed_modules = parser->Parse(entry.path().string());
                modules.insert(modules.end(), parsed_modules.begin(), parsed_modules.end());
            }
            else
            {
                std::cerr << FORMAT_WARNING "failed to load file.\n";
            }
            FreeParser(&parser);
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        std::cerr << FORMAT_ERROR << e.what() << std::endl;
        return 3;
    }
    std::cout << std::endl;

    std::cout << "Writing output..." << std::endl;
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
    std::cout << "Everything has been done!" << std::endl;

    return 0;
}
