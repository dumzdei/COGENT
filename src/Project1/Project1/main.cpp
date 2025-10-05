#include "Parser.h"
#include "Exporter.h"
#include <filesystem>

int main(int argc, char* argv[])
{
	char* path = nullptr;
	bool dark_theme = false;
	bool help = false;

	if (argc == 1)
	{
		std::cout << "No arguments. Use -h or --help for help." << std::endl;
		return 0;
	}

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--dark") == 0 || (strcmp(argv[i], "-d") == 0))
		{
			dark_theme = true;
		}
		else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
		{
			help = true;
		}
		else if ((strcmp(argv[i], "--file") == 0) || (strcmp(argv[i], "-f") == 0))
		{
			path = argv[++i];
		}
		else
		{
			std::cout << "\033[31m";
			std::cout << "Unknown option: " << argv[i] << std::endl;
			std::cout << "Use -h or --help.\n";
			std::cout << "\033[0m";
			return 1;
		}
	}

	if (help)
	{
		std::cout << "\033[35m";
		std::cout << R"(=============================================================================================================================================================================================================================
     ___   _       _        _______   ______     ______  __    __  .___  ___.  _______ .__   __. .___________.    ___   .___________.  ______   .______             ___     ___     ___     ___          _       _        ___
    /  //\| |/\ /\| |/\    |       \ /  __  \   /      ||  |  |  | |   \/   | |   ____||  \ |  | |           |   /   \  |           | /  __  \  |   _  \           / _ \   / _ \   / _ \   / _ \      /\| |/\ /\| |/\    /  /
   /  / \ ` ' / \ ` ' /    |  .--.  |  |  |  | |  ,----'|  |  |  | |  \  /  | |  |__   |   \|  | `---|  |----`  /  ^  \ `---|  |----`|  |  |  | |  |_)  |    _____| (_) | | | | | | | | | | | | |     \ ` ' / \ ` ' /   /  / 
  /  / |_     _|_     _|   |  |  |  |  |  |  | |  |     |  |  |  | |  |\/|  | |   __|  |  . `  |     |  |      /  /_\  \    |  |     |  |  |  | |      /    |______\__, | | | | | | | | | | | | |    |_     _|_     _| /  /  
 /  /   / , . \ / , . \    |  '--'  |  `--'  | |  `----.|  `--'  | |  |  |  | |  |____ |  |\   |     |  |     /  _____  \   |  |     |  `--'  | |  |\  \----.        / /  | |_| | | |_| | | |_| |     / , . \ / , . \ /  /   
/__/    \/|_|\/ \/|_|\/    |_______/ \______/   \______| \______/  |__|  |__| |_______||__| \__|     |__|    /__/     \__\  |__|      \______/  | _| `._____|       /_/    \___/   \___/   \___/      \/|_|\/ \/|_|\//__/    
=============================================================================================================================================================================================================================
)";
		std::cout << "\033[0m\n";
		std::cout << "Options:\n";
		std::cout << std::left << std::setw(32) << "  -f, --filepath <file>" << "input file\n";
		std::cout << std::left << std::setw(32) << "  -d, --dark" << "dark theme enable\n";
		std::cout << std::left << std::setw(32) << "  -h, --help" << "show this message\n";

		return 0;
	}

	if (!path) 
	{
		std::cout << "\033[31m";
		std::cout << "No input file!\n";
		std::cout << "Use -h or --help.\n";
		std::cout << "\033[0m";
		return 1;
	}

    Parser parser;
    Exporter exporter;
	std::vector<FileInfo> files_info;

	

    std::ofstream html("report.htm");

	std::string temp;
	if (dark_theme)
		temp = "templates/template_dark.htm";
	else
		temp = "templates/template_lite.htm";
	std::ifstream temp_html(temp);

	if (!html.is_open())
	{
		std::cout << "\033[31m";
		std::cerr << "Could not open report.htm for writing.\n";
		std::cout << "\033[0m";
		return 2;
	}
	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
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