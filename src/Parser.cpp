#include "Parser.h"

#include <iostream>

#include "Colors.hpp"

Parser* GetParser(const std::string& fileName) {
    Parser *parser = nullptr;

    // Проверяем, не является ли файл файлом формата Verilog
    parser = new Parser_Verilog;
    if (parser->IsMyFormat(fileName)) {
        std::cout << FORMAT_INFO "Verilog format detected for file '" << fileName << "'\n";
        return parser;
    }
    delete parser;
    parser = nullptr;

    // Проверяем, не является ли файл файлом формата VHDL
    parser = new Parser_VHDL;
    if (parser->IsMyFormat(fileName)) {
        std::cout << FORMAT_INFO "VHDL format detected for file '" << fileName << "'\n";
        return parser;
    }
    delete parser;
    parser = nullptr;

    return nullptr;
}

bool Parser::LoadFile(const std::string& filename)
{
    std::string line;
    lines.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    return true;
}

inline std::string Parser::Trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";

    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

std::string Parser::ExtractTag(std::string& text)
{
    for (const auto& tag : tags)
    {
        size_t pos = text.find(tag);
        if (pos != std::string::npos)
        {
            text.erase(pos, tag.length());
            return tag;
        }
    }
    return "";
}

void FreeParser(Parser** parser) {
    delete (*parser);
    (*parser) = nullptr;
}
