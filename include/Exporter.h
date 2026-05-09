#pragma once
#include "DataTypes.h"
#include "Colors.hpp"
#include "CmdLine.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>


class Exporter {
protected:
	std::string FormatTextWithLineBreaks(const std::string& text, const std::string& line_break_marker,
		bool ADOC = false);
    std::string Print_CSS_for_SVG(std::string theme_name);
    std::string Generate_SVG(const Module& module, bool use_external_styles = true, std::string theme_name = "dark");
	std::vector<std::string> CollectDynamicTags(const std::vector<Comment_block>& comments);
public:
	virtual int Export(std::vector<Module>& modules, std::string theme_name) = 0;
	virtual ~Exporter() = default;
};

Exporter* GetExporter(OutputFormat format);
void      FreeExporter(Exporter** exporter);

class Exporter_HTML : public Exporter {
public:
	int Export(std::vector<Module>& modules, std::string theme_name) override;
};

class Exporter_MD : public Exporter {
public:
	int Export(std::vector<Module>& modules, std::string theme_name) override;
};

class Exporter_ADOC : public Exporter {
public:
	int Export(std::vector<Module>& modules, std::string theme_name) override;
	int ExportPortList(std::vector<Module>& modules);
};