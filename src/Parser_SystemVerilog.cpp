#include "Parser.h"

bool Parser_SystemVerilog::IsMyFormat(const std::string& filename) {
    // Первым делом проверяем, может мы сможем по раширению понять, что за формат?
    auto pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        std::string ext = filename.substr(pos + 1);
        if (ext == "sv" || ext == "svh" || ext == "v" || ext == "vh")
            return true;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    int lines_checked = 0;
    while (std::getline(file, line) && lines_checked < 100) {
        lines_checked++;
        std::string line;

        if (line.find("module") != std::string::npos ||
            line.find("interface") != std::string::npos ||
            line.find("package") != std::string::npos ||
            line.find("logic") != std::string::npos ||
            line.find("always_ff") != std::string::npos ||
            line.find("always_comb") != std::string::npos) {
            return true;
        }
    }

    return false;
}

bool Parser_SystemVerilog::ParseModule(size_t& token_index, Module& module) {
    // Пропускаем 'module'
    NextToken(token_index);

    // Имя модуля
    if (!IsAtToken(token_index, TokenType::IDENTIFIER)) {
        return false;
    }
    module.name = CurrentToken(token_index).value;
    NextToken(token_index);

    // Параметры #(...)
    if (IsAtToken(token_index, TokenType::OP_HASH)) {
        NextToken(token_index);  // #
        if (IsAtToken(token_index, TokenType::OP_LPAREN)) {
            ParseParameterList(token_index, module);
        }
    }

    // Порты (...)
    if (IsAtToken(token_index, TokenType::OP_LPAREN)) {
        ParsePortList(token_index, module);
    }

    // Ищем endmodule
    while (token_index < tokens.size()) {
        if (CurrentToken(token_index).type == TokenType::KW_ENDMODULE) {
            NextToken(token_index);
            return true;
        }

        // Параметры внутри модуля
        if (CurrentToken(token_index).type == TokenType::KW_PARAM ||
            CurrentToken(token_index).type == TokenType::KW_LOCALPARAM) {
            ParseParameterList(token_index, module);
            continue;
        }

        // Порты внутри модуля (ANSI стиль)
        if (CurrentToken(token_index).type == TokenType::DIR_INPUT ||
            CurrentToken(token_index).type == TokenType::DIR_OUTPUT ||
            CurrentToken(token_index).type == TokenType::DIR_INOUT) {
            ParsePortList(token_index, module);
            continue;
        }

        NextToken(token_index);
    }

    return false;
}

bool Parser_SystemVerilog::ParsePortList(size_t& token_index, Module& module) {
    // Пропускаем '(' если есть
    if (IsAtToken(token_index, TokenType::OP_LPAREN)) {
        NextToken(token_index);
    }

    std::string direction;
    std::string port_type = "logic";
    std::string width = "1";
    std::string description;

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);

        // Направление
        if (tok.type == TokenType::DIR_INPUT ||
            tok.type == TokenType::DIR_OUTPUT ||
            tok.type == TokenType::DIR_INOUT) {
            direction = tok.value;
            NextToken(token_index);
            continue;
        }

        // Тип
        else if (tok.type == TokenType::DT_WIRE || tok.type == TokenType::DT_REG ||
            tok.type == TokenType::DT_LOGIC || tok.type == TokenType::DT_TRI ||
            tok.type == TokenType::DT_BIT) {
            port_type = tok.value;
            NextToken(token_index);
            continue;
        }

        // Ширина [N:M]
        else if (tok.type == TokenType::OP_LBRACKET) {
            NextToken(token_index);
            width = "";
            while (token_index < tokens.size() &&
                CurrentToken(token_index).type != TokenType::OP_RBRACKET) {
                width += CurrentToken(token_index).value;
                NextToken(token_index);
            }
            if (IsAtToken(token_index, TokenType::OP_RBRACKET)) {
                NextToken(token_index);
            }
            continue;
        }

        // Имя порта
        else if (tok.type == TokenType::IDENTIFIER) {
            Port p;
            p.name = tok.value;
            p.direction = direction.empty() ? "input" : direction;
            p.type = port_type;
            p.width = width;
            module.ports.push_back(p);

            NextToken(token_index);

            // Запятая или конец
            if (IsAtToken(token_index, TokenType::OP_COMMA)) {
                NextToken(token_index);
                direction = "";
                port_type = "logic";
                width = "1";
            }
            continue;
        }
        // Описание порта
        else if (tok.type == TokenType::COMMENT_DOC_SINGLE ||
            tok.type == TokenType::COMMENT_DOC_MULTI) {
            if (!module.ports.empty())
                module.ports.back().description = tok.value;
        }
        // Конец списка портов
        if (tok.type == TokenType::OP_RPAREN ||
            tok.type == TokenType::OP_SEMICOLON) {
            if (tok.type == TokenType::OP_RPAREN) {
                NextToken(token_index);
            }
            break;
        }
        NextToken(token_index);
    }

    return true;
}

