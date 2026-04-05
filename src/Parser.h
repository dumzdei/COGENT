#pragma once
#include "DataTypes.h"
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

class Parser {
protected:
    std::vector<std::string> lines;
    std::string current_filename;
    std::vector<Token> tokens;
    std::vector<std::string> tags =
    { "@brief" , "@todo" , "@description" , "@note" , "@warning" ,
      "@error" , "@author" , "@date" , "@example" , "@status" , "@top" };

    // ========== LEXICAL ANALYSIS ==========
    void Tokenize();
    Token CreateToken(TokenType type, const std::string& value,
        const std::string& lexeme, size_t line, size_t col);
    TokenType KeywordToTokenType(const std::string& keyword);
    void SkipWhitespace(size_t& line, size_t& col);
    void ReadComment(size_t& line, size_t& col);
    void ReadIdentifier(size_t& line, size_t& col);
    void ReadNumber(size_t& line, size_t& col);
    void ReadString(size_t& line, size_t& col);

    // ========== SYNTACTIC ANALYSIS ==========
    virtual std::vector<Module> ParseFromTokens() = 0;

    // Вспомогательные функции
    std::string Trim(const std::string& source_line);
    std::string CleanToken(std::string& str);
    std::string ExtractTag(std::string& text);

    bool IsAtToken(size_t index, TokenType type);
    bool IsAtKeyword(size_t index, const std::string& keyword);
    Token& CurrentToken(size_t index);
    Token& NextToken(size_t& index);

    std::vector<Comment_block> ParseCommentText(const std::string comment_text);


    bool LoadFile(const std::string& filename);
public:
    virtual bool IsMyFormat(const std::string& filename) = 0;
    std::vector<Module> Parse(const std::string& filename);
    virtual ~Parser() = default;
};

Parser* GetParser(const std::string& fileName);
void FreeParser(Parser **parser);

class Parser_SystemVerilog : public Parser {
private:
    // storing type aliases: type name -> full definition
    std::unordered_map<std::string, std::string> known_types;

    std::vector<Module> ParseFromTokens() override;

    bool ParseModule(size_t token_index, Module& module);
    bool ParsePortList(size_t& token_index, Module& module);
    bool ParseParameterList(size_t& token_index, Module& module);

public:
    bool IsMyFormat(const std::string& filename) override final;
};