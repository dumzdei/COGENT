#include "Parser.h"

bool Parser_Verilog::IsMyFormat(const std::string& filename) {
    // Первым делом проверяем, может мы сможем по раширению понять, что за формат?
    auto pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        if (filename.substr(pos + 1) == "v")
            return true;
    }
    // Если делать нормально, то если по расширению понять не 
    // удалось, нужно попробовать открыть файл, зачитать 
    // пару строк и попытаться по ним понять, что же за формат
    // ...
    // Но пока оставим так
}

std::vector<Port> Parser_Verilog::ParsePort(const std::string& source_line)
{
    std::vector<Port> ports;

    std::string line = source_line;
    line = Trim(line);

    std::string description;
    size_t doc_pos = line.find("/**");
    if (doc_pos != std::string::npos)
    {
        size_t end_doc_pos = line.find("**/");
        if (end_doc_pos == std::string::npos)
            description = Trim(line.substr(doc_pos + 3));
        else
            description = Trim(line.substr(doc_pos + 3, end_doc_pos - (doc_pos + 3)));
        line.erase(doc_pos);
    }
    else
    {
        doc_pos = line.find("//*");
        if (doc_pos != std::string::npos)
        {
            description = Trim(line.substr(doc_pos + 3));
            line.erase(doc_pos);
        }
    }

    std::regex portRegex(
        R"(\b(input|output|inout)\s+(?:(wire|reg)\s+)?(?:\[([^\]]+)\]\s+)?)"
    );

    std::smatch match;
    if (std::regex_search(line, match, portRegex))
    {
        // Извлекаем общие атрибуты для всех портов в этой строке
        std::string direction = match[1];
        std::string net_type = Trim(match[2]);
        std::string range_content = Trim(match[3]);

        std::string port_type = net_type.empty() ? "wire" : net_type;
        std::string width = range_content.empty() ? "1" : range_content;

        // Извлекаем список имён портов
        std::string names_part = match.suffix().str();

        if (!names_part.empty() && names_part.back() == ';')
            names_part.pop_back();

        std::stringstream ss(names_part);
        std::string name;

        while (std::getline(ss, name, ','))
        {
            name = Trim(name);
            if (!name.empty())
            {
                Port p;
                p.name = name;
                p.direction = direction;
                p.type = port_type;
                p.width = width;
                p.description = description;

                ports.push_back(p);
            }
        }
    }

    return ports;
}

std::vector<Param> Parser_Verilog::ParseParam(const std::string& source_line)
{
    std::vector<Param> params;
    std::string line = Trim(source_line);

    std::string description;

    size_t pos = line.find("parameter");
    if (pos != std::string::npos)
        line = line.substr(pos + 9); // длина "parameter"

    size_t comment_pos = line.find("/**");
    if (comment_pos != std::string::npos) {
        size_t comment_end = line.find("**/", comment_pos);
        if (comment_end != std::string::npos) {
            description = Trim(line.substr(comment_pos + 3, comment_end - comment_pos - 3));
            line = line.substr(0, comment_pos) + line.substr(comment_end + 3);
        }
    }
    else {
        comment_pos = line.find("//*");
        if (comment_pos != std::string::npos) {
            description = Trim(line.substr(comment_pos + 3));
            line = line.substr(0, comment_pos);
        }
    }

    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ',')) 
    {
        std::string name;
        std::string value_str;

        size_t eq_pos = item.find('=');
        if (eq_pos != std::string::npos) {
            value_str = Trim(item.substr(eq_pos + 1));
            item = item.substr(0, eq_pos);
        }

        name = Trim(item);
        size_t semicolon_pos = name.find(';');
        if (semicolon_pos != std::string::npos) {
            name = name.substr(0, semicolon_pos);
        }

        if (!CleanToken(name).empty()) {
            Param np;
            np.name = name;
            np.value = CleanToken(value_str);
            // Приоритет: комментарий параметра > общий комментарий
            np.description = description ;
            params.push_back(np);
        }
    }
    return params;
}

