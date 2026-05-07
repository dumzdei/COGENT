#include <filesystem>
#include <vector>
#include <iostream>
#include <fstream>

#include "Parser.h"
#include "Exporter.h"
#include "CmdLine.hpp"
#include "Colors.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Parsing command line ..." << std::endl;
    CmdLine cmdLine(argc, argv);
    if (!cmdLine.ok())
        return 1;
    if (!cmdLine.canContinue())
        return 0;
    std::cout << std::endl;

    Parser     *parser   = nullptr;
    Exporter   *exporter = nullptr;

    std::vector<Module> modules;

    std::cout << "Reading input file(s) ..." << std::endl;
    try {
        std::filesystem::path path = cmdLine.getPath();

        if (!std::filesystem::exists(path)) {
            std::cerr << FORMAT_ERROR "path \"" << path.string() << "\" does not exist.\n";
            return 3;
        }

        if (!std::filesystem::is_directory(path)) {
            std::cerr << FORMAT_ERROR "the specified path \"" << path.string() << "\" is not a directory.\n";
            return 3;
        }

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (!entry.is_regular_file()) continue;

            std::cout << FORMAT_INFO "processing file '" << entry.path().string() << "'\n";

            // GetParser determines the format and creates the required parser
            parser = GetParser(entry.path().string());
            if (!parser) {
                std::cerr << FORMAT_WARNING << "unsupported format: '" << entry.path().string()
                    << "', skipped\n";
                continue;
            }
            auto parsed_modules = parser->Parse(entry.path().string());
            if (parsed_modules.empty()) {
                std::cerr << FORMAT_WARNING << "no modules parsed in '" << entry.path().string() << "'\n";
            }
            else {
                std::cout << FORMAT_INFO << "found " << parsed_modules.size() << " module(s)\n";
                modules.insert(modules.end(), parsed_modules.begin(), parsed_modules.end());
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
    std::string theme_name = cmdLine.getThemeName();
    exporter = GetExporter(cmdLine.getOutputFormat());
    if (!exporter) {
        std::cerr << FORMAT_ERROR "unsupported output format\n";
        return 4;
	}
	int export_result = exporter->Export(modules, theme_name);
    if (export_result != 0) {
        std::cerr << FORMAT_ERROR "export failed with code " << export_result << "\n";
        return export_result;
	}
    if (cmdLine.getOutputFormat() == OutputFormat::asciidoc && cmdLine.PrintPortList()) {
        auto* adoc_exp = dynamic_cast<Exporter_ADOC*>(exporter);
        if (adoc_exp) {
            int pl_result = adoc_exp->ExportPortList(modules);
            if (pl_result != 0) {
                std::cerr << FORMAT_ERROR "port list export failed with code " << pl_result << "\n";
                return pl_result;
            }
            std::cout << FORMAT_INFO "Port list saved to 'portlist.adoc'\n";
        }
    }
    std::cout << "Everything has been done!" << std::endl;

    return 0;
}
