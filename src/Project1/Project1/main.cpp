#include "Parser.h"
#include "Exporter.h"
#include "CmdLine.hpp"
#include <filesystem>

int main(int argc, char* argv[])
{
	CmdLine cmdLine(argc, argv);
	if (!cmdLine.ok())
		return 1;
	if (!cmdLine.canContinue())
		return 0;

    Parser parser;
    Exporter exporter;
	std::vector<FileInfo> files_info;

    std::ofstream html("report.htm");

	std::ifstream temp_html("templates/template_" + cmdLine.getThemeName() + ".htm");

	if (!html.is_open())
	{
		std::cout << "\033[31m";
		std::cerr << "Could not open report.htm for writing.\n";
		std::cout << "\033[0m";
		return 2;
	}
	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(cmdLine.getPath()))
		{
			if (!entry.is_regular_file()) continue;

			std::cout << "Processing file: " << entry.path() << "\n";

			if (parser.loadFile(entry.path().string()))
			{
				files_info.push_back(parser.parse());
			}
			else
			{
				std::cout << "\033[33m";
				std::cerr << "Failed to load file.\n";
				std::cout << "\033[0m";
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		std::cout << "\033[31m";
		std::cerr << "Filesystem error: " << e.what() << "\n";
		std::cout << "\033[0m";
		return 3;
	}

    
    exporter.Export_to_HTML(files_info, html, temp_html);
	

    return 0;
}