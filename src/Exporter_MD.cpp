#include "Exporter.h"

int Exporter_MD::Export(std::vector<Module>& modules, std::string theme_name) {
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
                std::string description;
                for (const auto& cmt : param.comments) {
                    if ((cmt.tag == "@brief" || cmt.tag == "") && !cmt.text.empty()) {
                        description = cmt.text;
                        break;
                    }
                }
                md << "| `" << param.name << "` | ";
                if (module.ShowParamType)
                    md << param.type << " | ";
                if (module.ShowParamDataType)
                    md << param.data_type << " | ";
                md << "`" << param.value << "` | " << description << " |\n";
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
                std::string description;
                for (const auto& cmt : port.comments) {
                    if ((cmt.tag == "@brief" || cmt.tag == "") && !cmt.text.empty()) {
                        description = cmt.text;
                        break;
                    }
                }
                std::string dirIcon;
                if (port.direction == "input" || port.direction == "in")
                    dirIcon = "-> ";
                else if (port.direction == "output" || port.direction == "out")
                    dirIcon = "<- ";
                else
                    dirIcon = "<-> ";

                md << "| `" << port.name << "` | " << dirIcon << port.direction << " | "
                    << port.type << " | " << port.width << " | " << description << " |\n";
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