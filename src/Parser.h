#pragma once
#include "DataTypes.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

class Parser {
protected:
    std::vector<std::string> lines;
    std::vector<std::string> tags =
    { "@brief" , "@todo" , "@description" , "@note" , "@warning" , "@error" , "@author" ,
        "@date" , "@example" , "@status" , "@top" };

    std::string Trim(const std::string& source_line);
    std::string CleanToken(std::string& str);
    std::string ExtractTag(std::string& text);
    std::vector<Comment_block> Parse_CommentLine(const std::string& comment_text);
    std::vector<Comment_block> Parse_CommentLine(const std::vector<std::string>& comment_lines);
public:
    bool LoadFile(const std::string& fileName);
    virtual bool IsMyFormat(const std::string& fileName) = 0;
    virtual std::vector<Module> Parse(const std::string& fileName) = 0;
};

Parser* GetParser(const std::string& fileName);
void FreeParser(Parser **parser);

class Parser_Verilog : public Parser 
{
private:
    std::vector<Port> ParsePort(const std::string& source_line);
    std::vector<Param> ParseParam(const std::string& source_line);
public:
    bool IsMyFormat(const std::string& filename) override final;
    std::vector<Module> Parse(const std::string& source_file) override final;
};

class Parser_VHDL : public Parser
{
private:
    std::vector<Port> ParsePort(const std::string& source_line);
    std::vector<Param> ParseGenerics(const std::string& source_line);
public:
    bool IsMyFormat(const std::string& filename) override final;
    std::vector<Module> Parse(const std::string& source_file) override final;
};