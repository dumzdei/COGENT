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

std::string Exporter::Print_CSS_for_SVG(std::string theme_name) {
    std::string theme = theme_name;
    std::transform(theme.begin(), theme.end(), theme.begin(), ::tolower);

    std::string bg_sec, acc_pri, acc_sec, acc_warn, txt_ter, bord_bri;

    if (theme.find("dark") != std::string::npos) {
        bg_sec = "#111822";
        acc_pri = "#00e5ff";
        acc_sec = "#0099ff";
        acc_warn = "#ffb300";
        txt_ter = "#4a5a72";
        bord_bri = "#2a3a4a";
    }
    else {
        bg_sec = "#ffffff";
        acc_pri = "#0066cc";
        acc_sec = "#0099ff";
        acc_warn = "#cc7a00";
        txt_ter = "#8a9aaa";
        bord_bri = "#bcc4cc";
    }

    return "<style>\n"
        ".module-box {\n"
        "    fill: " + bg_sec + ";\n"
        "    stroke: " + bord_bri + ";\n"
        "    stroke-width: 1.5;\n"
        "}\n"
        ".module-name {\n"
        "    fill: " + acc_pri + ";\n"
        "    font-family: inherit;\n"
        "    font-size: 14px;\n"
        "    font-weight: 500;\n"
        "    letter-spacing: 0.05em;\n"
        "}\n"
        ".input-port {\n"
        "    stroke: " + acc_sec + ";\n"
        "    stroke-width: 1.5;\n"
        "}\n"
        ".output-port {\n"
        "    stroke: " + acc_pri + ";\n"
        "    stroke-width: 1.5;\n"
        "}\n"
        ".inout-port {\n"
        "    stroke: " + acc_warn + ";\n"
        "    stroke-width: 1.5;\n"
        "}\n"
        ".port-text {\n"
        "    font-family: inherit;\n"
        "    font-size: 9px;\n"
        "    letter-spacing: 0.03em;\n"
        "}\n"
        ".input-port-text {\n"
        "    fill: " + acc_sec + ";\n"
        "}\n"
        ".output-port-text {\n"
        "    fill: " + acc_pri + ";\n"
        "}\n"
        ".inout-port-text {\n"
        "    fill: " + acc_warn + ";\n"
        "}\n"
        ".port-type-text {\n"
        "    fill: " + txt_ter + ";\n"
        "    font-size: 8px;\n"
        "}\n"
        ".port-circle {\n"
        "    stroke-width: 1;\n"
        "}\n"
        "</style>\n";
}

