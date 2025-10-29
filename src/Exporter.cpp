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

    int input_count = 0;
    int output_count = 0;
    int inout_count = 0;

    for (const auto& port : module.ports)
    {
        if (port.direction == "input") input_count++;
        else if (port.direction == "output") output_count++;
        else inout_count++;
    }

    // –асчет высоты блока на основе максимального количества портов
    int max_side_ports = std::max(input_count, output_count);
    int module_height = std::max(300, 120 + max_side_ports * 30);
    int module_width = 340;

    int inout_spacing = 80;
    if (inout_count > 0)
    {
        module_width = inout_spacing * inout_count;
    }

    int svg_height = module_height + 150;
    int svg_width = module_width*2;
    

    // ѕозиционирование блока
    int module_x = svg_width/4;
    int module_y = 60;

    svg << "<svg width=\"" << svg_width << "\" height=\"" << svg_height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    svg << "<rect class=\"module-box\" x=\"" << module_x << "\" y=\"" << module_y
        << "\" width=\"" << module_width << "\" height=\"" << module_height << "\"/>\n";

    svg << "<text class=\"module-name\" x=\"" << (module_x + module_width / 2)
        << "\" y=\"" << (module_y + 30) << "\" text-anchor=\"middle\">"
        << module.name << "</text>\n";

    int input_y = module_y + 70;
    int output_y = module_y + 70;

    
    int inout_start_x = module_x + (module_width - (inout_count - 1) * inout_spacing) / 2;
    int inout_y = module_y + module_height;

    for (const auto& port : module.ports)
    {
        if (port.direction == "input")
        {
            svg << "<line class=\"input-port\" x1=\"" << module_x << "\" y1=\"" << input_y
                << "\" x2=\"" << (module_x - 20) << "\" y2=\"" << input_y << "\"/>\n";
            svg << "<circle class=\"port-circle input-port\" cx=\"" << (module_x - 20)
                << "\" cy=\"" << input_y << "\" r=\"3\" fill=\"#4CAF50\"/>\n";
            svg << "<text class=\"port-text input-port-text\" x=\"" << (module_x - 25)
                << "\" y=\"" << (input_y - 5) << "\" text-anchor=\"end\">"
                << port.name << "</text>\n";
            svg << "<text class=\"port-text port-type-text\" x=\"" << (module_x + 5)
                << "\" y=\"" << (input_y + 4) << "\">"
                << port.width << " " << port.type << "</text>\n";
            input_y += 30;
        }
        else if (port.direction == "output")
        {
            svg << "<line class=\"output-port\" x1=\"" << (module_x + module_width) << "\" y1=\"" << output_y
                << "\" x2=\"" << (module_x + module_width + 20) << "\" y2=\"" << output_y << "\"/>\n";
            svg << "<circle class=\"port-circle output-port\" cx=\"" << (module_x + module_width + 20)
                << "\" cy=\"" << output_y << "\" r=\"3\" fill=\"#FF5722\"/>\n";
            svg << "<text class=\"port-text output-port-text\" x=\"" << (module_x + module_width + 25)
                << "\" y=\"" << (output_y - 5) << "\" text-anchor=\"start\">"
                << port.name << "</text>\n";
            svg << "<text class=\"port-text port-type-text\" x=\"" << (module_x + module_width - 5)
                << "\" y=\"" << (output_y + 4) << "\" text-anchor=\"end\">"
                << port.width << " " << port.type << "</text>\n";
            output_y += 30;
        }
        else
        {
            svg << "<line class=\"inout-port\" x1=\"" << inout_start_x << "\" y1=\"" << inout_y
                << "\" x2=\"" << inout_start_x << "\" y2=\"" << (inout_y + 20) << "\"/>\n";
            svg << "<circle class=\"port-circle inout-port\" cx=\"" << inout_start_x
                << "\" cy=\"" << inout_y + 20 << "\" r=\"3\" fill=\"#2196F3\"/>\n";
            svg << "<text class=\"port-text inout-port-text\" x=\"" << inout_start_x
                << "\" y=\"" << (inout_y + 35) << "\" text-anchor=\"middle\">"
                << port.name << "</text>\n";
            svg << "<text class=\"port-text port-type-text\" x=\"" << inout_start_x
                << "\" y=\"" << (inout_y - 4) << "\" text-anchor=\"middle\">"
                << port.width << " " << port.type << "</text>\n";
            inout_start_x += inout_spacing;
        }
    }

    svg << "</svg>\n";
    return svg.str();
}