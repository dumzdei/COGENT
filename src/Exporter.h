#pragma once
#include "DataTypes.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

class Exporter
{
public:
    int Export_to_HTML(std::vector<Module>& modules, std::string theme_name);
    int Export_to_MD(std::vector<Module>& modules);
    int Export_to_ADOC(std::vector<Module>& modules);
private:
    std::string Generate_SVG(const Module& module, bool use_external_styles = true);
};