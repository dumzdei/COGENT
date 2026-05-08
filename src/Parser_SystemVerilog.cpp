#include <iostream>
#include "Parser.h"
#include "Colors.hpp"

bool Parser_SystemVerilog::IsMyFormat(const std::string& filename) {
    // ѕервым делом провер€ем, может мы сможем по раширению пон€ть, что за формат?
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
            line.find("always") != std::string::npos) {
            return true;
        }
    }

    return false;
}

TokenType Parser_SystemVerilog::KeywordToTokenType(const std::string& keyword) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"module", TokenType::KW_MODULE},
        {"endmodule", TokenType::KW_ENDMODULE},
        {"interface", TokenType::KW_INTERFACE},
        {"endinterface", TokenType::KW_ENDINTERFACE},
        {"package", TokenType::KW_PACKAGE},
        {"endpackage", TokenType::KW_ENDPACKAGE},
        {"function", TokenType::KW_FUNCTION},
        {"endfunction", TokenType::KW_ENDFUNCTION},
        {"task", TokenType::KW_TASK},
        {"endtask", TokenType::KW_ENDTASK},
        {"parameter", TokenType::KW_PARAM},
        {"localparam", TokenType::KW_LOCALPARAM},
        {"signed", TokenType::KW_SIGNED},
        {"enum", TokenType::KW_ENUM},
        {"type", TokenType::KW_TYPE},

        {"input", TokenType::DIR_INPUT},
        {"output", TokenType::DIR_OUTPUT},
        {"inout", TokenType::DIR_INOUT},

        {"logic", TokenType::DT_LOGIC},
        {"wire", TokenType::DT_WIRE},
        {"tri", TokenType::DT_TRI},
        {"reg", TokenType::DT_REG},
        {"int", TokenType::DT_INT},
        {"integer", TokenType::DT_INT},
        {"byte", TokenType::DT_BYTE},
        {"bit", TokenType::DT_BIT},
        {"struct", TokenType::DT_STRUCT}
    };

    auto it = keywords.find(keyword);
    return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
}

bool Parser_SystemVerilog::ParseModule(size_t& token_index, Module& module) {
    // Skipping 'module'
    NextToken(token_index);

    // Module name
    if (!IsAtToken(token_index, TokenType::IDENTIFIER)) {
        std::cout << FORMAT_WARNING "No module name";
        return false;
    }
    module.name = CurrentToken(token_index).value;
    NextToken(token_index);

    // Parameters #(...)
    if (IsAtToken(token_index, TokenType::OP_HASH)) {
        NextToken(token_index);  // #
        if (IsAtToken(token_index, TokenType::OP_LPAREN)) {
            ParseParameterList(token_index, module);
        }
    }

    // Ports (...)
    if (IsAtToken(token_index, TokenType::OP_LPAREN)) {
        ParsePortList(token_index, module);
    }

    while (token_index < tokens.size()) {
        switch (CurrentToken(token_index).type) {
        case TokenType::KW_ENDMODULE:
            NextToken(token_index);
            return true;

        case TokenType::KW_PARAM:
        case TokenType::KW_LOCALPARAM:
            ParseParameterList(token_index, module);
            continue;

        case TokenType::DIR_INPUT:
        case TokenType::DIR_OUTPUT:
        case TokenType::DIR_INOUT:
            ParsePortList(token_index, module);
            continue;

        default:
            NextToken(token_index);
            break;
        }
    }
    return false;
}

