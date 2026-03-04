#include "Parser.h"

bool Parser_VHDL::IsMyFormat(const std::string& filename) {
    // Первым делом проверяем, может мы сможем по раширению понять, что за формат?
    auto pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        if (filename.substr(pos + 1) == "vhd" || filename.substr(pos + 1) == "vhdl")
            return true;
    }
    // Если делать нормально, то если по расширению понять не 
    // удалось, нужно попробовать открыть файл, зачитать 
    // пару строк и попытаться по ним понять, что же за формат
    // ...
    // Но пока оставим так
}


std::string ConvertVHDLRange(const std::string& vhdl_range)
{
    // Парсим VHDL диапазон: (7 downto 0) или (0 to 7)
    std::regex rangeRegex(R"(\(\s*(\d+)\s*(downto|to)\s*(\d+)\s*\))");
    std::smatch match;

    if (std::regex_search(vhdl_range, match, rangeRegex))
    {
        int msb = std::stoi(match[1]);
        int lsb = std::stoi(match[3]);
        std::string direction = match[2];

        // Формируем унифицированный вид [msb:lsb]
        if (direction == "to")
            return "[" + std::to_string(lsb) + ":" + std::to_string(msb) + "]";
        else
            return "[" + std::to_string(msb) + ":" + std::to_string(lsb) + "]";
    }

    return "1";
}

std::vector<Port> Parser_VHDL::ParsePort(const std::string& source_line)
{
    std::vector<Port> ports;

    std::string line = source_line;
    line = Trim(line);

    std::string description;
    size_t doc_comment_pos = line.find("--*");
    if (doc_comment_pos != std::string::npos)
    {
        // Извлекаем всё после --* до конца строки
        description = Trim(line.substr(doc_comment_pos + 3));
    }

    std::regex portRegex(
        R"(\b([a-zA-Z_][a-zA-Z0-9_]*)\s*:\s*(in|out|inout|buffer|linkage)\s+([a-zA-Z_][a-zA-Z0-9_]*(?:\s*\([^)]+\))?)\s*)"
    );

    std::sregex_iterator it(line.begin(), line.end(), portRegex);
    std::sregex_iterator end;

    while (it != end)
    {
        std::smatch match = *it;

        Port p;
        p.name = Trim(match[1]);                    // Имя порта
        p.direction = match[2];                     // Направление (in/out/...)
        std::string base_type = Trim(match[3]);     // Тип с возможным диапазоном

        // Выделяем диапазон, если он есть в типе
        size_t paren_pos = base_type.find('(');
        if (paren_pos != std::string::npos)
        {
            p.type = Trim(base_type.substr(0, paren_pos));      // std_logic_vector
            std::string range = Trim(base_type.substr(paren_pos)); // (7 downto 0)
            p.width = ConvertVHDLRange(range);
        }
        else
        {
            p.type = base_type;
            p.width = "1";
        }

        p.description = description;

        ports.push_back(p);
        ++it;
    }

    return ports;
}

std::vector<Param> Parser_VHDL::ParseGenerics(const std::string& source_line)
{
    std::vector<Param> params;
    std::string line = Trim(source_line);

    std::string description;

    size_t pos = line.find("generic");
    if (pos != std::string::npos)
        line = line.substr(pos + 7);

    size_t comment_pos = line.find("--*");
    if (comment_pos != std::string::npos) {
        description = Trim(line.substr(comment_pos + 3));
        line = line.substr(0, comment_pos);
    }

    std::stringstream ss(line);
    std::string item;
    while (std::getline(ss, item, ';'))
    {
        std::string name;
        std::string type;
        std::string value_str;

        size_t eq_pos = item.find(":=");
        if (eq_pos != std::string::npos) {
            value_str = Trim(item.substr(eq_pos + 2));
            item = item.substr(0, eq_pos);
        }

        // Поиск типа
        size_t colon_pos = item.find(':');
        if (colon_pos != std::string::npos) {
            name = Trim(item.substr(0, colon_pos));
            type = Trim(item.substr(colon_pos + 1));
        }
        else {
            name = item;
        }

        if (!CleanToken(name).empty()) {
            Param np;
            np.name = name;
            np.type = CleanToken(type);
            np.value = CleanToken(value_str);
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

    std::regex port_keyword(R"(port\s*\()", std::regex_constants::icase);
    std::regex generic_keyword(R"(generic\s*\()", std::regex_constants::icase);

    // Переменные состояния для сбора блоков
    std::vector<std::string> port_buffer, generic_buffer;
    bool in_port_block = false;
    bool in_generic_block = false;
    int brace_balance = 0; // Счётчик вложенности скобок

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
                comment_block.lines.push_back(comment);
            if (!comment_block.lines.empty())
                module.comments.push_back(comment_block);
        }

        if (line.find("entity") != std::string::npos &&
            line.find("end entity") == std::string::npos)
        {
            module_area = true;
            module.filename = source_file;

            size_t pos = line.find("entity");
            size_t start_name = pos + 6; // длина "entity"

            size_t end = line.find("is", start_name);

            // Если "is" не найдено, берем до конца строки или скобки
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
            in_port_block = true;
            brace_balance = 0;
            port_buffer.clear();

            // Считаем скобки в этой строке и сохраняем содержимое
            for (char c : line) {
                if (c == '(') brace_balance++;
                if (c == ')') brace_balance--;
            }
            port_buffer.push_back(line);
            
            continue;
        }

        if (in_port_block)
        {
            {
                for (char c : line) {
                    if (c == '(') brace_balance++;
                    if (c == ')') brace_balance--;
                }

                port_buffer.push_back(line);

                if (brace_balance <= 0)
                {
                    for (size_t i = 0; i < port_buffer.size(); i++) {
                        auto ports = ParsePort(port_buffer[i]);
                        if (!ports.empty())
                            module.ports.insert(module.ports.end(), ports.begin(), ports.end());
                    }
                    in_port_block = false;
                    port_buffer.clear();
                }
            }
            continue;
        }

        if (module_area && std::regex_search(line, generic_keyword))
        {
            in_generic_block = true;
            brace_balance = 0;
            generic_buffer.clear();

            for (char c : line) {
                if (c == '(') brace_balance++;
                if (c == ')') brace_balance--;
            }
            generic_buffer.push_back(line);
            continue;
        }

        if (in_generic_block)
        {
            {
                for (char c : line) {
                    if (c == '(') brace_balance++;
                    if (c == ')') brace_balance--;
                }
                generic_buffer.push_back(line);

                if (brace_balance <= 0)
                {
                    for (size_t i = 0; i < generic_buffer.size(); i++) {
                        auto params = ParseGenerics(generic_buffer[i]);
                        if (!params.empty())
                            module.params.insert(module.params.end(), params.begin(), params.end());
                    }
                    in_generic_block = false;
                    generic_buffer.clear();
                }
            }
            continue;
        }

        /*else if (line.find("function") != std::string::npos || line.find("task") != std::string::npos)
        {
            module_area = false;
        }*/

        else if (line.find("end entity") != std::string::npos && line.find(module.name) != std::string::npos)
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