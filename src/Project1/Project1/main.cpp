#include "Parser.h"
#include "Exporter.h"
#include <filesystem>

int main()
{
    Parser parser;
    Exporter exporter;
	std::vector<FileInfo> files_info;

	std::cout << "Enter directory path: ";
    std::string directory_path;
    std::cin >> directory_path;

    std::ofstream html("report.htm");
	std::ifstream temp_html("templates/template_lite.htm");

    if (!html.is_open()) return 1;
        
    for (const auto& entry : std::filesystem::directory_iterator(directory_path))
    {
        if (!entry.is_regular_file()) continue;

        std::cout << "Processing file: " << entry.path() << "\n";

        if (parser.loadFile(entry.path().string()))
        {
            files_info.push_back(parser.parse());
        }
        else
        {
            std::cerr << "Failed to load file.\n";
        }
    }

    
    exporter.Export_to_HTML(files_info, html, temp_html);
	

    return 0;
}