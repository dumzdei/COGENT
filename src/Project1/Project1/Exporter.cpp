#include "Exporter.h"

int Exporter::Export_to_HTML(std::vector<FileInfo>& files_info, std::ofstream& html, std::ifstream& temp_html)
{    
    if (!html.is_open() || !temp_html.is_open())
    {
        std::cerr << "Error opening HTML file\n";
        return 1;
    }

    std::string line;
    int idx = 0;

    while (std::getline(temp_html, line))
    {
        if (line.find("{{MODULES}}") != std::string::npos)
        {
            html << "<div class='accordion'>\n";
            for (const auto& fileinfo : files_info)
            {
                for (const auto& module : fileinfo.modules)
                {
                    html << "<details>\n";
                    html << "<summary>Module: " << module.name << "</summary>\n";

                    for (const auto& cmt : fileinfo.comments)
                    {
                        if (cmt.tag == "@brief" && !cmt.comment_block.empty())
                        {
                            for (const auto& cmt_line : cmt.comment_block)
                                html << "<li class='brief'>" << cmt_line << "</li>\n";
                        }
                    }

                    if (!module.ports.empty())
                    {
                        html << "<table class='auto'>\n";
                        html << "<tr><th>Port name</th><th>Type</th><th>Width</th><th>Description</th></tr>\n";
                        for (const auto& port : module.ports)
                        {
                            html << "<tr><td>" << port.name << "</td>"
                                << "<td>" << port.type << "</td>"
                                << "<td>" << port.width << "</td>"
                                << "<td class='param'>" << port.description << "</td></tr>\n";
                        }
                        html << "</table>\n";
                    }
                    else
                    {
                        html << "<p>No ports</p>\n";
                    }
                    html << "</details>\n\n";
                }
            }
        }
        else 
        {
            html << line << "\n"; // обычная строка без изменений
        }
    }

	return 0;
}