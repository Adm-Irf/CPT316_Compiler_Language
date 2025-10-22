#pragma once
#include <string>

enum class TokenType
{
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    ASSIGNMENT,
    LEFT_PAREN,
    RIGHT_PAREN,
    STATEMENT_TERMINATOR,
    INVALID
};

struct Token
{
    TokenType type;
    std::string value;
    size_t start_pos; // character index where the token begins
    size_t length;    // number of characters in the token
    size_t line;      // line number
    size_t column;    // column number

    Token(TokenType t, std::string v) : type(t), value(std::move(v)) {}
};
