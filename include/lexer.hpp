#pragma once                // Header Guard
#include "token.hpp"        // Include Token Class
#include <string>
#include <vector>

// Lexer Class converts source code text into tokens
class Lexer
{
private:
    std::string input;      // Input string to be tokenized
    size_t pos;             // Current Index in input strings
    int id;

    // Counter of Tokens
    int IDENTIFIER;
    int NUMBER;
    int OPERATOR;
    int ASSIGNMENT;
    int PARENTHESES;
    int STATEMENT_TERMINATOR;
    int INVALID;

public:
    explicit Lexer(const std::string &text);    // Constructor (Initialize lexer input string)
    std::vector<Token> tokenize();              // Function that returns a vector of Token Object
    void summarize();                           // Function that prints the summary Tokens Count
};
