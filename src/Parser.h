#pragma once
#include "DataTypes.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <memory>
#include <unordered_map>

class Parser {
protected:
    std::vector<std::string> lines;
    std::vector<Token> tokens;
    std::vector<std::string> tags =
    { "@brief" , "@todo" , "@description" , "@note" , "@warning" ,
      "@error" , "@author" , "@date" , "@example" , "@status" , "@top" };

    // ========== ЛЕКСИЧЕСКИЙ АНАЛИЗ ==========
    void Tokenize();
    Token CreateToken(TokenType type, const std::string& value,
        const std::string& lexeme, size_t line, size_t col);
    TokenType KeywordToTokenType(const std::string& keyword);
    void SkipWhitespace(size_t& line, size_t& col);
    void ReadComment(size_t& line, size_t& col);
    void ReadIdentifier(size_t& line, size_t& col);
    void ReadNumber(size_t& line, size_t& col);
    void ReadString(size_t& line, size_t& col);

    // ========== СИНТАКСИЧЕСКИЙ АНАЛИЗ ==========
    virtual std::vector<Module> ParseFromTokens() = 0;

    // Вспомогательные функции
    std::string Trim(const std::string& source_line);
    std::string CleanToken(std::string& str);
    std::string ExtractTag(std::string& text);
    std::vector<Comment_block> ParseCommentText(const std::string& comment_text);
    std::vector<Comment_block> ParseCommentText(const std::vector<std::string>& comment_lines);

    // Поиск комментария перед текущей позицией
    Comment_block FindPrecedingDocComment(size_t token_index);
public:
    bool LoadFile(const std::string& fileName);
    virtual bool IsMyFormat(const std::string& fileName) = 0;
    virtual std::vector<Module> Parse(const std::string& fileName) = 0;
    virtual ~Parser() = default;
};

Parser* GetParser(const std::string& fileName);
void FreeParser(Parser **parser);

class Parser_SystemVerilog : public Parser {
private:
    // Парсинг из токенов
    std::vector<Module> ParseFromTokens() override;

    bool ParseModule(size_t& token_index, Module& module);
    bool ParsePortList(size_t& token_index, Module& module);
    bool ParseParameterList(size_t& token_index, Module& module);

    // Проверка токенов
    bool IsAtToken(size_t index, TokenType type);
    bool IsAtKeyword(size_t index, const std::string& keyword);
    Token& CurrentToken(size_t index);
    Token& NextToken(size_t& index);

public:
    bool IsMyFormat(const std::string& filename) override final;
    std::vector<Module> Parse(const std::string& source_file) override final;
};