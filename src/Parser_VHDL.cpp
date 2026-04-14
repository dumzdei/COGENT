#include "Parser.h"
#include <iostream>
#include "Colors.hpp"

bool Parser_VHDL::IsMyFormat(const std::string& filename) {
    // Проверка по расширению
    auto pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        std::string ext = filename.substr(pos + 1);
        if (ext == "vhd" || ext == "vhdl")
            return true;
    }

    // Проверка по содержимому (первые 100 строк)
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    int lines_checked = 0;
    while (std::getline(file, line) && lines_checked < 100) {
        lines_checked++;
        // Ищем ключевые слова VHDL (учитываем, что они могут быть в комментариях)
        if (line.find("--") == 0) continue; // Пропускаем строки-комментарии

        if (line.find("entity") != std::string::npos ||
            line.find("architecture") != std::string::npos ||
            line.find("port") != std::string::npos ||
            line.find("generic") != std::string::npos ||
            line.find("std_logic") != std::string::npos ||
            line.find("process") != std::string::npos) {
            return true;
        }
    }
    return false;
}

TokenType Parser_VHDL::KeywordToTokenType(const std::string& keyword) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"entity", TokenType::KW_ENTITY},
        {"architecture", TokenType::KW_ARCHITECTURE},
        {"end", TokenType::KW_END},
        {"begin", TokenType::KW_BEGIN},
        {"generic", TokenType::KW_GENERIC},
        {"port", TokenType::KW_PORT},
        {"constant", TokenType::KW_CONSTANT},
        {"signal", TokenType::KW_SIGNAL},
        {"process", TokenType::KW_PROCESS},
        {"downto", TokenType::KW_DOWNTO},
        {"to", TokenType::KW_TO},

        {"in", TokenType::DIR_IN},
        {"out", TokenType::DIR_OUT},
        {"inout", TokenType::DIR_INOUT_VHDL},
        {"buffer", TokenType::DIR_BUFFER},
        {"linkage", TokenType::DIR_LINKAGE},

        {"std_logic", TokenType::DT_STD_LOGIC},
        {"std_logic_vector", TokenType::DT_STD_LOGIC_VECTOR},
        {"integer", TokenType::DT_INTEGER},
        {"boolean", TokenType::DT_BOOLEAN},
        {"bit", TokenType::DT_BIT},
    };

    auto it = keywords.find(keyword);
    return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
}

bool Parser_VHDL::ReadComment(size_t& line, size_t& col) {
    if (line >= lines.size() || col + 1 >= lines[line].length())
        return false;

    const std::string& current_line = lines[line];
    size_t start_col = col;

    if (current_line[col] == '-' && current_line[col + 1] == '-') {
        bool is_doc = (col + 2 < current_line.length() && current_line[col + 2] == '*');
        size_t content_start = col + (is_doc ? 3 : 2);

        std::string content = (content_start < current_line.length()) ?
            current_line.substr(content_start) : "";

        TokenType type = is_doc ? TokenType::COMMENT_DOC_SINGLE : TokenType::COMMENT_SINGLE;
        tokens.push_back(CreateToken(type, content, content, line, start_col));

        col = current_line.length();
        return true;
    }
    return false;
}

