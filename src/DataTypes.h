#pragma once
#include <string>
#include <vector>

struct Port
{
    std::string name;
    std::string direction;
    std::string type;
    std::string description;
    std::string width;
};

struct Param
{
    std::string name;
    std::string description;
    std::string value;
};

struct Module
{
    std::string name;
    std::vector<Port> ports;
	std::vector<Param> params;
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