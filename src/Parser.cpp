#include "Parser.h"
#include <iostream>
#include "Colors.hpp"

Parser* GetParser(const std::string& filename) {
    Parser* parser = nullptr;

    // Проверяем, не является ли файл файлом формата SystemVerilog
    parser = new Parser_SystemVerilog;
    if (parser->IsMyFormat(filename)) {
        std::cout << FORMAT_INFO "SystemVerilog format detected for file '" << filename << "'\n";
        return parser;
    }
    delete parser;
    parser = nullptr;

    return nullptr;
}

bool Parser::LoadFile(const std::string& filename) {
    std::string line;
    current_filename = filename;
    lines.clear();
    tokens.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << FORMAT_ERROR "Cannot open file: " << filename << "\n";
        return false;
    }

    while (std::getline(file, line))
        lines.push_back(line);

    return true;
}

std::vector<Module> Parser::Parse(const std::string& filename) {
    if (!LoadFile(filename)) return {};
    current_filename = filename;
    Tokenize();
    return ParseFromTokens();
}

void Parser::Tokenize() {
    tokens.clear();
    size_t line = 0;

    while (line < lines.size()) {
        size_t col = 0;
        const std::string& current_line = lines[line];

        while (col < current_line.length()) {
            SkipWhitespace(line, col);
            if (col >= current_line.length()) break;

            char c = current_line[col];

            // Комментарии
            if (c == '/') {
                if (col + 1 < current_line.length()) {
                    char next = current_line[col + 1];
                    if (next == '*') {
                        // /* или /**
                        ReadComment(line, col);
                        continue;
                    }
                    else if (next == '/') {
                        // // или //*
                        ReadComment(line, col);
                        continue;
                    }
                }
            }

            // Идентификаторы и ключевые слова
            if (std::isalpha(c) || c == '_') {
                ReadIdentifier(line, col);
                continue;
            }

            // Числа
            if (std::isdigit(c)) {
                ReadNumber(line, col);
                continue;
            }

            // Строки
            if (c == '"') {
                ReadString(line, col);
                continue;
            }

            // Операторы
            switch (c) {
            case '=':
                tokens.push_back(CreateToken(TokenType::OP_ASSIGN, "=", "=", line, col));
                col++;
                break;
            case '[':
                tokens.push_back(CreateToken(TokenType::OP_LBRACKET, "[", "[", line, col));
                col++;
                break;
            case ']':
                tokens.push_back(CreateToken(TokenType::OP_RBRACKET, "]", "]", line, col));
                col++;
                break;
            case '(':
                tokens.push_back(CreateToken(TokenType::OP_LPAREN, "(", "(", line, col));
                col++;
                break;
            case ')':
                tokens.push_back(CreateToken(TokenType::OP_RPAREN, ")", ")", line, col));
                col++;
                break;
            case ';':
                tokens.push_back(CreateToken(TokenType::OP_SEMICOLON, ";", ";", line, col));
                col++;
                break;
            case ',':
                tokens.push_back(CreateToken(TokenType::OP_COMMA, ",", ",", line, col));
                col++;
                break;
            case '#':
                tokens.push_back(CreateToken(TokenType::OP_HASH, "#", "#", line, col));
                col++;
                break;
            case '-':
                tokens.push_back(CreateToken(TokenType::OP_MINUS, "-", "-", line, col));
                col++;
                break;
            case '+':
                tokens.push_back(CreateToken(TokenType::OP_PLUS, "+", "+", line, col));
                col++;
                break;
            case ':':
                tokens.push_back(CreateToken(TokenType::OP_COLON, ":", ":", line, col));
                col++;
                break;
            default:
                col++;
                break;
            }
        }

        line++;
    }

    tokens.push_back(CreateToken(TokenType::END_OF_FILE, "", "", lines.size(), 0));
}

Token Parser::CreateToken(TokenType type, const std::string& value,
    const std::string& lexeme, size_t line, size_t col) {

    if (lexeme.empty() && type != TokenType::END_OF_FILE &&
        type != TokenType::COMMENT_SINGLE && type != TokenType::COMMENT_MULTI) {
        std::cerr << FORMAT_WARNING "Empty lexeme in line " << (line + 1) << ". Character N. " << (col + 1) << "\n";
    }

    return Token(type, value, lexeme, line + 1, col + 1);  // 1-based для парсера/пользователя
}

