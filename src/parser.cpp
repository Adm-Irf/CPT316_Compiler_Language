#include "../include/parser.hpp"
#include <iostream>

Parser::Parser(const std::vector<Token> &toks) : tokens(toks), pos(0) {}

bool Parser::parse()
{
    // Extremely simplified grammar: E → ID = (ID|NUM) [OP (ID|NUM)]*
    if (tokens.empty())
        return false;
    if (tokens[pos].type != TokenType::IDENTIFIER)
        return false;
    pos++;

    if (pos >= tokens.size() || tokens[pos].value != "=")
        return false;
    pos++;

    if (pos >= tokens.size() ||
        (tokens[pos].type != TokenType::IDENTIFIER && tokens[pos].type != TokenType::NUMBER))
        return false;

    // Optional: handle trailing operator sequences
    while (++pos < tokens.size())
    {
        if (tokens[pos].type == TokenType::OPERATOR)
            continue;
        if (tokens[pos].type == TokenType::INVALID)
            return false;
    }

    return true;
}

void Parser::printSyntaxTree()
{
    std::cout << "(Assign\n  (Left: ID)\n  (Right: Expression))\n" << std::endl;
}
