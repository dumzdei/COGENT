#include "Exporter.h"

int Exporter_HTML::Export(std::vector<Module>& modules, std::string theme_name) {
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

                // PARAMETERS
                if (!module.params.empty()) {
                    std::vector<std::string> dynTags;
                    for (const auto& param : module.params) {
                        for (const auto& cmt : param.comments) {
                            if (!cmt.tag.empty() && std::find(dynTags.begin(), dynTags.end(), cmt.tag) == dynTags.end()) dynTags.push_back(cmt.tag);
                        }
                    }
                    html << "<h3>Parameters</h3>\n<table>\n<tr><th>Name</th>";
                    if (module.ShowParamType) html << "<th>Type</th>";
                    if (module.ShowParamDataType) html << "<th>Data Type</th>";
                    html << "<th>Default</th>";
                    for (const auto& tag : dynTags) {
                        if (tag == "@brief") html << "<th>Description</th>";
                        else if (tag == "@clock") html << "<th>Clock</th>";
                        else html << "<th>" << tag << "</th>";
                    }
                    html << "</tr>\n";
                    for (const auto& param : module.params) {
                        html << "<tr><td>" << param.name << "</td>";
                        if (module.ShowParamType) html << "<td>" << param.type << "</td>";
                        if (module.ShowParamDataType) html << "<td>" << param.data_type << "</td>";
                        html << "<td>" << param.value << "</td>";
                        for (const auto& tag : dynTags) {
                            std::string value = "-";
                            for (const auto& cmt : param.comments) if (cmt.tag == tag && !cmt.text.empty()) { value = cmt.text; break; }
                            html << "<td>" << value << "</td>";
                        }
                        html << "</tr>\n";
                    }
                    html << "</table>\n";
                }

                // PORTS (Dynamic)
                if (!module.ports.empty()) {
                    std::vector<std::string> dynTags;
                    for (const auto& port : module.ports) {
                        for (const auto& cmt : port.comments) {
                            if (!cmt.tag.empty() && std::find(dynTags.begin(), dynTags.end(), cmt.tag) == dynTags.end()) dynTags.push_back(cmt.tag);
                        }
                    }
                    html << "<h3>Ports</h3>\n<table>\n<tr><th>Name</th><th>Direction</th><th>Type</th><th>Width</th>";
                    for (const auto& tag : dynTags) {
                        if (tag == "@brief") html << "<th>Description</th>";
                        else if (tag == "@clock") html << "<th>Clock</th>";
                        else html << "<th>" << tag << "</th>";
                    }
                    html << "</tr>\n";
                    for (const auto& port : module.ports) {
                        html << "<tr><td>" << port.name << "</td><td>" << port.direction << "</td><td>" << port.type << "</td><td>" << port.width << "</td>";
                        for (const auto& tag : dynTags) {
                            std::string value = "-";
                            for (const auto& cmt : port.comments) if (cmt.tag == tag && !cmt.text.empty()) { value = cmt.text; break; }
                            html << "<td>" << value << "</td>";
                        }
                        html << "</tr>\n";
                    }
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