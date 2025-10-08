/*******************************************************************************
 *  @file   CmdLine.hpp
 *  @date   08.10.2025
 *  @brief  Файл содержит описание класса для разбора аргументов командной строки
 ******************************************************************************/

#pragma once

#include <string>

class CmdLine {
    bool        argsOk      = true,
                shouldStop  = false;
    std::string themeName   = "dark",
                path;
public:
    CmdLine(int _argc, char *_argv[]);
public:
    bool        ok()            { return argsOk; }
    bool        canContinue()   { return !shouldStop; }
    std::string getThemeName()  { return themeName; }
    std::string getPath()       { return path; }
private:
    bool        checkArgs();
    void        printHelpMessage();
};