std::string Exporter::Generate_SVG(const Module& module, bool use_external_styles, std::string theme_name)
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

    std::string theme = theme_name;
    std::transform(theme.begin(), theme.end(), theme.begin(), ::tolower);
    std::string bg_sec, acc_pri, acc_sec, acc_warn, txt_ter, bord_bri;
    if (theme.find("dark") != std::string::npos) {
        bg_sec = "#111822"; acc_pri = "#00e5ff"; acc_sec = "#0099ff";
        acc_warn = "#ffb300"; txt_ter = "#4a5a72"; bord_bri = "#2a3a4a";
    }
    else {
        bg_sec = "#ffffff"; acc_pri = "#0066cc"; acc_sec = "#0099ff";
        acc_warn = "#cc7a00"; txt_ter = "#8a9aaa"; bord_bri = "#bcc4cc";
    }

    svg << "<svg width=\"" << svg_width << "\" height=\"" << svg_height
        << "\" viewBox=\"0 0 " << svg_width << " " << svg_height
        << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    // Если нужны внешние стили (HTML) -> вставляем <style>
    if (!use_external_styles) {
        svg << Print_CSS_for_SVG(theme_name);
    }
    // Иначе (Markdown/AsciiDoc) -> используем только инлайн-стили

    // Module box
    svg << "<rect x=\"" << module_x << "\" y=\"" << module_y
        << "\" width=\"" << module_width << "\" height=\"" << module_height
        << "\" style=\"fill:" << bg_sec << ";stroke:" << bord_bri << ";stroke-width:1.5\"/>\n";

    // Module name
    svg << "<text x=\"" << (module_x + module_width / 2) << "\" y=\"" << (module_y + 30)
        << "\" text-anchor=\"middle\" style=\"fill:" << acc_pri << ";font-family:inherit;font-size:14px;font-weight:500;letter-spacing:0.05em\">"
        << module.name << "</text>\n";

    int input_y = module_y + 50;
    int output_y = module_y + 50;
    int inout_start_x = module_x + (module_width - (inout_count - 1) * inout_spacing) / 2;
    int inout_y = module_y + module_height;

    for (const auto& port : module.ports) {
        if (port.direction == "input" || port.direction == "in") {
            svg << "<line x1=\"" << module_x << "\" y1=\"" << input_y
                << "\" x2=\"" << (module_x - 20) << "\" y2=\"" << input_y
                << "\" style=\"stroke:" << acc_sec << ";stroke-width:1.5\"/>\n";
            svg << "<circle cx=\"" << (module_x - 20) << "\" cy=\"" << input_y
                << "\" r=\"3\" style=\"fill:" << acc_sec << ";stroke:none\"/>\n";
            svg << "<text x=\"" << (module_x - 25) << "\" y=\"" << (input_y - 5)
                << "\" text-anchor=\"end\" style=\"fill:" << acc_sec << ";font-family:inherit;font-size:9px;letter-spacing:0.03em\">"
                << port.name << "</text>\n";
            svg << "<text x=\"" << (module_x + 5) << "\" y=\"" << (input_y + 4)
                << "\" style=\"fill:" << txt_ter << ";font-family:inherit;font-size:8px\">"
                << port.width << " " << port.type << "</text>\n";
            input_y += 30;
        }
        else if (port.direction == "output" || port.direction == "out") {
            svg << "<line x1=\"" << (module_x + module_width) << "\" y1=\"" << output_y
                << "\" x2=\"" << (module_x + module_width + 20) << "\" y2=\"" << output_y
                << "\" style=\"stroke:" << acc_pri << ";stroke-width:1.5\"/>\n";
            svg << "<circle cx=\"" << (module_x + module_width + 20) << "\" cy=\"" << output_y
                << "\" r=\"3\" style=\"fill:" << acc_pri << ";stroke:none\"/>\n";
            svg << "<text x=\"" << (module_x + module_width + 25) << "\" y=\"" << (output_y - 5)
                << "\" text-anchor=\"start\" style=\"fill:" << acc_pri << ";font-family:inherit;font-size:9px;letter-spacing:0.03em\">"
                << port.name << "</text>\n";
            svg << "<text x=\"" << (module_x + module_width - 5) << "\" y=\"" << (output_y + 4)
                << "\" text-anchor=\"end\" style=\"fill:" << txt_ter << ";font-family:inherit;font-size:8px\">"
                << port.width << " " << port.type << "</text>\n";
            output_y += 30;
        }
        else {
            svg << "<line x1=\"" << inout_start_x << "\" y1=\"" << inout_y
                << "\" x2=\"" << inout_start_x << "\" y2=\"" << (inout_y + 20)
                << "\" style=\"stroke:" << acc_warn << ";stroke-width:1.5\"/>\n";
            svg << "<circle cx=\"" << inout_start_x << "\" cy=\"" << (inout_y + 20)
                << "\" r=\"3\" style=\"fill:" << acc_warn << ";stroke:none\"/>\n";
            svg << "<text x=\"" << inout_start_x << "\" y=\"" << (inout_y + 35)
                << "\" text-anchor=\"middle\" style=\"fill:" << acc_warn << ";font-family:inherit;font-size:9px;letter-spacing:0.03em\">"
                << port.name << "</text>\n";
            svg << "<text x=\"" << inout_start_x << "\" y=\"" << (inout_y - 4)
                << "\" text-anchor=\"middle\" style=\"fill:" << txt_ter << ";font-family:inherit;font-size:8px\">"
                << port.width << " " << port.type << "</text>\n";
            inout_start_x += inout_spacing;
        }
    }

    svg << "</svg>\n";
    return svg.str();
}

std::vector<std::string> Exporter::CollectDynamicTags(const std::vector<Comment_block>& comments) {
    std::vector<std::string> tags;
    for (const auto& cmt : comments) {
        if (cmt.tag.empty() || cmt.tag == "@brief" || cmt.tag == " ") continue;
        if (std::find(tags.begin(), tags.end(), cmt.tag) == tags.end()) {
            tags.push_back(cmt.tag);
        }
    }
    return tags;
}

void FreeExporter(Exporter** exporter) {
    delete (*exporter);
    *exporter = nullptr;
}