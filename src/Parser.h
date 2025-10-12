#pragma once
#include "DataTypes.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

class Parser
{
private:
    std::vector<std::string> lines;
    std::string line;
    std::vector<std::string> commentVariants = { "//", "//*", "/**", "**/", "/*",  "*/"};

    std::vector<std::string> tags = 
    { "@brief" , "@todo" , "@description" , "@note" , "@warning" , "@error" , "@author" ,
        "@date" , "@example" , "@status" , "@defgroup" , "@ingroup" , "@top" };

public:
    bool loadFile(const std::string& filename);
    std::string trim(const std::string& source_line);
    std::string extractTag(std::string& text);
    std::vector<Port> PortParcer(const std::string& source_line);
	std::vector<Param> ParamParcer(const std::string& source_line);
    FileInfo parse();
};