#include "Exporter.h"
#include <set>

int Exporter_ADOC::Export(std::vector<Module>& modules, std::string theme_name) {
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
                std::string description;
                for (const auto& cmt : param.comments) {
                    if ((cmt.tag == "@brief" || cmt.tag == "") && !cmt.text.empty()) {
                        description = cmt.text;
                        break;
                    }
                }
                adoc << "|`" << param.name << "` ";
                if (module.ShowParamType) adoc << "|" << param.type << " ";
                if (module.ShowParamDataType) adoc << "|" << param.data_type << " ";
                adoc << "|`" << param.value << "` |" << description << "\n";
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
                std::string description;
                for (const auto& cmt : port.comments) {
                    if ((cmt.tag == "@brief" || cmt.tag == "") && !cmt.text.empty()) {
                        description = cmt.text;
                        break;
                    }
                }
                std::string dirAdoc;
                if (port.direction == "input" || port.direction == "in")
                    dirAdoc = "&larr; Input";  // ←
                else if (port.direction == "output" || port.direction == "out")
                    dirAdoc = "&rarr; Output"; // →
                else
                    dirAdoc = "&harr; Inout";  // ↔

                adoc << "|`" << port.name << "` |" << dirAdoc << " |"
                    << port.type << " |" << port.width << " |" << description << "\n";
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

int Exporter_ADOC::ExportPortList(std::vector<Module>& modules) {
    std::ofstream adoc("portlist.adoc");
    if (!adoc.is_open()) {
        std::cerr << FORMAT_ERROR "could not open portlist.adoc for writing.\n";
        return 2;
    }

    adoc << "= Port List\n\n";
    adoc << ":toc: left\n";
    adoc << ":icons: font\n\n";


    for (const auto& mod : modules) {
        std::vector<std::string> seen_tags;
        for (const auto& port : mod.ports) {
            for (const auto& cmt : port.comments) {
                std::string t = cmt.tag;
                if (!t.empty() && std::find(seen_tags.begin(), seen_tags.end(), t) == seen_tags.end()) {
                    seen_tags.push_back(t);
                }
            }
        }
        adoc << "== " << mod.name << " (`" << mod.filename << "`)\n\n";
        if (mod.ports.empty()) {
            adoc << "_No ports defined._\n\n";
            continue;
        }

        std::string header = "|Name |Width |Direction";
        for (const auto& col : seen_tags) {
			if (col == "@brief") header += " |Description";
			if (col == "@clock") header += " |Clock";
        }
        header += "\n\n";

        // Подсчёт колонок для cols="N*"
        int col_count = 3 + static_cast<int>(seen_tags.size());
        adoc << "[options=\"header\",cols=\"" << col_count << "*\"]\n|===\n";
        adoc << header;

        // Строки таблицы
        for (const auto& port : mod.ports) {
            // Фиксированные колонки
            adoc << "|`" << port.name << "` |" << port.width << " |";

            std::string dirAdoc;
            if (port.direction == "input" || port.direction == "in") dirAdoc = "&larr; Input";
            else if (port.direction == "output" || port.direction == "out") dirAdoc = "&rarr; Output";
            else dirAdoc = "&harr; Inout";
            adoc << dirAdoc;

            // Динамические колонки: ищем значение по тэгу
            for (const auto& tag_name : seen_tags) {
                std::string value = "-";
                for (const auto& cmt : port.comments) {
                    if (cmt.tag == tag_name && !cmt.text.empty()) {
                        value = cmt.text;
                        break;
                    }
                }
                adoc << " |" << value;
            }
            adoc << "\n";
        }
        adoc << "|===\n\n";
    }

    adoc << "\n---\n\n_Port list generated automatically by COGENT_\n";
    adoc << "\nhttps://github.com/dumzdei/COGENT\n";
    return 0;
}