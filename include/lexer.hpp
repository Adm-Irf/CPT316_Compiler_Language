#pragma once                // Header Guard
#include "token.hpp"        // Include Token Class (enum TokenType & struct Token)
#include <string>
#include <vector>

// Lexer Class converts source code text into tokens
class Lexer
{

// Private : Accessible only inside Lexer Class (Encapsulation)
private:
    std::string input;      // Input string to be Tokenize
    size_t pos;             // Current Index Position (Like Pointer) in input strings

    // Counter of Tokens
    int IDENTIFIER;
    int NUMBER;
    int OPERATOR;
    int ASSIGNMENT;
    int PARENTHESES;
    int STATEMENT_TERMINATOR;
    int INVALID;

public:
    explicit Lexer(const std::string &text);    // Constructor (Initialize input string & reset counter)
    std::vector<Token> tokenize();              // Scan input & returns a vector(list) of Token Object
    void summarize();                           // Prints the summary Tokens Type Count (After Tokenization)
};
