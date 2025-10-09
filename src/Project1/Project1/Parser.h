#pragma once
#include "DataTypes.h"
#include <string>
#include <vector>
#include <fstream>

class Parser
{
private:
    std::vector<std::string> lines;
    std::string line;
    std::vector<std::string> commentVariants = { "//*", "/**", "//", "/*" };
public:
    bool loadFile(const std::string& filename);
    std::string trim(const std::string& str);
    std::vector<Port> PortParcer(const std::string& source_line);
	std::vector<Param> ParamParcer(const std::string& source_line);
    FileInfo parse();
};