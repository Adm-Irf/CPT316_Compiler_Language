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
    Token(TokenType t, std::string v) : type(t), value(std::move(v)) {}
};
