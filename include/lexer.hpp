#pragma once                // Header Guard
#include "token.hpp"        // Include Token CLass
#include <string>
#include <vector>

// Lexer Class converts source code text into tokens
class Lexer
{
private:
    std::string input;      // Input string to be tokenized
    size_t pos;             // Current Index in input strings
    size_t column;
    int id;

    int IDENTIFIER;
    int NUMBER;
    int OPERATOR;
    int ASSIGNMENT;
    int PARENTHESES;
    int STATEMENT_TERMINATOR;
    int INVALID;

public:
    explicit Lexer(const std::string &text);
    std::vector<Token> tokenize();
    void summarize();
};
