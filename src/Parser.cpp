#include "Parser.h"
#include <iostream>
#include "Colors.hpp"

Parser* GetParser(const std::string& filename) {
    Parser* parser = nullptr;

    // Checking if a file is in SystemVerilog format
    parser = new Parser_SystemVerilog;
    if (parser->IsMyFormat(filename)) {
        std::cout << FORMAT_INFO "SystemVerilog format detected for file '" << filename << "'\n";
        return parser;
    }
    delete parser;

    // Checking if a file is in VHDL format
    parser = new Parser_VHDL;
    if (parser->IsMyFormat(filename)) {
        std::cout << FORMAT_INFO "VHDL format detected for file '" << filename << "'\n";
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

            // Each language decides for itself whether it is a comment
            if (c == '/' || c == '-') {
                if (ReadComment(line, col)) 
                    continue;
            }

            if (std::isalpha(c) || c == '_') {
                ReadIdentifier(line, col);
                continue;
            }
            if (std::isdigit(c)) {
                ReadNumber(line, col);
                continue;
            }
            if (c == '"') {
                ReadString(line, col);
                continue;
            }

            switch (c) {
            case '=': tokens.push_back(CreateToken(TokenType::OP_ASSIGN, "=", "=", line, col)); col++; break;
            case '[': tokens.push_back(CreateToken(TokenType::OP_LBRACKET, "[", "[", line, col)); col++; break;
            case ']': tokens.push_back(CreateToken(TokenType::OP_RBRACKET, "]", "]", line, col)); col++; break;
            case '(': tokens.push_back(CreateToken(TokenType::OP_LPAREN, "(", "(", line, col)); col++; break;
            case ')': tokens.push_back(CreateToken(TokenType::OP_RPAREN, ")", ")", line, col)); col++; break;
            case ';': tokens.push_back(CreateToken(TokenType::OP_SEMICOLON, ";", ";", line, col)); col++; break;
            case ',': tokens.push_back(CreateToken(TokenType::OP_COMMA, ",", ",", line, col)); col++; break;
            case '#': tokens.push_back(CreateToken(TokenType::OP_HASH, "#", "#", line, col)); col++; break;
            case '-': tokens.push_back(CreateToken(TokenType::OP_MINUS, "-", "-", line, col)); col++; break;
            case '+': tokens.push_back(CreateToken(TokenType::OP_PLUS, "+", "+", line, col)); col++; break;
            case ':': tokens.push_back(CreateToken(TokenType::OP_COLON, ":", ":", line, col)); col++; break;
            default: col++; break;
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

    return Token(type, value, lexeme, line + 1, col + 1);  // 1-based for parser/user
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
    col++;  // Skip the opening quotation mark

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

    // Remove brackets, commas, and semicolons
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

// String processing
std::vector<Comment_block> Parser::ParseCommentText(std::string comment_text) {
    std::vector<Comment_block> blocks;

    if (comment_text.empty()) return blocks;

    std::string current_tag;
    std::string current_text;

    while (!comment_text.empty()) {
        size_t next_tag_pos = std::string::npos;
        std::string next_tag;

        for (const auto& tag : tags) {
            size_t found = comment_text.find(tag);
            if (found != std::string::npos) {
                if (next_tag_pos == std::string::npos || found < next_tag_pos) {
                    next_tag_pos = found;
                    next_tag = tag;
                }
            }
        }

        if (next_tag_pos != std::string::npos) {
            // Save the previous block if there is one.
            if (!current_tag.empty() && !current_text.empty()) {
                Comment_block block;
                block.tag = current_tag;
                block.text = Trim(current_text);
                blocks.push_back(block);
            }
            else if (current_tag.empty() && next_tag_pos > 0) {
                std::string before = Trim(comment_text.substr(0, next_tag_pos));
                if (!before.empty()) {
                    Comment_block block;
                    block.tag = "";
                    block.text = Trim(before);
                    blocks.push_back(block);
                }
            }

            current_tag = next_tag;
            current_text = comment_text.substr(next_tag_pos + next_tag.length());
            comment_text = "";

            // Checking if there are more tags in the current text
            for (const auto& tag : tags) {
                size_t found = current_text.find(tag);
                if (found != std::string::npos) {
                    comment_text = current_text.substr(found);
                    current_text = current_text.substr(0, found);
                    break;
                }
            }
        }
        else {
            // There are no more tags
            if (!current_tag.empty()) {
                current_text += (current_text.empty() ? "" : " ") + comment_text;
            }
            else if (!comment_text.empty()) {
                Comment_block block;
                block.tag = "";
                block.text = Trim(comment_text);
                blocks.push_back(block);
            }
            break;
        }
    }

    // Save the last block
    if (!current_tag.empty() && !current_text.empty()) {
        Comment_block block;
        block.tag = current_tag;
        block.text = Trim(current_text);
        blocks.push_back(block);
    }

    return blocks;
}


void FreeParser(Parser** parser) {
    delete (*parser);
    (*parser) = nullptr;
}
