#include "Exporter.h"
#include <string>
#include <algorithm>
#include <cctype>  
#include "Colors.hpp"


Exporter* GetExporter(OutputFormat format) {
    switch (format) {
    case OutputFormat::html:
        return new Exporter_HTML;
    case OutputFormat::markdown:
        return new Exporter_MD;
    case OutputFormat::asciidoc:
        return new Exporter_ADOC;
    default:
        return nullptr;
    }
}

std::string Exporter::FormatTextWithLineBreaks(const std::string& text, const std::string& line_break_marker,
    bool ADOC) {
    std::string result = text;
    size_t pos = 0;
    if (ADOC) {
        while ((pos = result.find(line_break_marker, pos)) != std::string::npos) {
            result.replace(pos, line_break_marker.length(), "+\n");
        }
    }
    else {
        while ((pos = result.find(line_break_marker, pos)) != std::string::npos) {
            result.replace(pos, line_break_marker.length(), "<br>");
            pos += 4; // <br>
        }
    }
    
    return result;
}

std::string Exporter::Print_CSS_for_SVG() {
    return "\
<style>\n\
.module-box {\n\
    fill: #ffffff;\n\
    stroke: #bcc4cc;\n\
    stroke-width: 1.5;\n\
}\n\
.module-name {\n\
    fill: #0066cc;\n\
    font-family: 'JetBrains Mono', 'Consolas', monospace;\n\
    font-size: 14;\n\
    font-weight: 500;\n\
    letter-spacing: 0.5;\n\
}\n\
.input-port {\n\
    stroke: #0099ff;\n\
    stroke-width: 1.5;\n\
}\n\
.output-port {\n\
    stroke: #0066cc;\n\
    stroke-width: 1.5;\n\
}\n\
.inout-port {\n\
    stroke: #cc7a00;\n\
    stroke-width: 1.5;\n\
}\n\
.port-text {\n\
    font-family: 'JetBrains Mono', 'Consolas', monospace;\n\
    font-size: 9;\n\
}\n\
.input-port-text {\n\
    fill: #0099ff;\n\
}\n\
.output-port-text {\n\
    fill: #0066cc;\n\
}\n\
.inout-port-text {\n\
    fill: #cc7a00;\n\
}\n\
.port-type-text {\n\
    fill: #8a9aaa;\n\
    font-size: 8;\n\
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

    int max_side_ports = std::max(input_count, output_count);
    int module_height = std::max(300, 60 + max_side_ports * 30);
    int module_width = 400;

    int inout_spacing = 80;
    if (inout_count > 0)
    {
        module_width = std::max(module_width, inout_spacing * inout_count);
    }

    int svg_height = module_height + 30;
    int svg_width = module_width * 2;

    int module_x = svg_width / 4;
    int module_y = 10;

    // Важно: указываем viewBox для правильного масштабирования
    svg << "<svg width=\"" << svg_width << "\" height=\"" << svg_height
        << "\" viewBox=\"0 0 " << svg_width << " " << svg_height
        << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";

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
                << "\" cy=\"" << input_y << "\" r=\"3\" fill=\"#0099ff\"/>\n";
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
                << "\" cy=\"" << output_y << "\" r=\"3\" fill=\"#0066cc\"/>\n";
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
                << "\" cy=\"" << inout_y + 20 << "\" r=\"3\" fill=\"#cc7a00\"/>\n";
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

void FreeExporter(Exporter** exporter) {
    delete (*exporter);
    *exporter = nullptr;
}