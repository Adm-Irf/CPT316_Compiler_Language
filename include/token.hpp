#pragma once
#include <string>

enum class TokenType { IDENTIFIER, NUMBER, OPERATOR, INVALID };

struct Token {
    TokenType type;
    std::string value;
    Token(TokenType t, std::string v) : type(t), value(std::move(v)) {}
};