bool Parser_VHDL::ParsePortList(size_t& token_index, Module& module) {
    if (!IsAtToken(token_index, TokenType::KW_PORT)) return false;
    NextToken(token_index);

    if (!IsAtToken(token_index, TokenType::OP_LPAREN)) return false;
    NextToken(token_index);

    std::string direction = "input";
    std::string data_type = "std_logic";
    std::string width = "1";
    std::string current_name;
    std::string description;
    bool reading_names = true;

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);

        if (tok.type == TokenType::IDENTIFIER && reading_names) {
            current_name = tok.value;
            NextToken(token_index);
            continue;
        }

        else if (tok.type == TokenType::OP_COLON) {
            reading_names = false;
            NextToken(token_index);
            continue;
        }

        else if (tok.type == TokenType::DIR_IN || tok.type == TokenType::DIR_OUT ||
            tok.type == TokenType::DIR_INOUT || tok.type == TokenType::DIR_BUFFER) {
            direction = tok.value;
            reading_names = false;
            NextToken(token_index);
            continue;
        }

        else if (tok.type == TokenType::DT_STD_LOGIC ||
            tok.type == TokenType::DT_STD_LOGIC_VECTOR ||
            tok.type == TokenType::DT_INTEGER ||
            tok.type == TokenType::DT_BOOLEAN) {
            data_type = tok.value;
            reading_names = false;
            NextToken(token_index);
            continue;
        }

        else if (tok.type == TokenType::OP_LPAREN && !reading_names) {
            width = ParseVHDLRange(token_index);
            continue;
        }

        // --*
        else if (tok.type == TokenType::COMMENT_DOC_SINGLE) {
            if (module.ports.back().description.empty())
                module.ports.back().description = tok.value;
            description = tok.value;
            NextToken(token_index);
            continue;
        }

        else if (tok.type == TokenType::OP_SEMICOLON || tok.type == TokenType::OP_COMMA) {
            if (!current_name.empty()) {
                AddVHDLPort(module, current_name, direction, data_type, width, "");
            }
            if (tok.type == TokenType::OP_SEMICOLON) {
                current_name.clear();
                direction = "input";
                data_type = "std_logic";
                width = "1";
                reading_names = true;
            }
            NextToken(token_index);
            continue;
        }

        else if (tok.type == TokenType::OP_RPAREN) {
            if (!current_name.empty()) {
                AddVHDLPort(module, current_name, direction, data_type, width, description);
            }
            NextToken(token_index);
            break;
        }
        NextToken(token_index);
    }
    return true;
}

bool Parser_VHDL::ParseGenericList(size_t& token_index, Module& module) {
    module.ShowParamType = false;     // VHDL has no types

    if (!IsAtToken(token_index, TokenType::KW_GENERIC)) return false;
    NextToken(token_index);

    if (!IsAtToken(token_index, TokenType::OP_LPAREN)) return false;
    NextToken(token_index);

    std::string data_type = "std_logic";
    std::string current_name;
    std::string pending_description;
    bool reading_names = true;

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);

        if (tok.type == TokenType::IDENTIFIER && reading_names) {
            current_name = tok.value;
            NextToken(token_index);
            continue;
        }

        else if (tok.type == TokenType::OP_COLON) {
            reading_names = false;
            NextToken(token_index);
            continue;
        }

        else if (tok.type == TokenType::DT_STD_LOGIC ||
            tok.type == TokenType::DT_STD_LOGIC_VECTOR ||
            tok.type == TokenType::DT_INTEGER ||
            tok.type == TokenType::DT_BOOLEAN) {
            data_type = tok.value;
            if (tok.type == TokenType::DT_STD_LOGIC_VECTOR) {
                NextToken(token_index);
                data_type += ParseVHDLRange(token_index);
            }
            reading_names = false;
            NextToken(token_index);
            continue;
        }

        if (tok.type == TokenType::OP_ASSIGN &&
            token_index + 1 < tokens.size() &&
            CurrentToken(token_index - 1).value == ":") {
            std::string default_value = ParseVHDLDefault(token_index);
            if (!current_name.empty()) {
                AddVHDLGeneric(module, current_name, data_type, default_value, pending_description);
                pending_description.clear();
                current_name.clear();
                data_type = "std_logic";
                default_value.clear();
                reading_names = true;
            }
            NextToken(token_index);
            continue;
        }

        // --*
        if (tok.type == TokenType::COMMENT_DOC_SINGLE) {
            module.params.back().description = tok.value;
            NextToken(token_index);
            continue;
        }

        // Конец списка дженериков
        if (tok.type == TokenType::OP_RPAREN) {
            NextToken(token_index);
            break;
        }

        NextToken(token_index);
    }
    return true;
}