TokenType Parser::KeywordToTokenType(const std::string& keyword) {
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
        {"byte", TokenType::DT_BYTE},
        {"bit", TokenType::DT_BIT},
        {"struct", TokenType::DT_STRUCT}
    };

    auto it = keywords.find(keyword);
    return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
}

void Parser::SkipWhitespace(size_t& line, size_t& col) {
    while (line < lines.size()) {
        while (col < lines[line].length()) {
            char c = lines[line][col];
            if (c == ' ' || c == '\t' || c == '\r') {
                col++;
            }
            else if (c == '\n') {
                line++;
                col = 0;
            }
            else {
                return;
            }
        }
        line++;
        col = 0;
    }
}

void Parser::ReadComment(size_t& line, size_t& col) {
    if (line >= lines.size()) return;

    const std::string& current_line = lines[line];
    size_t start_col = col;

    if (col + 1 >= current_line.length()) return;

    char next = current_line[col + 1];

    if (next == '*') {
        // Многострочный комментарий /* или /**
        bool is_doc = (col + 2 < current_line.length() && current_line[col + 2] == '*');
        std::string content;
        size_t content_start = col + (is_doc ? 3 : 2);

        // Первая строка
        if (content_start < current_line.length()) {
            size_t end_pos = current_line.find("*/", content_start);
            if (end_pos != std::string::npos) {
                content = current_line.substr(content_start, end_pos - content_start);
                col = end_pos + 2;
                TokenType type = is_doc ? TokenType::COMMENT_DOC_MULTI : TokenType::COMMENT_MULTI;
                tokens.push_back(CreateToken(type, content, content, line, start_col));
                return;
            }
            else {
                content = current_line.substr(content_start);
            }
        }

        // Продолжение на следующих строках
        line++;
        while (line < lines.size()) {
            size_t end_pos = lines[line].find("**/");
            if (end_pos != std::string::npos) {
                content += "\n" + lines[line].substr(0, end_pos);
                col = end_pos + 2;
                TokenType type = is_doc ? TokenType::COMMENT_DOC_MULTI : TokenType::COMMENT_MULTI;
                tokens.push_back(CreateToken(type, content, content, line, start_col));
                return;
            }
            else {
                content += "\n" + lines[line];
                line++;
            }
        }

    }
    else if (next == '/') {
        // Однострочный комментарий // или //*
        bool is_doc = (col + 2 < current_line.length() && current_line[col + 2] == '*');
        size_t content_start = col + (is_doc ? 3 : 2);
        std::string content = (content_start < current_line.length()) ?
            current_line.substr(content_start) : "";

        TokenType type = is_doc ? TokenType::COMMENT_DOC_SINGLE : TokenType::COMMENT_SINGLE;
        tokens.push_back(CreateToken(type, content, content, line, start_col));
        col = current_line.length();
    }
}

void Parser::ReadIdentifier(size_t& line, size_t& col) {
    if (line >= lines.size()) return;

    size_t start_col = col;
    std::string ident;

    while (col < lines[line].length()) {
        char c = lines[line][col];
        if (std::isalnum(c) || c == '_') {
            ident += c;
            col++;
        }
        else {
            break;
        }
    }

    TokenType type = KeywordToTokenType(ident);
    tokens.push_back(CreateToken(type, ident, ident, line, start_col));
}

void Parser::ReadNumber(size_t& line, size_t& col) {
    if (line >= lines.size()) return;

    size_t start_col = col;
    std::string num;

    while (col < lines[line].length()) {
        char c = lines[line][col];
        if (std::isalnum(c) || c == '_' || c == '\'' || c == 'x' || c == 'X' ||
            c == 'z' || c == 'Z' || c == 'b' || c == 'B' || c == 'h' || c == 'H') {
            num += c;
            col++;
        }
        else {
            break;
        }
    }

    tokens.push_back(CreateToken(TokenType::NUMBER, num, num, line, start_col));
}

