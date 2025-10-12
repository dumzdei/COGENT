#include "Exporter.h"

int Exporter::Export_to_HTML(std::vector<FileInfo>& files_info, std::ofstream& html, std::ifstream& temp_html)
{    
    if (!html.is_open() || !temp_html.is_open())
    {
        std::cerr << "Error opening HTML file\n";
        return 1;
    }

    std::string line;

    while (std::getline(temp_html, line))
    {
        if (line.find("{{TOP_MODULE}}") != std::string::npos)
        {
            for (int i = 0; i < files_info.size(); i++)
            {
                for (int j = 0; j < files_info[i].comments.size(); j++)
                {
                    if (files_info[i].comments[j].tag == "@top")
                    {
                        html << "<h1>" << files_info[i].modules[j].name << "</h1>\n";
						html << "<h3>" << files_info[i].comments[j].comment_block[0] << "</h3>\n";
						break;
					}
                }
            }
        }
        else if (line.find("{{MODULES}}") != std::string::npos)
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

                    if (!module.params.empty())
                    {
                        html << "<p>Parameters</p>\n";
                        html << "<table class='auto'>\n";
                        html << "<tr><th>Parameter name</th><th>Value</th><th>Description</th></tr>\n";
                        for (const auto& param : module.params)
                        {
							html << "<tr><td>" << param.name << "</td>"
								<< "<td>" << param.value << "</td>"
                                << "<td class='param'>" << param.description << "</td></tr>\n";
                        }
                        html << "</table>\n";
                    }
                    else
                    {
                        html << "<p>No parameters</p>\n";
					}

                    if (!module.ports.empty())
                    {
                        html << "<p>Ports</p>\n";
                        html << "<table class='auto'>\n";
                        html << "<tr><th>Port name</th><th>Direction</th><th>Type</th><th>Width</th><th>Description</th></tr>\n";
                        for (const auto& port : module.ports)
                        {
                            html << "<tr><td>" << port.name << "</td>"
								<< "<td>" << port.direction << "</td>"
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
                    for (const auto& cmt : fileinfo.comments)
                    {
                        if (cmt.tag == "@author" && !cmt.comment_block.empty())
                        {
                            for (const auto& cmt_line : cmt.comment_block)
                                html << "<author>" << "Author: " << cmt_line << "</author>\n";
                        }
                    }
                    html << "</details>\n\n";
                }
            }
        }
        else 
        {
            html << line << "\n";
        }
    }

	return 0;
}