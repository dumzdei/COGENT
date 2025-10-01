#include "Parser.h"
#include <sstream>
#include <regex>
#include <filesystem>

bool Parser::loadFile(const std::string& filename)
{
    lines.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line))
    {
        lines.push_back(line);
    }
    return true;
}

std::string Parser::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

std::vector<Port> Parser::PortParcer(const std::string& source_line)
{
    std::vector<Port> ports;

    std::string line = source_line;
    line.erase(0, line.find_first_not_of(" \t"));

    std::regex baseRegex(R"((input wire|output wire|output reg)\s*(\[[0-9]+:[0-9]+\])?)");
    std::smatch match;

    if (std::regex_search(line, match, baseRegex))
    {
        Port p;
        p.type = match[1];

        std::string widthStr = match[2];
        if (!widthStr.empty())
        {
            int msb, lsb;
            sscanf_s(widthStr.c_str(), "[%d:%d]", &msb, &lsb);
            p.width = abs(msb - lsb) + 1;
        }
        else
            p.width = 1;

        std::string rest = line.substr(match[0].length());

        std::vector<std::string> commentVariants = { "//*", "/**", "//", "/*" };
        size_t firstPos = std::string::npos;

        for (const auto& cmt : commentVariants)
        {
            size_t pos = rest.find(cmt);
            if (pos != std::string::npos)
            {
                if (firstPos == std::string::npos || pos < firstPos)
                {
                    firstPos = pos;
                }
            }
        }

        if (firstPos != std::string::npos)
        {
            rest = rest.substr(0, firstPos);
        }
        std::stringstream ss(rest);
        std::string name;
        while (std::getline(ss, name, ','))
        {
            name.erase(0, name.find_first_not_of(" \t"));
            name.erase(name.find_last_not_of(" \t;") + 1);

            if (!name.empty())
            {
                Port np = p;
                np.name = name;
                ports.push_back(np);
            }
        }
    }
    return ports;
}

FileInfo Parser::parse()
{
    FileInfo fileinfo;
    Module module;
    Comment_block comment_block;
    std::vector<std::string> tags = { "@brief" , "@port" };

    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string& line = lines[i];
        if (line.find("input wire") != std::string::npos || line.find("output wire") != std::string::npos || line.find("output reg") != std::string::npos)
        {
            std::vector<Port> parsed_ports = PortParcer(line);
            for (const auto& p : parsed_ports)
                module.ports.push_back(p);
        }
        else if (line.find("module") != std::string::npos && line.find("endmodule") == std::string::npos)
        {
            size_t pos = line.find("module");
            if (pos != std::string::npos)
            {
                pos += 6;  //length of "module"
                size_t end = line.find_first_of("(", pos);
                module.name = trim(line.substr(pos, end - pos));
            }
            else
                module.name = "No name ";
        }
        if (line.find("//*") != std::string::npos)
        {
            size_t pos = line.find("//*");
            std::string cleaned_line = trim(line.substr(pos + 3));

            for (const auto& tag : tags)
            {
                if (line.find(tag) != std::string::npos)
                {
                    comment_block.tag = tag;

                    size_t tag_pos = cleaned_line.find(tag);
                    if (tag == "@port")
                    {
                        module.ports.back().description = trim(cleaned_line.substr(tag_pos + tag.length()));
                    }
                    if (tag_pos != std::string::npos)
                    {
                        cleaned_line.erase(tag_pos, tag.length());
                    }
                    break;
                }
            }
            comment_block.comment_block.push_back(cleaned_line);
            fileinfo.comments.push_back(comment_block);
            comment_block = Comment_block();
        }
        else if (line.find("/**") != std::string::npos)
        {
            while (i < lines.size())
            {
                std::string comment = trim(lines[i]);
                if (lines[i].find("/**") != std::string::npos)
                {
                    size_t start_pos = lines[i].find("/**");
                    comment = trim(lines[i].substr(start_pos + 3));
                }
                else if (lines[i].find("**/") != std::string::npos)
                {
                    size_t end_pos = lines[i].find("**/");
                    if (end_pos != std::string::npos && end_pos <= lines[i].size())
                        comment = trim(lines[i].substr(0, end_pos));
                    else
                        comment = "";
                    break;
                }
                for (const auto& tag : tags)
                {
                    if (comment.find(tag) != std::string::npos)
                    {
                        comment_block.tag = tag;

                        size_t tag_pos = comment.find(tag);
                        if (tag_pos != std::string::npos)
                        {
                            comment.erase(tag_pos, tag.length());
                        }
                        break;
                    }
                }
                if (comment.empty())
                {
                    ++i;
                    continue;
                }
                ++i;
                comment_block.comment_block.push_back(comment);

            }
            fileinfo.comments.push_back(comment_block);
            comment_block = Comment_block();
        }
    }
    fileinfo.modules.push_back(module);
    return fileinfo;
}