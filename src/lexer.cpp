#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string &text) : input(text), pos(0) {}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (pos < input.size())
    {
        char c = input[pos];

        if (std::isspace(c))
        {
            pos++;
            continue;
        }

        if (std::islower(c))
        { // Identifier
            std::string id(1, c);
            pos++;
            tokens.push_back(Token(TokenType::IDENTIFIER, id));
        }
        else if (std::isdigit(c))
        { // Number
            std::string num;
            while (pos < input.size() && std::isdigit(input[pos]))
                num += input[pos++];
            tokens.push_back(Token(TokenType::NUMBER, num));
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        { // Operator
            tokens.push_back(Token(TokenType::OPERATOR, std::string(1, c)));
            pos++;
        }
        else if (c == '=')
        { // Assignment
            tokens.push_back(Token(TokenType::ASSIGNMENT, std::string(1, c)));
            pos++;
        }
        else if (c == '(')
        {
            tokens.push_back(Token(TokenType::LEFT_PAREN, "("));
            pos++;
        }
        else if (c == ')')
        {
            tokens.push_back(Token(TokenType::RIGHT_PAREN, ")"));
            pos++;
        }
        else if (c == ';')
        {
            tokens.push_back(Token(TokenType::STATEMENT_TERMINATOR, ";"));
            pos++;
        }

        else
        { // Invalid Case
            tokens.push_back(Token(TokenType::INVALID, std::string(1, c)));
            pos++;
        }
    }

    return tokens;
}
