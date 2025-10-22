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
    size_t start_pos = 0; // character index where the token begins
    size_t length = 0;    // number of characters in the token
    size_t line = 0;      // line number
    size_t column = 0;    // column number

    Token(TokenType t, std::string v, size_t sp, size_t ln, size_t cl) : type(t), value(std::move(v)), start_pos(sp), line(ln), column(cl), length(v.size()) {}
};
