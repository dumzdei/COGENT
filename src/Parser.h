#pragma once
#include "DataTypes.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

class Parser {
public:
    virtual bool isMyFormat(const std::string& fileName) = 0;
    virtual bool loadFile(const std::string& fileName) = 0;
    virtual std::vector<Module> parse(const std::string& fileName) = 0;
};

Parser* GetParser(const std::string& fileName);
void FreeParser(Parser **parser);

class Parser_Verilog : public Parser 
{
private:
    std::vector<std::string> lines;
    std::string line;
    std::vector<std::string> commentVariants = { "//", "//*", "/**", "**/", "/*",  "*/"};

    std::vector<std::string> tags = 
    { "@brief" , "@todo" , "@description" , "@note" , "@warning" , "@error" , "@author" ,
        "@date" , "@example" , "@status" , "@top" };

public:
    bool isMyFormat(const std::string& filename) override final;
    bool loadFile(const std::string& filename) override final;
    std::vector<Module> parse(const std::string& source_file) override final;
    std::string trim(const std::string& source_line);
    std::string extractTag(std::string& text);
    std::vector<Port> PortParcer(const std::string& source_line);
	std::vector<Param> ParamParcer(const std::string& source_line);
};