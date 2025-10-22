#include "lexer.hpp"
#include <cctype>

// constructor
Lexer::Lexer(const std::string &text) : input(text), pos(0), line(0), column(0), IDENTIFIER(0), NUMBER(0), OPERATOR(0), ASSIGNMENT(0), PARENTHESES(0), STATEMENT_TERMINATOR(0), INVALID(0) {}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (pos < input.size())
    {
        char c = input[pos];

        if (c == '\n')
        {
            line++;
            pos++;
            column = 0;
            continue;
        }
        else if (std::isspace(c)) // continue to next token when whitespace is found
        {
            column++;
            pos++;
            continue;
        }

        if (std::islower(c))
        { // Identifier
            std::string id(1, c);
            tokens.emplace_back(TokenType::IDENTIFIER, id, pos);
            pos++;
            column++;
            IDENTIFIER++; // track count of identifier token type
        }
        else if (std::isdigit(c))
        { // Number
            size_t start_pos = pos;
            std::string num;
            while (pos < input.size() && std::isdigit(input[pos]))
            {
                num += input[pos++];
                column++;
            }

            tokens.emplace_back(TokenType::NUMBER, num, start_pos);
            NUMBER++; // track count of number token type
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        { // Operator
            tokens.emplace_back(TokenType::OPERATOR, std::string(1, c), pos);
            pos++;
            column++;
            OPERATOR++; // track count of operator token type
        }
        else if (c == '=')
        { // Assignment
            tokens.emplace_back(TokenType::ASSIGNMENT, std::string(1, c), pos);
            pos++;
            column++;
            ASSIGNMENT++; // track count of assignment token type
        }
        else if (c == '(')
        { // Left Parentheses
            tokens.emplace_back(TokenType::LEFT_PAREN, "(", pos);
            pos++;
            column++;
            PARENTHESES++; // track count of parentheses token type
        }
        else if (c == ')')
        { // Right Parentheses
            tokens.emplace_back(TokenType::RIGHT_PAREN, ")", pos);
            pos++;
            column++;
            PARENTHESES++; // track count of parentheses token type
        }
        else if (c == ';')
        { // Statement Terminator
            tokens.emplace_back(TokenType::STATEMENT_TERMINATOR, ";", pos);
            pos++;
            column++;
            STATEMENT_TERMINATOR++; // track count of statement terminator token type
        }

        else
        { // Invalid Case
            tokens.emplace_back(TokenType::INVALID, std::string(1, c), pos);
            pos++;
            column++;
            INVALID++; // track count of invalid tokens
        }
    }

    return tokens;
}