bool Parser_VHDL::ParseEntity(size_t token_index, Module& module) {
    NextToken(token_index);

    if (!IsAtToken(token_index, TokenType::IDENTIFIER)) {
        std::cout << FORMAT_WARNING "Entity name not specified" << "\n";
        return false;
    }
    module.name = CurrentToken(token_index).value;
    NextToken(token_index);

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);

        // Generic list
        if (tok.type == TokenType::KW_GENERIC) {
            ParseGenericList(token_index, module);
            continue;
        }

        // Port list
        if (tok.type == TokenType::KW_PORT) {
            ParsePortList(token_index, module);
            continue;
        }

        if (tok.type == TokenType::KW_END) {
            NextToken(token_index);
            if (IsAtToken(token_index, TokenType::KW_ENTITY) ||
                IsAtToken(token_index, TokenType::IDENTIFIER)) {
                NextToken(token_index);
            }
            if (IsAtToken(token_index, TokenType::OP_SEMICOLON)) {
                NextToken(token_index);
            }
            return true;
        }

        NextToken(token_index);
    }
    return false;
}

std::vector<Module> Parser_VHDL::ParseFromTokens() {
    std::vector<Module> modules;
    Module module;
    size_t token_index = 0;

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);

        if (tok.type == TokenType::KW_ENTITY) {
            module.filename = current_filename;

            // ID 
            std::string base_filename = module.filename;
            size_t path_pos = base_filename.find_last_of("/\\");
            if (path_pos != std::string::npos) {
                base_filename = base_filename.substr(path_pos + 1);
            }
            size_t file_hash = std::hash<std::string>{}(base_filename);
            module.id = base_filename.substr(0, base_filename.find_last_of('.')) +
                std::to_string(file_hash).substr(0, 4);

            ParseEntity(token_index, module);
        }
        else if (tok.type == TokenType::COMMENT_DOC_SINGLE) {
            std::vector<Comment_block> blocks = ParseCommentText(tok.value);
            size_t block_count = blocks.size();
            for (int i = 0; i < block_count; i++) {
                module.comments.push_back(blocks[i]);
            }
        }
        else if (tok.type == TokenType::KW_END) {
            NextToken(token_index);
            if (IsAtToken(token_index, TokenType::KW_ENTITY)) {
                modules.push_back(module);
            }
        }
        NextToken(token_index);
    }
    return modules;
}

// Range processing: (7 downto 0) или (0 to 7)
std::string Parser_VHDL::ParseVHDLRange(size_t& token_index) {
    if (!IsAtToken(token_index, TokenType::OP_LPAREN)) return "1";
    NextToken(token_index);

    std::string range;

    while (token_index < tokens.size() &&
        CurrentToken(token_index).type != TokenType::OP_RPAREN) {
        if (IsAtToken(token_index, TokenType::KW_DOWNTO)) {
            range += " downto ";
        }
        else if (IsAtToken(token_index, TokenType::KW_TO)) {
            range += " to ";
        } 
        else {
            range += CurrentToken(token_index).value;
        }
        NextToken(token_index);
    }

    if (IsAtToken(token_index, TokenType::OP_RPAREN)) {
        NextToken(token_index);
    }
    return range.empty() ? "1" : range;
}

// :=
std::string Parser_VHDL::ParseVHDLDefault(size_t token_index) {
    // Check for ":="
    if (token_index + 1 >= tokens.size()) return "";
    if (CurrentToken(token_index).type != TokenType::OP_ASSIGN ||
        CurrentToken(token_index - 1).value != ":") {
        return "";
    }

    NextToken(token_index); // '='

    std::string value;
    while (token_index < tokens.size()) {
        TokenType t = CurrentToken(token_index).type;
        if (t == TokenType::OP_SEMICOLON || t == TokenType::COMMENT_DOC_SINGLE || 
            t == TokenType::COMMENT_SINGLE || t == TokenType::OP_RPAREN) {
            break;
        }
        value += CurrentToken(token_index).lexeme;
        NextToken(token_index);
    }
    return value;
}

// Adding a port
void Parser_VHDL::AddVHDLPort(Module& module, const std::string& name,
    const std::string& direction, const std::string& type,
    const std::string& width, const std::string& description) {
    Port p;
    p.name = name;
    p.direction = direction;
    p.type = type;
    p.width = width;
    p.description = description;
    module.ports.push_back(p);
}

// Adding a generic
void Parser_VHDL::AddVHDLGeneric(Module& module, const std::string& name,
    const std::string& type, const std::string& value,
    const std::string& description) {
    Param p;
    p.name = name;
    p.data_type = type;
    p.value = value;
    p.type = "generic";
    p.description = description;
    module.params.push_back(p);
}