#include "Parser.h"
#include <sstream>
#include <regex>
#include <filesystem>

bool Parser::loadFile(const std::string& filename)
{
    lines.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return false;

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

    std::regex portRegex(R"(\b(input|output|inout)\b\s*(wire|reg|logic)?\s*(\[[^\]]+\])?)");

    std::smatch match;

    if (std::regex_search(line, match, portRegex))
    {
        Port p;
        p.direction = match[1];

        if (match[2].matched)
            p.type = match[2];
        else
			p.type = "wire";

        if (match[3].matched)
            p.width = match[3];
        else
            p.width = "1";

        std::string rest = line.substr(match[0].length());

        
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
            if (cmt == "//*" && pos != std::string::npos)
            {
				p.description = trim(rest.substr(pos + 3));
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
			trim(name);
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

std::vector<Param> Parser::ParamParcer(const std::string& source_line)
{
    std::vector<Param> params;
    
    std::string line = trim(source_line);
	size_t pos = line.find("parameter");
	line = line.substr(pos + 9, line.length());

    std::stringstream ss(line);
    std::string hash;
    while (std::getline(ss, hash, ','))
    {
        std::string name;
        std::string value_str;
        std::string description;
        if (hash.find('=') != std::string::npos)
        {
            if (hash.find("/**") != std::string::npos && hash.find("**/") != std::string::npos)
            {
				description = trim(hash.substr(hash.find("/**") + 3, hash.find("**/") - (hash.find("/**") + 3)));
            }
            name = trim(hash.substr(0, hash.find('=')));
			value_str = trim(hash.substr(hash.find('=') + 1, ',' | ')'));
        }

        if (!name.empty())
        {
            Param np;
            np.name = name;
			np.value = value_str;
			np.description = description;
            params.push_back(np);
        }
    }
    return params;
}

FileInfo Parser::parse()
{
    FileInfo fileinfo;
    Module module;
    Comment_block comment_block;
    std::vector<std::string> tags = { "@brief" };

    bool module_area = false;

    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string& line = lines[i];

		std::vector<Port> parsed_ports;
		std::vector<Param> parsed_params;

        if (module_area && (line.find("input") != std::string::npos || line.find("output") != std::string::npos || line.find("inout") != std::string::npos))
        {
            parsed_ports = PortParcer(line);
        }

        if (module_area && line.find("parameter") != std::string::npos)
        {
            parsed_params = ParamParcer(line);
        }

        else if (line.find("function") != std::string::npos || line.find("task") != std::string::npos)
            module_area = false;

        else if (line.find("module") != std::string::npos && line.find("endmodule") == std::string::npos)
        {
            module_area = true;

            size_t end = line.length();
            char c;

            size_t pos = line.find("module");
            pos += 6;  //length of "module"
            
            for (size_t i = pos; i < line.length(); i++)
            {
                c = line[i];
                if (c == '#' || c == '(' || c == ';')
                {
                    end = i;
                    break;
                }
            }

            module.name = trim(line.substr(pos, end - pos));
            
        }
        if (line.find("//*") != std::string::npos)
        {
            size_t pos = line.find("//*");
            std::string comment = trim(line.substr(pos + 3));

            for (const auto& tag : tags)
            {
                if (line.find(tag) != std::string::npos)
                {
                    comment_block.tag = tag;
                    size_t tag_pos = comment.find(tag);
                    
                    comment.erase(tag_pos, tag.length());

                    break;
                }
            }

            comment_block.comment_block.push_back(comment);
            fileinfo.comments.push_back(comment_block);
            comment_block = Comment_block();
        }
        else if (line.find("/**") != std::string::npos)
        {
            while (lines[i].find("**/") == std::string::npos)
            {
                std::string comment = trim(lines[i]);
                if (lines[i].find("/**") != std::string::npos)
                {
                    comment = trim(lines[i].substr(lines[i].find("/**") + 3));
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
				comment_block.comment_block.push_back(comment);
                ++i;
            }
            fileinfo.comments.push_back(comment_block);
            comment_block = Comment_block();
        }

        for (const auto& p : parsed_ports)
            module.ports.push_back(p);

		for (const auto& p : parsed_params)
			module.params.push_back(p);
    }
    fileinfo.modules.push_back(module);
    return fileinfo;
}