std::vector<Module> Parser_Verilog::Parse(const std::string& source_file)
{
    Module module;
    std::vector<Module> modules;
    Comment_block comment_block;
    std::vector<Port> parsed_ports;
    std::vector<Param> parsed_params;
    bool module_area = false;

    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::string& line = lines[i];
        if (line.empty())
            continue;
        std::string current_tag;

        // Удаляем служебные комментарии
        if (line.find("//") != std::string::npos && line.find("//*") == std::string::npos)
        {
            size_t pos = line.find("//");
            line.erase(pos);
        }

        if (line.find("module") != std::string::npos && line.find("endmodule") == std::string::npos)
        {
            module_area = true;
            module.filename = source_file;

            size_t pos = line.find("module") + 6;
            size_t end = line.find_first_of("#(;");

            module.name = Trim(line.substr(pos, end - pos));

            // ID: имя_модуля + хеш файла (первые 4 символа)
            std::string base_filename = source_file.substr(source_file.find_last_of("/\\") + 1);
            size_t file_hash = std::hash<std::string>{}(base_filename);
            module.id = module.name + std::to_string(file_hash).substr(0, 4);
        }

        if (module_area && (line.find("input") != std::string::npos ||
            line.find("output") != std::string::npos ||
            line.find("inout") != std::string::npos))
        {
            auto ports = ParsePort(line);
            module.ports.insert(module.ports.end(), ports.begin(), ports.end());
        }

        else if (module_area && line.find("parameter") != std::string::npos)
        {
            auto params = ParseParam(line);
            module.params.insert(module.params.end(), params.begin(), params.end());
        }

        else if (line.find("function") != std::string::npos || line.find("task") != std::string::npos)
        {
            module_area = false;
        }

        if (line.find("/*") != std::string::npos &&
            line.find("/**") == std::string::npos &&
            line.find("//*") == std::string::npos)
        {
            while (i < lines.size() && line.find("*/") == std::string::npos)
            {
                line.clear();
                if (i + 1 < lines.size())
                {
                    i++;
                    line = lines[i];
                }
                else
                {
                    break;
                }
            }

            if (line.find("*/") != std::string::npos)
            {
                size_t endPos = line.find("*/");
                line.erase(0, endPos + 3);
            }
            else
            {
                line.clear();
            }
        }

        if (line.find("//*") != std::string::npos)
        {
            size_t pos = line.find("//*");

            std::string comment_text = line.substr(pos + 3);

            line.erase(pos);

            auto comment_blocks = Parse_CommentLine(comment_text);
            module.comments.insert(module.comments.end(),
                comment_blocks.begin(),
                comment_blocks.end());
        }
        else if (line.find("/**") != std::string::npos)
        {
            std::vector<std::string> full_comment;
            size_t startPos = line.find("/**");

            std::string comment_line = line.substr(startPos + 3);
            size_t endPos = comment_line.find("**/");

            if (endPos != std::string::npos)
            {
                full_comment.push_back(Trim(comment_line.substr(0, endPos)));
            }
            else
            {
                full_comment.push_back(Trim(comment_line));

                ++i;
                while (i < lines.size())
                {
                    std::string cur = lines[i];
                    size_t local_endPos = cur.find("**/");

                    if (local_endPos != std::string::npos)
                    {
                        full_comment.push_back(Trim(cur.substr(0, local_endPos)));

                        // Оставляем остаток строки после **/ для дальнейшей обработки
                        std::string after_comment = cur.substr(local_endPos + 3);
                        lines[i] = after_comment;
                        break;
                    }
                    else
                    {
                        full_comment.push_back(Trim(cur));
                        lines[i].clear();
                    }
                    ++i;
                }
            }

            if (!full_comment.empty())
            {
                auto comment_blocks = Parse_CommentLine(full_comment);
                module.comments.insert(module.comments.end(),
                    comment_blocks.begin(),
                    comment_blocks.end());
            }
        }

        else if (line.find("endmodule") != std::string::npos)
        {
            modules.push_back(module);

            module = Module();
            comment_block = Comment_block();
            parsed_ports.clear();
            parsed_params.clear();
            module_area = false;
        }
    }

    return modules;
}