#include "Exporter.h"

int Exporter::Export_to_HTML(std::vector<Module>& modules, std::ofstream& html, std::ifstream& temp_html)
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
            for (const auto& module : modules)
            {
                for (const auto& cmt : module.comments)
                {
                    if (cmt.tag == "@top" && !cmt.comment_block.empty())
                    {
                        html << "<h1>" << module.name << "</h1>\n";
                        html << "<h3>" << cmt.comment_block[0] << "</h3>\n";
                        break;
                    }
                }
            }
        }
        else if (line.find("{{MODULES}}") != std::string::npos)
        {
            html << "<div class='accordion'>\n";
            for (const auto& module : modules)
            {
                html << "<details>\n";
                html << "<summary>Module: " << module.name << "</summary>\n";

                // Комментарии @brief
                for (const auto& cmt : module.comments)
                {
                    if (cmt.tag == "@brief" && !cmt.comment_block.empty())
                    {
                        for (const auto& text : cmt.comment_block)
                            html << "<li class='brief'>" << text << "</li>\n";
                    }
                }

                // Параметры
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

                // Порты
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

                // Автор
                for (const auto& cmt : module.comments)
                {
                    if (cmt.tag == "@author" && !cmt.comment_block.empty())
                    {
                        for (const auto& text : cmt.comment_block)
                            html << "<author>Author: " << text << "</author>\n";
                    }
                }

                html << "</details>\n\n";
            }
            html << "</div>\n";
        }
        else
        {
            html << line << "\n";
        }
    }

    return 0;
}
