/*******************************************************************************
 *  @file   CmdLine.hpp
 *  @date   08.10.2025
 *  @brief  Файл содержит описание класса для разбора аргументов командной строки
 ******************************************************************************/

#pragma once

#include <string>

enum class OutputFormat {
    html,
    markdown,
    asciidoc,
};

class CmdLine {
    bool            argsOk      = true,
                    shouldStop  = false;
    std::string     themeName   = "dark",
                    path;
    OutputFormat    format      = OutputFormat::html;
public:
    CmdLine(int _argc, char *_argv[]);
public:
    bool            ok()                { return argsOk; }
    bool            canContinue()       { return !shouldStop; }
    std::string     getThemeName()      { return themeName; }
    std::string     getPath()           { return path; }
    OutputFormat    getOutputFormat()   { return format; }
private:
    bool        checkArgs();
    void        printHelpMessage();
};
