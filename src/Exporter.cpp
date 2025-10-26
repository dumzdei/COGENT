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
        else if (line.find("{{GROUPS}}") != std::string::npos)
        {
            bool first = true;
            for (const auto& module : modules)
            {
                std::string id_safe = module.name;
                std::replace(id_safe.begin(), id_safe.end(), ' ', '_');

                if (first) {
                    html << "<button onclick=\"showModule('" << id_safe << "')\" data-module=\"" << id_safe << "\" class=\"active\">" << module.name << "</button>\n";
                    first = false;
                }
                else {
                    html << "<button onclick=\"showModule('" << id_safe << "')\" data-module=\"" << id_safe << "\">" << module.name << "</button>\n";
                }
            }
        }
        else if (line.find("{{MODULES}}") != std::string::npos)
        {
            for (const auto& module : modules)
            {
                std::string id_safe = module.name;
                std::replace(id_safe.begin(), id_safe.end(), ' ', '_');
                html << "<div id='" << id_safe << "' class='module-content'>\n";

                html << "<h2>Module: " << module.name << "</h2>\n";

                html << "<div class='module-schema'>\n";
                html << Generate_SVG(module);
                html << "</div>\n";

                for (const auto& cmt : module.comments)
                {
                    if (cmt.tag == "@brief" && !cmt.comment_block.empty())
                    {
                        html << "<h3>Description</h3><ul>\n";
                        for (const auto& text : cmt.comment_block)
                            html << "<li>" << text << "</li>\n";
                        html << "</ul>\n";
                    }
                }

                if (!module.params.empty())
                {
                    html << "<h3>Parameters</h3>\n";
                    html << "<table>\n<tr><th>Name</th><th>Value</th><th>Description</th></tr>\n";
                    for (const auto& param : module.params)
                        html << "<tr><td>" << param.name << "</td><td>" << param.value << "</td><td>" << param.description << "</td></tr>\n";
                    html << "</table>\n";
                }

                if (!module.ports.empty())
                {
                    html << "<h3>Ports</h3>\n";
                    html << "<table>\n<tr><th>Name</th><th>Direction</th><th>Type</th><th>Width</th><th>Description</th></tr>\n";
                    for (const auto& port : module.ports)
                        html << "<tr><td>" << port.name << "</td><td>" << port.direction << "</td><td>" << port.type << "</td><td>" << port.width << "</td><td>" << port.description << "</td></tr>\n";
                    html << "</table>\n";
                }

                for (const auto& cmt : module.comments)
                {
                    if (cmt.tag == "@author" && !cmt.comment_block.empty())
                        html << "<p><b>Author:</b> " << cmt.comment_block[0] << "</p>\n";
                }

                html << "</div>\n";
            }
        }
        else
        {
            html << line << "\n";
        }
    }

    return 0;
}

std::string Exporter::Generate_SVG(const Module& module)
{
    std::stringstream svg;

    svg << "<svg width=\"500\" height=\"400\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    svg << "<style>\n";
    svg << "  .module-box { fill: #2d2d2d; stroke: #ffab40; stroke-width: 2; rx: 5; }\n";
    svg << "  .module-name { fill: #ffab40; font-family: Arial; font-size: 16; font-weight: bold; }\n";
    svg << "  .input-port { stroke: #4CAF50; stroke-width: 2; }\n";
    svg << "  .output-port { stroke: #FF5722; stroke-width: 2; }\n";
    svg << "  .inout-port { stroke: #2196F3; stroke-width: 2; }\n";
    svg << "  .port-text { font-family: Arial; font-size: 11; }\n";
    svg << "</style>\n";

    svg << "<rect class=\"module-box\" x=\"80\" y=\"60\" width=\"340\" height=\"280\"/>\n";

    svg << "<text class=\"module-name\" x=\"250\" y=\"90\" text-anchor=\"middle\">"
        << module.name << "</text>\n";

    int input_y = 140;
    int output_y = 140;
    int inout_y = 140;

    for (const auto& port : module.ports) {
        if (port.direction == "input") 
        {

            svg << "<line class=\"input-port\" x1=\"80\" y1=\"" << input_y << "\" x2=\"60\" y2=\"" << input_y << "\"/>\n";
            svg << "<circle cx=\"60\" cy=\"" << input_y << "\" r=\"3\" fill=\"#4CAF50\"/>\n";
            svg << "<text class=\"port-text\" fill=\"#4CAF50\" x=\"55\" y=\"" << input_y - 5 << "\" text-anchor=\"end\">"
                << port.name << "</text>\n";
            svg << "<text class=\"port-text\" fill=\"#888\" x=\"85\" y=\"" << input_y + 4 << "\">"
                << port.width << " " << port.type << "</text>\n";
            input_y += 30;
        }
        else if (port.direction == "output") 
        {

            svg << "<line class=\"output-port\" x1=\"420\" y1=\"" << output_y << "\" x2=\"440\" y2=\"" << output_y << "\"/>\n";
            svg << "<circle cx=\"440\" cy=\"" << output_y << "\" r=\"3\" fill=\"#FF5722\"/>\n";
            svg << "<text class=\"port-text\" fill=\"#FF5722\" x=\"445\" y=\"" << output_y - 5 << "\" text-anchor=\"start\">"
                << port.name << "</text>\n";
            svg << "<text class=\"port-text\" fill=\"#888\" x=\"415\" y=\"" << output_y + 4 << "\" text-anchor=\"end\">"
                << port.width << " " << port.type << "</text>\n";
            output_y += 30;
        }
        else 
        {

            svg << "<line class=\"inout-port\" x1=\"" << inout_y << "\" y1=\"340\" x2=\"" << inout_y << "\" y2=\"360\"/>\n";
            svg << "<circle cx=\"" << inout_y << "\" cy=\"360\" r=\"3\" fill=\"#2196F3\"/>\n";
            svg << "<text class=\"port-text\" fill=\"#2196F3\" x=\"" << inout_y << "\" y=\"375\" text-anchor=\"middle\">"
                << port.name << "</text>\n";
            inout_y += 40;
        }
    }

    svg << "</svg>\n";
    return svg.str();
}