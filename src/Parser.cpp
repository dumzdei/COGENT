#include "Parser.h"


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

inline std::string Parser::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";

    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

std::string Parser::extractTag(std::string& text)
{
    for (const auto& tag : tags)
    {
        size_t pos = text.find(tag);
        if (pos != std::string::npos)
        {
            text.erase(pos, tag.length());
            return tag;
        }
    }
    return "";
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
            if (cmt == "/**" && pos != std::string::npos)
                break;
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
			name = trim(name);
            name.erase(name.find_last_not_of(" \t;") + 1);

            if (!name.empty())
            {
                Port np = p;
                if (name.find("/**") != std::string::npos && name.find("**/") != std::string::npos)
                {
                    size_t start_desc = name.find("/**");
                    size_t end_desc =  name.find("**/");
                    np.description = trim(name.substr(start_desc +  3, end_desc - start_desc - 3));
					name = name.substr(0, start_desc);
				}
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

		size_t eq_pos = hash.find('=');
        if (eq_pos != std::string::npos)
        {
			size_t start_desc = hash.find("/**") + 3;
			size_t end_desc = hash.find("**/");

			name = trim(hash.substr(0, eq_pos));

            size_t value_start = eq_pos + 1;
            size_t value_end = value_start;
            while (value_end < hash.size() && (isdigit(hash[value_end]) || hash[value_end] == ' '))
                ++value_end;

            value_str = hash.substr(value_start, value_end - value_start);

            if (start_desc != std::string::npos && end_desc != std::string::npos)
            {
				description = trim(hash.substr(start_desc, end_desc - start_desc));
            }
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

std::vector<Module> Parser::parse()
{
    Module module;
    std::vector<Module> modules;
    Comment_block comment_block;
    std::vector<Port> parsed_ports;
    std::vector<Param> parsed_params;
    bool module_area = false;

    for (size_t i = 0; i < lines.size(); ++i)
    {
        const std::string& line = lines[i];

        // Обработка портов
        if (module_area && (line.find("input") != std::string::npos ||
            line.find("output") != std::string::npos ||
            line.find("inout") != std::string::npos))
        {
            auto ports = PortParcer(line);
            module.ports.insert(module.ports.end(), ports.begin(), ports.end());
        }
        // Обработка параметров
        else if (module_area && line.find("parameter") != std::string::npos)
        {
            auto params = ParamParcer(line);
            module.params.insert(module.params.end(), params.begin(), params.end());
        }
        // Выход из области модуля при функции/таске
        else if (line.find("function") != std::string::npos || line.find("task") != std::string::npos)
        {
            module_area = false;
        }
        // Начало модуля
        else if (line.find("module") != std::string::npos && line.find("endmodule") == std::string::npos)
        {
            module_area = true;

            size_t pos = line.find("module") + 6;
            size_t end = line.find_first_of("#(;");

            module.name = trim(line.substr(pos, end - pos));
        }
        // Конец модуля
        else if (line.find("endmodule") != std::string::npos)
        {
            if (!comment_block.comment_block.empty())
                module.comments.push_back(comment_block);

            modules.push_back(module);

            // Сброс данных для следующего модуля
            module = Module();
            comment_block = Comment_block();
            parsed_ports.clear();
            parsed_params.clear();
            module_area = false;
        }

        // Однострочные комментарии
        if (line.find("//*") != std::string::npos)
        {
            size_t pos = line.find("//*");
            std::string comment = trim(line.substr(pos + 3));

            comment_block.tag = extractTag(comment);

            if (!comment.empty())
                comment_block.comment_block.push_back(comment);
        }
        // Многострочные /** **/ комментарии
        else if (line.find("/**") != std::string::npos)
        {
            while (i < lines.size() && lines[i].find("**/") == std::string::npos)
            {
                std::string comment = trim(lines[i]);
                if (lines[i].find("/**") != std::string::npos)
                    comment = trim(lines[i].substr(lines[i].find("/**") + 3));

                std::string tag = extractTag(comment);
                if (!tag.empty() && comment_block.tag.empty())
                    comment_block.tag = tag;
                else if (!tag.empty())
                {
                    if (!comment_block.comment_block.empty())
                        module.comments.push_back(comment_block);

                    comment_block = Comment_block();
                    comment_block.tag = tag;
                }

                if (!comment.empty())
                    comment_block.comment_block.push_back(comment);

                ++i;
            }
            // Обязательно добавить последнюю строку с "**/"
            if (i < lines.size())
            {
                std::string lastLine = trim(lines[i]);
                size_t endPos = lastLine.find("**/");
                if (endPos != std::string::npos)
                    lastLine = trim(lastLine.substr(0, endPos));
                if (!lastLine.empty())
                    comment_block.comment_block.push_back(lastLine);
            }
        }
    }

    return modules;
}
