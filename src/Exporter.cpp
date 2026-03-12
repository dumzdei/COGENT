#include "Exporter.h"

#include <string>

#include "Colors.hpp"
/*

ДОБАВИТЬ ОБРАБОТКУ ТИПОВ В ПАРАМТЕРАХ В VHDL и SystemVerilog

*/
int Exporter::Export_to_HTML(std::vector<Module>& modules, std::string theme_name)
{
    std::ofstream html("report.htm");
    std::ifstream temp_html("templates/template_" + theme_name + ".htm");

    if (!html.is_open())
    {
        std::cerr << FORMAT_ERROR "could not open report.htm for writing.\n";
        return 2;
    }

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
                    if (cmt.tag == "@top" && !cmt.lines.empty())
                    {
                        html << "<h1>" << module.name << "</h1>\n";
                        html << "<h3>" << cmt.lines[0] << "</h3>\n";
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
                std::string id_safe = module.id;
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
                std::string id_safe = module.id;
                std::replace(id_safe.begin(), id_safe.end(), ' ', '_');
                html << "<div id='" << id_safe << "' class='module-content'>\n";

                html << "<h2>Module: " << module.name << "</h2>\n";
                html << "<h3>File: " << module.filename << "</h3>\n";

                html << "<div class='module-schema'>\n";
                html << Generate_SVG(module);
                html << "</div>\n";

                for (const auto& cmt : module.comments)
                {
                    if (cmt.tag == "@status" && !cmt.lines.empty())
                    {
                        html << "<p><b>Status:</b> " << cmt.lines[0] << "</p>\n";
                    }
                    else if (cmt.tag == "@date" && !cmt.lines.empty())
                    {
                        html << "<p><b>Date:</b> " << cmt.lines[0] << "</p>\n";
                    }
                    else if (cmt.tag == "@brief" && !cmt.lines.empty())
                    {
                        html << "<h3>Description</h3><ul>\n";
                        for (const auto& text : cmt.lines)
                            html << "<li>" << text << "</li>\n";
                        html << "</ul>\n";
                    }
                    else if (cmt.tag == "@note" && !cmt.lines.empty())
                    {
                        html << "<h3>Note</h3><div class=\"note\">\n";
                        for (const auto& text : cmt.lines)
                            html << "<p>" << text << "</p>\n";
                        html << "</div>\n";
                    }
                    else if (cmt.tag == "@warning" && !cmt.lines.empty())
                    {
                        html << "<h3>Warning</h3><div class=\"warning\">\n";
                        for (const auto& text : cmt.lines)
                            html << "<p>" << text << "</p>\n";
                        html << "</div>\n";
                    }
                    else if (cmt.tag == "@error" && !cmt.lines.empty())
                    {
                        html << "<h3>Error</h3><div class=\"error\">\n";
                        for (const auto& text : cmt.lines)
                            html << "<p>" << text << "</p>\n";
                        html << "</div>\n";
                    }
                    else if (cmt.tag == "@todo" && !cmt.lines.empty())
                    {
                        html << "<h3>Todo</h3><ul>\n";
                        for (const auto& text : cmt.lines)
                            html << "<li>" << text << "</li>\n";
                        html << "</ul>\n";
                    }
                    else if (cmt.tag == "@example" && !cmt.lines.empty())
                    {
                        html << "<h3>Example</h3><ul>\n";
                        for (const auto& text : cmt.lines)
                            html << "<li>" << text << "</li>\n";
                        html << "</ul>\n";
                    }
                    else if (cmt.tag == "@author" && !cmt.lines.empty())
                        html << "<p><b>Author:</b> " << cmt.lines[0] << "</p>\n";
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

int Exporter::Export_to_MD(std::vector<Module>& modules) {
    std::ofstream md("report.md");

    if (!md.is_open())
    {
        std::cerr << FORMAT_ERROR "could not open report.md for writing.\n";
        return 2;
    }

    bool inTopModule = false;
    const Module *top = nullptr;
    
    for (const auto& module : modules)
    {
        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@top" && !cmt.lines.empty())
            {
                top = &module;
                break;
            }
        }
        if (top)
            break;
    }

    if (!top)
        if (modules.size() == 1)
            top = &modules[0];
        else {
            std::cerr << FORMAT_ERROR "didn't find the top module.\n";
            return 3;
        }

    md << "<h2>Module: " << top->name << "</h2>\n";
    md << "<h3>File: " << top->filename << "</h3>\n";

    md << "\n";
    md << Generate_SVG(*top, false);
    md << "\n\n";


    for (const auto& cmt : top->comments)
    {
        if (cmt.tag == "@top" && !cmt.lines.empty())
        {
            //md << "#" << top->name << "\n\n";
            //md << cmt.lines[0] << "\n\n";
            continue;
        }
        if (cmt.tag == "@status" && !cmt.lines.empty())
        {
            md << "### Status: " << cmt.lines[0] << "\n\n";
            continue;
        }
        if (cmt.tag == "@date" && !cmt.lines.empty())
        {
            md << "### Date: " << cmt.lines[0] << "\n";
            continue;
        }
        if (cmt.tag == "@brief" && !cmt.lines.empty())
        {
            md << "### Description\n\n";
            for (const auto& text : cmt.lines)
                if (!text.empty())
                    md << text << "<br/>\n";
            md << "\n\n";
            continue;
        }
        if (cmt.tag == "@note" && !cmt.lines.empty())
        {
            md << "### Note\n\n";
            for (const auto& text : cmt.lines)
                md << text << "<br/>\n";
            md << "\n";
            continue;
        }
        if (cmt.tag == "@warning" && !cmt.lines.empty())
        {
            md << "### Warning\n\n";
            for (const auto& text : cmt.lines)
                md << text << "<br/>\n";
            md << "\n";
            continue;
        }
        if (cmt.tag == "@error" && !cmt.lines.empty())
        {
            md << "### Error\n\n";
            for (const auto& text : cmt.lines)
                md << text << "<br/>\n";
            md << "\n";
            continue;
        }
        if (cmt.tag == "@todo" && !cmt.lines.empty())
        {
            md << "### TODO\n\n";
            for (const auto& text : cmt.lines)
                md << "- " << text << "\n";
            md << "\n\n";
            continue;
        }
        if (cmt.tag == "@example" && !cmt.lines.empty())
        {
            md << "### Example\n\n";
            md << "```v";
            for (const auto& text : cmt.lines)
                md << text << "\n";
            md << "```\n\n";
            continue;
        }
        if (cmt.tag == "@author" && !cmt.lines.empty())
            md << "### Author: \n\n" << cmt.lines[0] << "\n\n";
    }

    return 0;
}

int Exporter::Export_to_ADOC(std::vector<Module>& modules) {
    std::ofstream adoc("report.adoc");

    if (!adoc.is_open())
    {
        std::cerr << FORMAT_ERROR "could not open report.adoc for writing.\n";
        return 2;
    }

    bool inTopModule = false;
    const Module *top = nullptr;

    for (const auto& module : modules)
    {
        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@top" && !cmt.lines.empty())
            {
                top = &module;
                break;
            }
        }
        if (top)
            break;
    }

    if (!top)
        if (modules.size() == 1)
            top = &modules[0];
        else {
            std::cerr << FORMAT_ERROR "didn't find the top module.\n";
            return 3;
        }

    adoc << "== Module: " << top->name << "\n\n";
    adoc << "== File: " << top->filename << "\n\n";

    adoc << "\n++++\n";
    adoc << Generate_SVG(*top, false);
    adoc << "\n++++\n\n";


    for (const auto& cmt : top->comments)
    {
        if (cmt.tag == "@top" && !cmt.lines.empty())
        {
            //adoc << "#" << top->name << "\n\n";
            //adoc << cmt.lines[0] << "\n\n";
            continue;
        }
        if (cmt.tag == "@status" && !cmt.lines.empty())
        {
            adoc << "=== Status: " << cmt.lines[0] << "\n\n";
            continue;
        }
        if (cmt.tag == "@date" && !cmt.lines.empty())
        {
            adoc << "=== Date: " << cmt.lines[0] << "\n";
            continue;
        }
        if (cmt.tag == "@brief" && !cmt.lines.empty())
        {
            adoc << "=== Description\n\n";
            for (const auto& text : cmt.lines)
                if (!text.empty())
                    adoc << text << "<br/>\n";
            adoc << "\n\n";
            continue;
        }
        if (cmt.tag == "@note" && !cmt.lines.empty())
        {
            adoc << "=== Note\n\n";
            for (const auto& text : cmt.lines)
                adoc << text << "<br/>\n";
            adoc << "\n";
            continue;
        }
        if (cmt.tag == "@warning" && !cmt.lines.empty())
        {
            adoc << "=== Warning\n\n";
            for (const auto& text : cmt.lines)
                adoc << text << "<br/>\n";
            adoc << "\n";
            continue;
        }
        if (cmt.tag == "@error" && !cmt.lines.empty())
        {
            adoc << "=== Error\n\n";
            for (const auto& text : cmt.lines)
                adoc << text << "<br/>\n";
            adoc << "\n";
            continue;
        }
        if (cmt.tag == "@todo" && !cmt.lines.empty())
        {
            adoc << "=== TODO\n\n";
            for (const auto& text : cmt.lines)
                adoc << "* " << text << "\n";
            adoc << "\n\n";
            continue;
        }
        if (cmt.tag == "@example" && !cmt.lines.empty())
        {
            adoc << "=== Example\n\n";
            adoc << "[source,verilog]\n";
            adoc << "....";
            for (const auto& text : cmt.lines)
                adoc << text << "\n";
            adoc << "....\n\n";
            continue;
        }
        if (cmt.tag == "@author" && !cmt.lines.empty())
            adoc << "=== Author: \n\n" << cmt.lines[0] << "\n\n";
    }

    return 0;
}

std::string Exporter::Print_CSS_for_SVG() {
    return "\
<style>\n\
.module-box {\n\
    fill: #ffffff;\n\
    stroke: #1976d2;\n\
    stroke-width: 2;\n\
    rx : 5;\n\
}\n\
.module-name {\n\
    fill: #1976d2;\n\
    font-family: 'Segoe UI', Arial, sans-serif;\n\
    font-size: 16;\n\
    font-weight: bold;\n\
}\n\
.input-port {\n\
    stroke: #2E7D32;\n\
    stroke-width: 2;\n\
}\n\
.output-port {\n\
    stroke: #D32F2F;\n\
    stroke-width: 2;\n\
}\n\
.inout-port {\n\
    stroke: #1565C0;\n\
    stroke-width: 2;\n\
}\n\
.port-text {\n\
    font-family: 'Segoe UI', Arial, sans-serif;\n\
    font-size: 11;\n\
}\n\
.input-port-text {\n\
    fill: #2E7D32;\n\
}\n\
.output-port-text {\n\
    fill: #D32F2F;\n\
}\n\
.inout-port-text {\n\
    fill: #1565C0;\n\
}\n\
.port-type-text {\n\
    fill: #666666;\n\
    font-size: 10;\n\
}\n\
.port-circle {\n\
    stroke-width: 1;\n\
}\n\
</style>\n";
}

std::string Exporter::Generate_SVG(const Module& module, bool use_external_styles)
{
    std::stringstream svg;

    int input_count = 0;
    int output_count = 0;
    int inout_count = 0;

    for (const auto& port : module.ports)
    {
        if (port.direction == "input" || port.direction == "in") input_count++;
        else if (port.direction == "output" || port.direction == "out") output_count++;
        else inout_count++;
    }

    // Расчет высоты блока на основе максимального количества портов
    int max_side_ports = std::max(input_count, output_count);
    int module_height = std::max(300, 60 + max_side_ports * 30);
    int module_width = 400;

    int inout_spacing = 80;
    if (inout_count > 0)
    {
        module_width = inout_spacing * inout_count;
    }

    int svg_height = module_height + 30;
    int svg_width = module_width*2;

    // Позиционирование блока
    int module_x = svg_width/4;
    int module_y = 10;

    svg << "<svg width=\"" << svg_width << "\" height=\"" << svg_height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    if (!use_external_styles) 
        svg << Print_CSS_for_SVG();

    svg << "<rect class=\"module-box\" x=\"" << module_x << "\" y=\"" << module_y
        << "\" width=\"" << module_width << "\" height=\"" << module_height << "\"/>\n";

    svg << "<text class=\"module-name\" x=\"" << (module_x + module_width / 2)
        << "\" y=\"" << (module_y + 30) << "\" text-anchor=\"middle\">"
        << module.name << "</text>\n";

    int input_y = module_y + 50;
    int output_y = module_y + 50;

    
    int inout_start_x = module_x + (module_width - (inout_count - 1) * inout_spacing) / 2;
    int inout_y = module_y + module_height;

    for (const auto& port : module.ports)
    {
        if (port.direction == "input" || port.direction == "in")
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
        else if (port.direction == "output" || port.direction == "out")
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