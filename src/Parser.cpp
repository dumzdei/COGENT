#include "Parser.h"

#include <iostream>

#include "Colors.hpp"

Parser* GetParser(const std::string& fileName) {
    Parser *parser = nullptr;

    // Проверяем, не является ли файл файлом формата Verilog
    parser = new Parser_Verilog;
    if (parser->isMyFormat(fileName)) {
        std::cout << FORMAT_INFO "Verilog format detected for file '" << fileName << "'\n";
        return parser;
    }
    delete parser;
    parser = nullptr;

    // Проверяем, не является ли файл файлом формата VHDL
    /*
    parser = new Parser_VHDL;
    if (parser->isMyFormat(fileName)) {
        std::cout << FORMAT_INFO "VHDL format detected for file '" << fileName << "'\n";
        return parser;
    }
    delete parser;
    parser = nullptr;
    //*/

    return nullptr;
}

void FreeParser(Parser** parser) {
    delete (*parser);
    (*parser) = nullptr;
}
