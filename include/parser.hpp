#pragma once
#include "token.hpp"
#include <vector>

class Parser
{
private:
    const std::vector<Token> &tokens;
    size_t pos;

public:
    explicit Parser(const std::vector<Token> &toks);
    bool parse();           // returns true if grammar valid
    void printSyntaxTree(); // simple visualization
};
