#pragma once
#include <string>
#include <vector>

struct Port
{
    std::string name;
    std::string type;
    std::string description;
    int width = 0;
};

struct Module
{
    std::string name;
    std::vector<Port> ports;
};

struct Comment_block
{
    std::vector<std::string> comment_block;
    std::string tag;
};

struct FileInfo
{
    std::vector<Module> modules;
    std::vector<Comment_block> comments;
};