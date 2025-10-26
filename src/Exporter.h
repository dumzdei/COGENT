#pragma once
#include "DataTypes.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

class Exporter
{
public:
    int Export_to_HTML(std::vector<Module>& modules, std::ofstream& html, std::ifstream& temp_html);
private:
    std::string Generate_SVG(const Module& module);
};