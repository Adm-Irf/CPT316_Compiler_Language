#pragma once
#include "token.hpp"
#include <string>
#include <vector>

class Lexer
{
private:
    std::string input;
    size_t pos;
    size_t line;
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
};
