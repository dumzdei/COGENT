#pragma once
#include "DataTypes.h"
#include <fstream>
#include <sstream>
#include <iostream>

class Exporter
{
public:
    int Export_to_HTML(std::vector<FileInfo>& files_info, std::ofstream& html, std::ifstream& temp_html);
};