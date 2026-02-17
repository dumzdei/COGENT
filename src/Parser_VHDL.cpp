#include "Parser.h"

bool Parser_VHDL::IsMyFormat(const std::string& filename) {
    // ѕервым делом провер€ем, может мы сможем по раширению пон€ть, что за формат?
    auto pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        if (filename.substr(pos + 1) == "vhd" || filename.substr(pos + 1) == "vhdl")
            return true;
    }
    // ≈сли делать нормально, то если по расширению пон€ть не 
    // удалось, нужно попробовать открыть файл, зачитать 
    // пару строк и попытатьс€ по ним пон€ть, что же за формат
    // ...
    // Ќо пока оставим так
}

std::vector<Port> Parser_VHDL::ParsePort(const std::string& source_line)
{
    std::vector<Port> ports;

    std::string line = source_line;
    line = Trim(line);

    std::regex portRegex(R"(\b(input|output|inout)\s*(wire|reg|logic)?\s*(\[[^\]]+\])?\s*([^,);/\s]+)\s*)");

    std::sregex_iterator it(line.begin(), line.end(), portRegex);
    std::sregex_iterator end;

    while (it != end)
    {
        std::smatch match = *it;

        // ѕровер€ем, что это действительно объ€вление порта, а не часть другого слова
        size_t match_pos = match.position();
        if (match_pos > 0 && std::isalnum(line[match_pos - 1]))
        {
            ++it;
            continue;
        }

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

        std::string name = Trim(match[4]);

        if (match.suffix().matched)
        {
            if (match.suffix().str().find("/**") != std::string::npos && match.suffix().str().find("**/") != std::string::npos)
            {
                size_t start_desc = match.suffix().str().find("/**");
                size_t end_desc = match.suffix().str().find("**/");
                p.description = Trim(match.suffix().str().substr(start_desc + 3, end_desc - start_desc - 3));
            }
            else if (match.suffix().str().find("//*") != std::string::npos)
            {
                size_t start_desc = match.suffix().str().find("//*");
                p.description = Trim(match.suffix().str().substr(start_desc + 3));
            }
        }
        p.name = name;
        ports.push_back(p);
        ++it;
    }

    return ports;
}

std::vector<Param> Parser_VHDL::ParseGenerics(const std::string& source_line)
{
    std::vector<Param> params;

    std::string line = Trim(source_line);
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

            name = Trim(hash.substr(0, eq_pos));

            size_t value_start = eq_pos + 1;
            size_t value_end = value_start;
            while (value_end < hash.size() && (isdigit(hash[value_end]) || hash[value_end] == ' '))
                ++value_end;

            value_str = hash.substr(value_start, value_end - value_start);

            if (start_desc != std::string::npos && end_desc != std::string::npos)
            {
                description = Trim(hash.substr(start_desc, end_desc - start_desc));
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

std::vector<Module> Parser_VHDL::Parse(const std::string& source_file)
{
    Module module;
    std::vector<Module> modules;
    Comment_block comment_block;
    std::vector<Port> parsed_ports;
    std::vector<Param> parsed_params;
    bool module_area = false;

    std::regex port_keyword(R"(^.*port\b)", std::regex_constants::icase);
    std::regex generic_keyword(R"(^.*generic\b)", std::regex_constants::icase);

    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::string& line = lines[i];

        if (line.find("--") != std::string::npos && line.find("--*") == std::string::npos)
        {
            size_t pos = line.find("--");
            line.erase(pos);
        }

        if (line.find("--*") != std::string::npos)
        {
            comment_block = Comment_block();
            size_t pos = line.find("--*");

            std::string comment = Trim(line.substr(pos + 3));

            comment_block.tag = ExtractTag(comment);

            if (!comment.empty())
                comment_block.comment_block.push_back(comment);
            if (!comment_block.comment_block.empty())
                module.comments.push_back(comment_block);
            line.erase(pos);
        }

        if (line.find("entity") != std::string::npos &&
            line.find("end entity") == std::string::npos)
        {
            module_area = true;
            module.filename = source_file;

            size_t pos = line.find("entity");
            size_t start_name = pos + 6; // длина "entity"

            size_t end = line.find("is", start_name);

            // ≈сли "is" не найдено, берем до конца строки или скобки
            if (end == std::string::npos) {
                end = line.find_first_of("(", start_name);
            }

            module.name = Trim(line.substr(start_name, end - start_name));

            std::string base_filename = source_file.substr(source_file.find_last_of("/\\") + 1);
            size_t file_hash = std::hash<std::string>{}(base_filename);
            module.id = module.name + std::to_string(file_hash).substr(0, 4);
        }

        if (module_area && std::regex_search(line, port_keyword))
        {
            auto ports = ParsePort(line);
            module.ports.insert(module.ports.end(), ports.begin(), ports.end());
        }
        
        else if (module_area && std::regex_search(line, generic_keyword))
        {
            auto params = ParseGenerics(line);
            module.params.insert(module.params.end(), params.begin(), params.end());
        }

        /*else if (line.find("function") != std::string::npos || line.find("task") != std::string::npos)
        {
            module_area = false;
        }*/

        else if (line.find("end") != std::string::npos && line.find(module.name) != std::string::npos)
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