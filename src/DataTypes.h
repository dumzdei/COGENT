#pragma once
#include <string>
#include <vector>

enum class TokenType {
    // SV Keywords
    KW_MODULE, KW_ENDMODULE, KW_INTERFACE, KW_ENDINTERFACE,
    KW_PACKAGE, KW_ENDPACKAGE, KW_FUNCTION, KW_ENDFUNCTION,
    KW_TASK, KW_ENDTASK, KW_PARAM, KW_LOCALPARAM, KW_SIGNED,
    KW_PACKED, KW_ENUM, KW_TYPE, KW_TYPEDEF,

    // VHDL Keywords
    KW_ENTITY, KW_ARCHITECTURE, KW_END, KW_GENERIC, KW_CONSTANT,
    KW_SIGNAL, KW_PROCESS, KW_BEGIN, KW_IS,

    // SV Data types
    DT_LOGIC, DT_WIRE, DT_TRI, DT_REG, DT_INT,
    DT_BYTE, DT_BIT, DT_STRUCT, DT_UNION,

    // VHDL Data types
    DT_STD_LOGIC, DT_STD_LOGIC_VECTOR, DT_INTEGER, DT_BOOLEAN,

    // SV Directions
    DIR_INPUT, DIR_OUTPUT, DIR_INOUT,

    // VHDL directions
    DT_IN, DT_OUT, DT_INOUT, DT_BUFFER, 

    // Comments
    COMMENT_DOC_SINGLE,    // //* or --*
    COMMENT_DOC_MULTI,     // /** **/
    COMMENT_SINGLE,        // // or --
    COMMENT_MULTI,         // /* */

    // Operators
    OP_ASSIGN,             // =
    OP_LBRACKET,           // [
    OP_RBRACKET,           // ]
    OP_LPAREN,             // (
    OP_RPAREN,             // )
    OP_SEMICOLON,          // ;
    OP_COMMA,              // ,
    OP_HASH,               // #
    OP_MINUS,              // -
    OP_PLUS,               // +
    OP_COLON,              // :

    // Literals and Identifiers
    IDENTIFIER,
    NUMBER,
    STRING,

    // Special
    UNKNOWN,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    std::string lexeme;      // Original text
    size_t line;             // Line number in the file
    size_t column;           // Position in line

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
    std::string data_type;
    std::string type;
};

struct Comment_block
{
    std::string text;
    std::string tag;
};

struct Module
{
    std::string name;
    std::string filename;
    std::string id;
    std::vector<Port> ports;
	std::vector<Param> params;
    std::vector<Comment_block> comments;
};