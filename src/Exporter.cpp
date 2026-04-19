#include "Exporter.h"

#include <string>
#include <algorithm>
#include <cctype>  
#include "Colors.hpp"

std::string FormatTextWithLineBreaks(const std::string& text, const std::string& line_break_marker,
    bool ADOC = false) {
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
    bool top_found = false;
    while (std::getline(temp_html, line))
    {
        if (line.find("{{TOP_MODULE}}") != std::string::npos)
        {
            for (const auto& module : modules)
            {
                for (const auto& cmt : module.comments)
                {
                    if (cmt.tag == "@top" && !cmt.text.empty())
                    {
                        html << "<h1>" << module.name << "</h1>\n";
                        html << "<h3>" << cmt.text << "</h3>\n";
                        top_found = true;
                        break;
                    }
                }
                if (top_found) break;
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
                    html << "<button onclick=\"showModule('" << id_safe << "')\" data-module=\"" <<
                        id_safe << "\" class=\"active\">" << module.name << "</button>\n";
                    first = false;
                }
                else {
                    html << "<button onclick=\"showModule('" << id_safe << "')\" data-module=\"" <<
                        id_safe << "\">" << module.name << "</button>\n";
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
                    if (cmt.tag == "@status" && !cmt.text.empty())
                    {
                        html << "<div class=\"info-line status\"><span class=\"info-label\">Status</span><span \
                                 class=\"info-value\">" << FormatTextWithLineBreaks(cmt.text, "\\") << "</span></div>\n";
                    }
                    else if (cmt.tag == "@date" && !cmt.text.empty())
                    {
                        html << "<div class=\"info-line date\"><span class=\"info-label\">Date</span><span \
                                 class=\"info-value\">" << FormatTextWithLineBreaks(cmt.text, "\\") << "</span></div>\n";
                    }
                    else if (cmt.tag == "@author" && !cmt.text.empty())
                    {
                        html << "<div class=\"info-line author\"><span \
                                 class=\"info-label\">Author</span><span class=\"info-value\">"
                             << FormatTextWithLineBreaks(cmt.text, "\\") << "</span></div>\n";
                    }
                    else if (cmt.tag == "@brief" && !cmt.text.empty())
                    {
                        html << "<div class=\"info-block brief\">\n";
                        html << "<div class=\"info-block-header\">Description</div>\n";
                        html << "<div class=\"info-block-content\">" <<
                            FormatTextWithLineBreaks(cmt.text, "\\") << "</div>\n";
                        html << "</div>\n";
                    }
                    else if (cmt.tag == "@note" && !cmt.text.empty())
                    {
                        html << "<div class=\"info-block note\">\n";
                        html << "<div class=\"info-block-header\">Note</div>\n";
                        html << "<div class=\"info-block-content\">" <<
                            FormatTextWithLineBreaks(cmt.text, "\\") << "</div>\n";
                        html << "</div>\n";
                    }
                    else if (cmt.tag == "@warning" && !cmt.text.empty())
                    {
                        html << "<div class=\"info-block warning\">\n";
                        html << "<div class=\"info-block-header\">Warning</div>\n";
                        html << "<div class=\"info-block-content\">" <<
                            FormatTextWithLineBreaks(cmt.text, "\\") << "</div>\n";
                        html << "</div>\n";
                    }
                    else if (cmt.tag == "@error" && !cmt.text.empty())
                    {
                        html << "<div class=\"info-block error\">\n";
                        html << "<div class=\"info-block-header\">Error</div>\n";
                        html << "<div class=\"info-block-content\">" <<
                            FormatTextWithLineBreaks(cmt.text, "\\") << "</div>\n";
                        html << "</div>\n";
                    }
                    else if (cmt.tag == "@todo" && !cmt.text.empty())
                    {
                        html << "<div class=\"info-block todo\">\n";
                        html << "<div class=\"info-block-header\">Todo</div>\n";
                        html << "<div class=\"info-block-content\">" <<
                            FormatTextWithLineBreaks(cmt.text, "\\") << "</div>\n";
                        html << "</div>\n";
                    }
                    else if (cmt.tag == "@example" && !cmt.text.empty())
                    {
                        html << "<div class=\"info-block example\">\n";
                        html << "<div class=\"info-block-header\">Example</div>\n";
                        html << "<div class=\"info-block-content\"><pre><code>" <<
                            FormatTextWithLineBreaks(cmt.text, "\\") << "</code></pre></div>\n";
                        html << "</div>\n";
                    }
                }

                if (!module.params.empty())
                {
                    html << "<h3>Parameters</h3>\n";
                    html << "<table>\n<tr><th>Name</th>";
                    if (module.ShowParamType)
                        html << "<th>Type</th>";
                    if (module.ShowParamDataType)
                        html << "<th>Data Type</th>";
                    html << "<th>Value</th><th>Description</th></tr>\n";
                    for (const auto& param : module.params)
                    {
                        html << "<tr><td>" << param.name << "</td>";
                        if (module.ShowParamType)
                            html << "<td>" << param.type << "</td>";
                        if (module.ShowParamDataType)
                            html << "<td>" << param.data_type << "</td>";
                        html << "<td>" << param.value << "</td><td>" << param.description << "</td></tr>\n";
                    }
                    html << "</table>\n";
                }

                if (!module.ports.empty())
                {
                    html << "<h3>Ports</h3>\n";
                    html << "<table>\n<tr><th>Name</th><th>Direction</th><th>Type</th>\
                        <th>Width</th><th>Description</th></tr>\n";
                    for (const auto& port : module.ports)
                        html << "<tr><td>" << port.name << "</td><td>" << port.direction << "</td><td>" <<
                        port.type << "</td><td>" << port.width << "</td><td>" << port.description << "</td></tr>\n";
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

// coming soon
// coming soon
int Exporter::Export_to_MD(std::vector<Module>& modules) {
    std::ofstream md("README.md");

    if (!md.is_open())
    {
        std::cerr << FORMAT_ERROR "could not open README.md for writing.\n";
        return 2;
    }

    // Document header with table of contents
    md << "# Module Documentation\n\n";
    md << "## Table of Contents\n\n";

    for (const auto& module : modules)
    {
        std::string anchor = module.name;
        std::replace(anchor.begin(), anchor.end(), ' ', '-');
        std::transform(anchor.begin(), anchor.end(), anchor.begin(), ::tolower);

        // Check if this is the top module
        bool isTop = false;
        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@top" && !cmt.text.empty())
            {
                isTop = true;
                break;
            }
        }

        md << "- [" << module.name << "](#" << anchor << ")";
        if (isTop)
            md << " <sup><code>TOP</code></sup>";
        md << "\n";
    }
    md << "\n---\n\n";

    // Process all modules
    for (size_t idx = 0; idx < modules.size(); ++idx)
    {
        const auto& module = modules[idx];

        // Module header with anchor
        std::string anchor = module.name;
        std::replace(anchor.begin(), anchor.end(), ' ', '-');
        std::transform(anchor.begin(), anchor.end(), anchor.begin(), ::tolower);

        md << "<a name=\"" << anchor << "\"></a>\n";
        md << "# " << module.name;

        // Add TOP badge if this is the top module
        bool isTop = false;
        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@top" && !cmt.text.empty())
            {
                isTop = true;
                break;
            }
        }
        if (isTop)
            md << " <sup><code>TOP</code></sup>";
        md << "\n\n";

        md << "**File:** `" << module.filename << "`\n\n";
        md << "---\n\n";

        // SVG Diagram
        md << "## Module Diagram\n\n";
        md << "<div align=\"center\">\n\n";
        md << Generate_SVG(module, false);
        md << "\n\n</div>\n\n";
        md << "---\n\n";

        // Metadata table
        bool hasMetadata = false;
        std::stringstream metadataStream;

        metadataStream << "| Property | Value |\n";
        metadataStream << "|----------|-------|\n";

        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@status" && !cmt.text.empty())
            {
                std::string statusText = cmt.text;
                std::string Text = statusText;
                std::transform(statusText.begin(), statusText.end(), statusText.begin(), [](unsigned char c) {
                    return std::tolower(c);
                    });
                std::string statusBadge;
                if (statusText.find("complete") != std::string::npos ||
                    statusText.find("done") != std::string::npos ||
                    statusText.find("verified in simulation") != std::string::npos)
                    statusBadge = "**[COMPLETE]** ";
                else if (statusText.find("progress") != std::string::npos)
                    statusBadge = "**[IN PROGRESS]** ";
                else
                    statusBadge = "**[PENDING]** ";
                metadataStream << "| **Status** | " << statusBadge << Text << " |\n";
                hasMetadata = true;
            }
            if (cmt.tag == "@date" && !cmt.text.empty())
            {
                metadataStream << "| **Date** | " << FormatTextWithLineBreaks(cmt.text, "\\") << " |\n";
                hasMetadata = true;
            }
            if (cmt.tag == "@author" && !cmt.text.empty())
            {
                metadataStream << "| **Author** | " << FormatTextWithLineBreaks(cmt.text, "\\") << " |\n";
                hasMetadata = true;
            }
        }

        if (hasMetadata)
        {
            md << "## Module Information\n\n";
            md << metadataStream.str();
            md << "\n---\n\n";
        }

        // Description
        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@brief" && !cmt.text.empty())
            {
                md << "## Description\n\n";
                md << FormatTextWithLineBreaks(cmt.text, "\\") << "\n\n";
                md << "---\n\n";
            }
        }

        // Parameters table
        if (!module.params.empty())
        {
            md << "## Parameters\n\n";
            md << "| Name | ";
            if (module.ShowParamType)
                md << "Type | ";
            if (module.ShowParamDataType)
                md << "Data Type | ";
            md << "Default | Description |\n";

            md << "|------|";
            if (module.ShowParamType)
                md << "------|";
            if (module.ShowParamDataType)
                md << "-----------|";
            md << "---------|-------------|\n";

            for (const auto& param : module.params)
            {
                md << "| `" << param.name << "` | ";
                if (module.ShowParamType)
                    md << param.type << " | ";
                if (module.ShowParamDataType)
                    md << param.data_type << " | ";
                md << "`" << param.value << "` | " << param.description << " |\n";
            }
            md << "\n---\n\n";
        }

        // Ports table
        if (!module.ports.empty())
        {
            md << "## Ports\n\n";
            md << "| Name | Direction | Type | Width | Description |\n";
            md << "|------|-----------|------|-------|-------------|\n";

            for (const auto& port : module.ports)
            {
                std::string dirIcon;
                if (port.direction == "input" || port.direction == "in")
                    dirIcon = "-> ";
                else if (port.direction == "output" || port.direction == "out")
                    dirIcon = "<- ";
                else
                    dirIcon = "<-> ";

                md << "| `" << port.name << "` | " << dirIcon << port.direction << " | "
                    << port.type << " | " << port.width << " | " << port.description << " |\n";
            }
            md << "\n---\n\n";
        }

        // Additional sections
        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@note" && !cmt.text.empty())
            {
                md << "## Notes\n\n";
                md << "> **Note:** " << FormatTextWithLineBreaks(cmt.text, "\\") << "\n\n";
                md << "---\n\n";
            }
            if (cmt.tag == "@warning" && !cmt.text.empty())
            {
                md << "## Warnings\n\n";
                md << "> **Warning:** " << FormatTextWithLineBreaks(cmt.text, "\\") << "\n\n";
                md << "---\n\n";
            }
            if (cmt.tag == "@error" && !cmt.text.empty())
            {
                md << "## Errors\n\n";
                md << "> **Error:** " << FormatTextWithLineBreaks(cmt.text, "\\") << "\n\n";
                md << "---\n\n";
            }
            if (cmt.tag == "@todo" && !cmt.text.empty())
            {
                md << "## TODO\n\n";
                std::string todoText = FormatTextWithLineBreaks(cmt.text, "\\");
                std::stringstream ss(todoText);
                std::string token;
                while (std::getline(ss, token, '\n')) {
                    if (!token.empty())
                        md << "- [ ] " << token << "\n";
                }
                md << "\n---\n\n";
            }
            if (cmt.tag == "@example" && !cmt.text.empty())
            {
                md << "## Example\n\n";
                md << "```\n";
                md << cmt.text;
                md << "\n```\n\n";
                md << "---\n\n";
            }
        }

        // Add page break between modules (for PDF conversion)
        if (idx < modules.size() - 1)
            md << "\n<div style=\"page-break-after: always;\"></div>\n\n";
    }

    // Footer
    md << "\n---\n\n";
    md << "*Documentation generated automatically by COGENT*\n";

    return 0;
}
int Exporter::Export_to_ADOC(std::vector<Module>& modules) {
    std::ofstream adoc("report.adoc");

    if (!adoc.is_open())
    {
        std::cerr << FORMAT_ERROR "could not open report.adoc for writing.\n";
        return 2;
    }

    // AsciiDoc header with custom CSS
    adoc << "= Module Documentation\n";
    adoc << ":toc: left\n";
    adoc << ":toclevels: 3\n";
    adoc << ":toc-title: Table of Contents\n";
    adoc << ":sectnums:\n";
    adoc << ":icons: font\n";
    adoc << ":source-highlighter: rouge\n";
    adoc << ":sectanchors:\n";
    adoc << ":linkattrs:\n\n";

    adoc << ":stylesheet: " << "default.css\n";
    adoc << "\n";

    // Process all modules
    for (size_t idx = 0; idx < modules.size(); ++idx)
    {
        const auto& module = modules[idx];

        // Create anchor-friendly ID
        std::string anchor = module.name;
        std::replace(anchor.begin(), anchor.end(), ' ', '_');
        std::replace(anchor.begin(), anchor.end(), '(', '_');
        std::replace(anchor.begin(), anchor.end(), ')', '_');
        std::replace(anchor.begin(), anchor.end(), '[', '_');
        std::replace(anchor.begin(), anchor.end(), ']', '_');

        // Module header with anchor
        adoc << "[#" << anchor << "]\n";

        bool isTop = false;
        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@top" && !cmt.text.empty())
            {
                isTop = true;
                break;
            }
        }

        adoc << "== " << module.name;
        if (isTop)
            adoc << " TOP";
        adoc << "\n\n";

        // Module info sidebar
        bool hasMetadata = false;
        std::stringstream sidebarStream;

        sidebarStream << "*File:* `" << module.filename << "`\n\n";

        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@status" && !cmt.text.empty())
            {
                std::string statusText = cmt.text;
                std::string Text = statusText;
                std::transform(statusText.begin(), statusText.end(), statusText.begin(), [](unsigned char c) {
                    return std::tolower(c);
                    });
                sidebarStream << "*Status:* ";
                if (statusText.find("complete") != std::string::npos ||
                    statusText.find("done") != std::string::npos ||
                    statusText.find("verified in simulation") != std::string::npos)
                    sidebarStream << "[+] ";
                else if (statusText.find("progress") != std::string::npos)
                    sidebarStream << "[~] ";
                else
                    sidebarStream << "[-]#";
                sidebarStream << Text << "\n\n";
                hasMetadata = true;
            }
            if (cmt.tag == "@date" && !cmt.text.empty())
            {
                sidebarStream << "*Date:* " << FormatTextWithLineBreaks(cmt.text, "\\", true) << "\n\n";
                hasMetadata = true;
            }
            if (cmt.tag == "@author" && !cmt.text.empty())
            {
                sidebarStream << "*Author:* " << FormatTextWithLineBreaks(cmt.text, "\\", true) << "\n\n";
                hasMetadata = true;
            }
        }

        adoc << ".Module Information\n";
        adoc << "[sidebar]\n";
        adoc << "--\n";
        adoc << sidebarStream.str();
        adoc << "--\n\n";

        // Module Diagram
        adoc << "=== Module Diagram\n\n";
        adoc << "[.text-center]\n";
        adoc << "++++\n";
        adoc << Generate_SVG(module, false);
        adoc << "\n++++\n\n";

        // Parameters
        if (!module.params.empty())
        {
            adoc << "=== Parameters\n\n";
            adoc << "[options=\"header\",cols=\"2,";
            if (module.ShowParamType) adoc << "2,";
            if (module.ShowParamDataType) adoc << "2,";
            adoc << "2,5\"]\n";
            adoc << "|===\n";
            adoc << "|Name ";
            if (module.ShowParamType) adoc << "|Type ";
            if (module.ShowParamDataType) adoc << "|Data Type ";
            adoc << "|Default |Description\n\n";

            for (const auto& param : module.params)
            {
                adoc << "|`" << param.name << "` ";
                if (module.ShowParamType) adoc << "|" << param.type << " ";
                if (module.ShowParamDataType) adoc << "|" << param.data_type << " ";
                adoc << "|`" << param.value << "` |" << param.description << "\n";
            }
            adoc << "|===\n\n";
        }

        // Ports
        if (!module.ports.empty())
        {
            adoc << "=== Ports\n\n";
            adoc << "[options=\"header\",cols=\"3,2,2,2,5\"]\n";
            adoc << "|===\n";
            adoc << "|Name |Direction |Type |Width |Description\n\n";

            for (const auto& port : module.ports)
            {
                std::string dirAdoc;
                if (port.direction == "input" || port.direction == "in")
                    dirAdoc = "&larr; Input";  // ←
                else if (port.direction == "output" || port.direction == "out")
                    dirAdoc = "&rarr; Output"; // →
                else
                    dirAdoc = "&harr; Inout";  // ↔

                adoc << "|`" << port.name << "` |" << dirAdoc << " |"
                    << port.type << " |" << port.width << " |" << port.description << "\n";
            }
            adoc << "|===\n\n";
        }

        // Documentation sections
        for (const auto& cmt : module.comments)
        {
            if (cmt.tag == "@brief" && !cmt.text.empty())
            {
                adoc << "=== Description\n\n";
                adoc << FormatTextWithLineBreaks(cmt.text, "\\", true) << "\n\n";
            }
            else if (cmt.tag == "@note" && !cmt.text.empty())
            {
                adoc << "=== Notes\n\n";
                adoc << "NOTE: " << FormatTextWithLineBreaks(cmt.text, "\\", true) << "\n\n";
            }
            else if (cmt.tag == "@warning" && !cmt.text.empty())
            {
                adoc << "=== Warnings\n\n";
                adoc << "WARNING: " << FormatTextWithLineBreaks(cmt.text, "\\", true) << "\n\n";
            }
            else if (cmt.tag == "@error" && !cmt.text.empty())
            {
                adoc << "=== Errors\n\n";
                adoc << "CAUTION: " << FormatTextWithLineBreaks(cmt.text, "\\", true) << "\n\n";
            }
            else if (cmt.tag == "@todo" && !cmt.text.empty())
            {
                adoc << "=== TODO\n\n";
                std::string todoText = FormatTextWithLineBreaks(cmt.text, "\\", true);
                std::stringstream ss(todoText);
                std::string token;
                while (std::getline(ss, token, '\n')) {
                    if (!token.empty())
                        adoc << "* [ ] " << token << "\n";
                }
                adoc << "\n";
            }
            else if (cmt.tag == "@example" && !cmt.text.empty())
            {
                adoc << "=== Example\n\n";
                adoc << "[source,verilog]\n";
                adoc << "----\n";
                adoc << cmt.text;
                adoc << "\n----\n\n";
            }
        }

        // Page break between modules
        if (idx < modules.size() - 1)
            adoc << "<<<\n\n";
    }

    // Footer
    adoc << "\n'''\n\n";
    adoc << "Documentation generated automatically | _Last updated: {docdate}_\n";

    return 0;
}