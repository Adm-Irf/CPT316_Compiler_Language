#include "lexer.hpp"
#include <cctype>

// constructor
Lexer::Lexer(const std::string &text) : input(text), pos(0), IDENTIFIER(0), NUMBER(0), OPERATOR(0), ASSIGNMENT(0), PARENTHESES(0), STATEMENT_TERMINATOR(0), INVALID(0) {}

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
            IDENTIFIER++; // track count of identifier token type
        }
        else if (std::isdigit(c))
        { // Number
            std::string num;
            while (pos < input.size() && std::isdigit(input[pos]))
                num += input[pos++];
            tokens.push_back(Token(TokenType::NUMBER, num));
            NUMBER++; // track count of number token type
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        { // Operator
            tokens.push_back(Token(TokenType::OPERATOR, std::string(1, c)));
            pos++;
            OPERATOR++; // track count of operator token type
        }
        else if (c == '=')
        { // Assignment
            tokens.push_back(Token(TokenType::ASSIGNMENT, std::string(1, c)));
            pos++;
            ASSIGNMENT++; // track count of assignment token type
        }
        else if (c == '(')
        { // Left Parentheses
            tokens.push_back(Token(TokenType::LEFT_PAREN, "("));
            pos++;
            PARENTHESES++; // track count of parentheses token type
        }
        else if (c == ')')
        { // Right Parentheses
            tokens.push_back(Token(TokenType::RIGHT_PAREN, ")"));
            pos++;
            PARENTHESES++; // track count of parentheses token type
        }
        else if (c == ';')
        { // Statement Terminator
            tokens.push_back(Token(TokenType::STATEMENT_TERMINATOR, ";"));
            pos++;
            STATEMENT_TERMINATOR++; // track count of statement terminator token type
        }

        else
        { // Invalid Case
            tokens.push_back(Token(TokenType::INVALID, std::string(1, c)));
            pos++;
            INVALID++; // track count of invalid tokens
        }
    }

    return tokens;
}
