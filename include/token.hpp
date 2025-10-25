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
    Token(TokenType t, std::string v, size_t sp) : type(t), value(std::move(v)), start_pos(sp), length(v.size()) {}
};