bool Parser_SystemVerilog::ParsePortList(size_t& token_index, Module& module) {
    // ѕропускаем '(' если есть
    if (IsAtToken(token_index, TokenType::OP_LPAREN)) {
        NextToken(token_index);
    }

    std::string direction;
    std::string port_type = "logic";
    std::string width = "1";
    std::string description;

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);

        // Direction
        if (tok.type == TokenType::DIR_INPUT ||
            tok.type == TokenType::DIR_OUTPUT ||
            tok.type == TokenType::DIR_INOUT) {
            direction = tok.value;
            NextToken(token_index);
            continue;
        }

        // Type
        else if (tok.type == TokenType::DT_WIRE || tok.type == TokenType::DT_REG ||
            tok.type == TokenType::DT_LOGIC || tok.type == TokenType::DT_TRI ||
            tok.type == TokenType::DT_BIT) {
            port_type = tok.value;
            NextToken(token_index);
            continue;
        }

        // Width [N:M]
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

        // Port name
        else if (tok.type == TokenType::IDENTIFIER) {
            Port p;

            if (known_types.count(tok.lexeme) > 0) {
                port_type = tok.value;
                if (known_types[tok.lexeme].find('[')) {
                    size_t wdt_start = known_types[tok.lexeme].find('[');
                    size_t wdt_end = known_types[tok.lexeme].find(']');
                    width = known_types[tok.lexeme].substr(wdt_start + 1, wdt_end - wdt_start - 1);
                }
                NextToken(token_index);
                continue;
            }

            p.name = tok.value;
            p.direction = direction.empty() ? "input" : direction;
            p.type = port_type;
            p.width = width;
            module.ports.push_back(p);

            NextToken(token_index);

            // Comma or end
            if (IsAtToken(token_index, TokenType::OP_COMMA)) {
                NextToken(token_index);
                direction = "";
                port_type = "logic";
                width = "1";
            }
            continue;
        }
        // Port description
        else if (tok.type == TokenType::COMMENT_DOC_SINGLE ||
            tok.type == TokenType::COMMENT_DOC_MULTI) {
            if (!module.ports.empty()) {
                std::vector<Comment_block> blocks = ParseCommentText(tok.value);
                module.ports.back().comments.insert(module.ports.back().comments.end(),
                    blocks.begin(), blocks.end());
            }
            NextToken(token_index);
            continue;
        }
        // End of port list
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
    // Skip '(' if there is one
    if (IsAtToken(token_index, TokenType::OP_LPAREN)) {
        NextToken(token_index);
    }

    std::string data_type;
    bool is_local = false;

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);
        
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
            if (!module.params.empty()) {
                std::vector<Comment_block> blocks = ParseCommentText(tok.value);
                module.params.back().comments.insert(module.params.back().comments.end(),
                    blocks.begin(), blocks.end());
            }
            NextToken(token_index);
            continue;
        }
        // Parameter name
        else if (tok.type == TokenType::IDENTIFIER) {
            Param p;
            p.name = tok.value;
            p.type = is_local ? "localparam" : "parameter";
            if (!data_type.empty())
            {
                module.ShowParamDataType = true;
                p.data_type = data_type;
            }

            NextToken(token_index);

            // Value = ...
            if (IsAtToken(token_index, TokenType::OP_ASSIGN)) {
                NextToken(token_index);
                if (!is_local)
                {
                    while (CurrentToken(token_index).type != TokenType::OP_COMMA &&
                        CurrentToken(token_index).type != TokenType::OP_SEMICOLON &&
                        CurrentToken(token_index).type != TokenType::OP_RPAREN &&
                        CurrentToken(token_index).type != TokenType::COMMENT_DOC_SINGLE &&
                        CurrentToken(token_index).type != TokenType::COMMENT_DOC_MULTI &&
                        CurrentToken(token_index).type != TokenType::COMMENT_SINGLE &&
                        CurrentToken(token_index).type != TokenType::COMMENT_MULTI) {
                        p.value += CurrentToken(token_index).lexeme;
                        NextToken(token_index);
                    }
                }
                else
                {
                    while (CurrentToken(token_index).type != TokenType::OP_SEMICOLON &&
                        CurrentToken(token_index).type != TokenType::COMMENT_DOC_SINGLE &&
                        CurrentToken(token_index).type != TokenType::COMMENT_DOC_MULTI &&
                        CurrentToken(token_index).type != TokenType::COMMENT_SINGLE &&
                        CurrentToken(token_index).type != TokenType::COMMENT_MULTI) {
                        p.value += CurrentToken(token_index).lexeme;
                        NextToken(token_index);
                    }
                }
            }

            if (data_type == "type") {
                known_types.insert({ p.name, p.value });
            }

            module.params.push_back(p);
            // Comma
            if (IsAtToken(token_index, TokenType::OP_COMMA)) {
                NextToken(token_index);
            }
            continue;
        }
        NextToken(token_index);

        // End of list
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

bool Parser_SystemVerilog::ReadComment(size_t& line, size_t& col) {
    if (line >= lines.size() || col + 1 >= lines[line].length()) return false;

    const std::string& current_line = lines[line];
    size_t start_col = col;

    char next = current_line[col + 1];

    if (next == '*') {
        // Multi-line comment /* or /**
        bool is_doc = (col + 2 < current_line.length() && current_line[col + 2] == '*');
        std::string content;
        size_t content_start = col + (is_doc ? 3 : 2);

        // First line
        if (content_start < current_line.length()) {
            size_t end_pos = current_line.find("*/", content_start);
            if (end_pos != std::string::npos) {
                content = current_line.substr(content_start, end_pos - content_start);
                col = end_pos + 2;
                TokenType type = is_doc ? TokenType::COMMENT_DOC_MULTI : TokenType::COMMENT_MULTI;
                tokens.push_back(CreateToken(type, content, content, line, start_col));
                return true;
            }
            else {
                content = current_line.substr(content_start);
            }
        }

        // Continued on the following lines
        line++;
        while (line < lines.size()) {
            size_t end_pos = lines[line].find("*/");
            if (end_pos != std::string::npos) {
                content += lines[line].substr(0, end_pos);
                col = end_pos + 2;
                TokenType type = is_doc ? TokenType::COMMENT_DOC_MULTI : TokenType::COMMENT_MULTI;
                tokens.push_back(CreateToken(type, content, content, line, start_col));
                return true;
            }
            else {
                content += ' ' + lines[line];
                line++;
            }
        }

    }
    else if (next == '/') {
        // Single-line comment // or //*
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

std::vector<Module> Parser_SystemVerilog::ParseFromTokens() {
    std::vector<Module> modules;
    Module module;
    size_t token_index = 0;

    while (token_index < tokens.size()) {
        Token& tok = CurrentToken(token_index);

        if (tok.type == TokenType::KW_MODULE)  {
            module.filename = current_filename;

            // ID
            std::string base_filename = module.filename;
            size_t path_pos = base_filename.find_last_of("/\\");
            if (path_pos != std::string::npos) {
                base_filename = base_filename.substr(path_pos + 1);
            }
            size_t file_hash = std::hash<std::string>{}(base_filename);
            module.id = module.name + std::to_string(file_hash).substr(0, 4);

            ParseModule(token_index, module);
            NextToken(token_index);
        }
        else if (tok.type == TokenType::COMMENT_DOC_MULTI || tok.type == TokenType::COMMENT_DOC_SINGLE) {
            std::vector<Comment_block> blocks =  ParseCommentText(tok.value);
            size_t block_count = blocks.size();
            for (int i = 0; i < block_count; i++) {
                module.comments.push_back(blocks[i]);
            }
            NextToken(token_index);
        }
        else if (tok.type == TokenType::KW_ENDMODULE) {
            modules.push_back(module);
            NextToken(token_index);
        }
        else {
            NextToken(token_index);
        }
    }

    return modules;
}