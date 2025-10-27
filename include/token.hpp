#pragma once                // Header Guard
#include <string>

// Define Categories for token (Refer assignment Requirement)
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

// Represent a single token scanned from the source code (almost like a pointer)
struct Token
{
    TokenType type;         // Category of the current token
    std::string value;      // Current Token Value
    size_t start_pos = 0;   // Starting character index
    size_t length = 0;      // Length of token text

    // Token Constructor
    Token(TokenType t, std::string v, size_t sp) : type(t), value(std::move(v)), start_pos(sp), length(v.size()) {}
};
