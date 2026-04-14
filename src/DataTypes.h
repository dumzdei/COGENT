#pragma once
#include <string>
#include <vector>

enum class TokenType {
    //  SystemVerilog Keywords 
    KW_MODULE, KW_ENDMODULE,
    KW_INTERFACE, KW_ENDINTERFACE,
    KW_PACKAGE, KW_ENDPACKAGE,
    KW_FUNCTION, KW_ENDFUNCTION,
    KW_TASK, KW_ENDTASK,
    KW_PARAM, KW_LOCALPARAM,
    KW_SIGNED, KW_ENUM, KW_TYPE,

    //  VHDL Keywords 
    KW_ENTITY, KW_ARCHITECTURE, KW_END,
    KW_GENERIC, KW_PORT,
    KW_CONSTANT, KW_SIGNAL, KW_PROCESS,
    KW_BEGIN, KW_DOWNTO, KW_TO,

    //  SystemVerilog Data Types
    DT_LOGIC, DT_WIRE, DT_TRI, DT_REG,
    DT_INT, DT_BYTE, DT_BIT, DT_STRUCT,

    //  VHDL Data Types
    DT_STD_LOGIC, DT_STD_LOGIC_VECTOR,
    DT_INTEGER, DT_BOOLEAN,

    //  SystemVerilog Directions
    DIR_INPUT, DIR_OUTPUT, DIR_INOUT,

    //  VHDL Directions
    DIR_IN, DIR_OUT, DIR_INOUT_VHDL,
    DIR_BUFFER, DIR_LINKAGE,

    //  Comments 
    COMMENT_DOC_SINGLE,    // //* или --*
    COMMENT_DOC_MULTI,     // /** */
    COMMENT_SINGLE,        // // или --
    COMMENT_MULTI,         // /* */

    //  Operators 
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
    OP_SLASH,              // /
    OP_STAR,               // *
    OP_INTERROGATIVE,      // ?

    //  Literals & Identifiers 
    IDENTIFIER,
    NUMBER,
    STRING,

    //  Special 
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
    bool ShowParamType = true;
    bool ShowParamDataType = false;
    std::vector<Comment_block> comments;
};