void Parser::ReadString(size_t& line, size_t& col) {
    if (line >= lines.size()) return;

    size_t start_col = col;
    std::string str;
    col++;  // Пропускаем открывающую кавычку

    while (col < lines[line].length()) {
        char c = lines[line][col];
        if (c == '"') {
            col++;
            break;
        }
        else if (c == '\\' && col + 1 < lines[line].length()) {
            str += c;
            str += lines[line][col + 1];
            col += 2;
        }
        else {
            str += c;
            col++;
        }
    }

    tokens.push_back(CreateToken(TokenType::STRING, str, str, line, start_col));
}


bool Parser::IsAtToken(size_t index, TokenType type) {
    return (index < tokens.size() && tokens[index].type == type);
}

bool Parser::IsAtKeyword(size_t index, const std::string& keyword) {
    if (index >= tokens.size()) return false;
    return (tokens[index].type == TokenType::IDENTIFIER &&
        tokens[index].value == keyword);
}

Token& Parser::CurrentToken(size_t index) {
    static Token empty;
    return (index < tokens.size()) ? tokens[index] : empty;
}

Token& Parser::NextToken(size_t& index) {
    return (index < tokens.size()) ? tokens[index++] : tokens.back();
}

std::string Parser::Trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";

    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

std::string Parser::CleanToken(std::string& str)
{
    str = Trim(str);

    // Удаляем скобки, запятые, точки с запятой
    const std::string charsToRemove = "(){}[],;";

    for (char c : charsToRemove) {
        str.erase(std::remove(str.begin(), str.end(), c), str.end());
    }

    return Trim(str);
}

std::string Parser::ExtractTag(std::string& text)
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

// Обработка строки
std::vector<Comment_block> Parser::ParseCommentText(const std::string& comment_text) {
    std::vector<Comment_block> blocks;
    std::string remaining = Trim(comment_text);

    if (remaining.empty()) return blocks;

    std::string current_tag;
    std::string current_text;

    while (!remaining.empty()) {
        size_t next_tag_pos = std::string::npos;
        std::string next_tag;

        for (const auto& tag : tags) {
            size_t found = remaining.find(tag);
            if (found != std::string::npos) {
                if (next_tag_pos == std::string::npos || found < next_tag_pos) {
                    next_tag_pos = found;
                    next_tag = tag;
                }
            }
        }

        if (next_tag_pos != std::string::npos) {
            // Сохраняем предыдущий блок если есть
            if (!current_tag.empty() && !current_text.empty()) {
                Comment_block block;
                block.tag = current_tag;
                block.lines.push_back(Trim(current_text));
                blocks.push_back(block);
            }
            else if (current_tag.empty() && next_tag_pos > 0) {
                std::string before = Trim(remaining.substr(0, next_tag_pos));
                if (!before.empty()) {
                    Comment_block block;
                    block.tag = "";
                    block.lines.push_back(before);
                    blocks.push_back(block);
                }
            }

            current_tag = next_tag;
            current_text = remaining.substr(next_tag_pos + next_tag.length());
            remaining = "";

            // Проверяем, есть ли ещё теги в текущем тексте
            for (const auto& tag : tags) {
                size_t found = current_text.find(tag);
                if (found != std::string::npos) {
                    remaining = current_text.substr(found);
                    current_text = current_text.substr(0, found);
                    break;
                }
            }
        }
        else {
            // Тегов больше нет
            if (!current_tag.empty()) {
                current_text += (current_text.empty() ? "" : " ") + remaining;
            }
            else if (!remaining.empty()) {
                Comment_block block;
                block.tag = "";
                block.lines.push_back(Trim(remaining));
                blocks.push_back(block);
            }
            break;
        }
    }

    // Сохраняем последний блок
    if (!current_tag.empty() && !current_text.empty()) {
        Comment_block block;
        block.tag = current_tag;
        block.lines.push_back(Trim(current_text));
        blocks.push_back(block);
    }

    return blocks;
}

// Обработка вектора строк
std::vector<Comment_block> Parser::ParseCommentText(const std::vector<std::string>& comment_lines) {
    if (comment_lines.empty()) return {};

    std::string combined;
    for (size_t i = 0; i < comment_lines.size(); ++i) {
        if (i > 0) combined += "\n";
        combined += comment_lines[i];
    }

    return ParseCommentText(combined);
}

void FreeParser(Parser** parser) {
    delete (*parser);
    (*parser) = nullptr;
}