bool Parser_SystemVerilog::ParseParameterList(size_t& token_index, Module& module) {
    // Пропускаем '(' если есть
    if (IsAtToken(token_index, TokenType::OP_LPAREN)) {
        NextToken(token_index);
    }

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);
        std::string data_type;
        bool is_local = true;
        
        if (tok.type == TokenType::KW_LOCALPARAM) {
            is_local = true;
        }

        if (tok.type == TokenType::DT_LOGIC || tok.type == TokenType::DT_REG ||       ///ADD TYPEDEF SUPPORT
            tok.type == TokenType::DT_WIRE || tok.type == TokenType::DT_BIT ||        
            tok.type == TokenType::DT_INT || tok.type == TokenType::DT_BYTE ||
            tok.type == TokenType::KW_TYPE) {
            data_type = tok.value;
        }
        else if (tok.type == TokenType::COMMENT_DOC_SINGLE ||
            tok.type == TokenType::COMMENT_DOC_MULTI) {
            if (!module.params.empty())
                module.params.back().description = tok.value;
        }
        // Имя параметра
        else if (tok.type == TokenType::IDENTIFIER) {
            Param p;
            p.name = tok.value;
            p.type = is_local ? "localparam" : "parameter";
            p.data_type = data_type;

            NextToken(token_index);

            // Значение = ...
            if (IsAtToken(token_index, TokenType::OP_ASSIGN)) {
                NextToken(token_index);
                while (CurrentToken(token_index).type != TokenType::OP_COMMA &&
                    CurrentToken(token_index).type != TokenType::OP_RPAREN &&
                    CurrentToken(token_index).type != TokenType::COMMENT_DOC_SINGLE &&
                    CurrentToken(token_index).type != TokenType::COMMENT_DOC_MULTI &&
                    CurrentToken(token_index).type != TokenType::COMMENT_SINGLE &&
                    CurrentToken(token_index).type != TokenType::COMMENT_MULTI) {
                    p.value += CurrentToken(token_index).lexeme;
                    NextToken(token_index);
                }
            }

            module.params.push_back(p);
            // Запятая
            if (IsAtToken(token_index, TokenType::OP_COMMA)) {
                NextToken(token_index);
            }
            continue;
        }
        NextToken(token_index);

        // Конец списка
        if (tok.type == TokenType::OP_RPAREN ||
            tok.type == TokenType::OP_SEMICOLON) {
            if (tok.type == TokenType::OP_RPAREN) {
                NextToken(token_index);
            }
            break;
        }
    }

    return true;
}


std::vector<Module> Parser_SystemVerilog::ParseFromTokens() {
    std::vector<Module> modules;
    size_t token_index = 0;

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);

        if (tok.type == TokenType::KW_MODULE) {
            Module module;
            module.filename = current_filename;

            // ID
            std::string base_filename = module.filename;
            size_t path_pos = base_filename.find_last_of("/\\");
            if (path_pos != std::string::npos) {
                base_filename = base_filename.substr(path_pos + 1);
            }
            size_t file_hash = std::hash<std::string>{}(base_filename);
            module.id = module.name + std::to_string(file_hash).substr(0, 4);

            if (ParseModule(token_index, module)) {
                modules.push_back(module);
            }
        }
        else {
            NextToken(token_index);
        }
    }

    return modules;
}