#pragma once
#include <string>
#include <vector>

enum class TokenType {
    // Ключевые слова
    KW_MODULE, KW_ENDMODULE, KW_INTERFACE, KW_ENDINTERFACE,
    KW_PACKAGE, KW_ENDPACKAGE, KW_FUNCTION, KW_ENDFUNCTION,
    KW_TASK, KW_ENDTASK, KW_INPUT, KW_OUTPUT, KW_INOUT,
    KW_PARAMETER, KW_LOCALPARAM, KW_WIRE, KW_REG, KW_LOGIC,
    KW_TRI, KW_BIT, KW_SIGNED,

    // Комментарии
    COMMENT_DOC_SINGLE,    // //*
    COMMENT_DOC_MULTI,     // /** **/
    COMMENT_SINGLE,        // //
    COMMENT_MULTI,         // /* */

    // Операторы
    OP_ASSIGN,             // =
    OP_LBRACKET,           // [
    OP_RBRACKET,           // ]
    OP_LPAREN,             // (
    OP_RPAREN,             // )
    OP_SEMICOLON,          // ;
    OP_COMMA,              // ,
    OP_HASH,               // #

    // Литералы и идентификаторы
    IDENTIFIER,
    NUMBER,
    STRING,

    // Специальные
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    std::string lexeme;      // Оригинальный текст
    size_t line;             // Номер строки в файле
    size_t column;           // Позиция в строке

    Token(TokenType t = TokenType::UNKNOWN,
        const std::string& v = "",
        const std::string& l = "",
        size_t ln = 0,
        size_t col = 0)
        : type(t), value(v), lexeme(l), line(ln), column(col) {
    }
};

struct Port
{
    std::string name;
    std::string direction;
    std::string type;
    std::string description;
    std::string width;
};

struct Param
{
    std::string name;
    std::string description;
    std::string value;
    std::string type;
};

struct Comment_block
{
    std::vector<std::string> lines;
    std::string tag;
};

struct Module
{
    std::string name;
    std::string filename;
    std::string id;
    std::string description;    // Основное описание модуля
    std::vector<Port> ports;
	std::vector<Param> params;
    std::vector<Comment_block> comments;
};