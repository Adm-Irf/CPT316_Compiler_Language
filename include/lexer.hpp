#pragma once                // Header Guard
#include "token.hpp"        // Include Token Definition
#include <string>
#include <vector>

// Lexer Class converts source code text into vector of tokens
class Lexer
{

// Private Member
private:
    std::string input;      // Input string to be Tokenized
    size_t pos;             // Current Index Position in input strings

    // Token Counter
    int IDENTIFIER;
    int NUMBER;
    int OPERATOR;
    int ASSIGNMENT;
    int PARENTHESES;
    int STATEMENT_TERMINATOR;
    int INVALID;

// Public Member
public:
    explicit Lexer(const std::string &text);    // Constructor new Lexer
    std::vector<Token> tokenize();              // Scan input & returns a vector(list) of Token Object
    void summarize();                           // Prints the summary Tokens Type Count
};
