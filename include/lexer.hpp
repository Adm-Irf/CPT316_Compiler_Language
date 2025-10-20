#pragma once
#include "token.hpp"
#include <string>
#include <vector>

class Lexer
{
private:
    std::string input;
    size_t pos;

public:
    explicit Lexer(const std::string &text);
    std::vector<Token> tokenize();
};
