#include "../include/lexer.hpp"
#include <iostream>
#include <string>
#include <cctype>
#include <map>

// constructor
Lexer::Lexer(const std::string &text) : input(text), pos(0), IDENTIFIER(0), NUMBER(0), OPERATOR(0), ASSIGNMENT(0), PARENTHESES(0), STATEMENT_TERMINATOR(0), INVALID(0) {}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (pos < input.size())
    {
        char c = input[pos];

        if (std::isspace(c)) // continue to next token when whitespace is found
        {
            pos++;
            continue;
        }

        if (std::islower(c))
        { // Identifier
            std::string id(1, c);
            tokens.emplace_back(TokenType::IDENTIFIER, id, pos);
            pos++;
            IDENTIFIER++; // track count of identifier token type
        }
        else if (std::isdigit(c))
        { // Number
            size_t start_pos = pos;
            std::string num;
            while (pos < input.size() && std::isdigit(input[pos]))
            {
                num += input[pos++];
            }

            tokens.emplace_back(TokenType::NUMBER, num, start_pos);
            NUMBER++; // track count of number token type
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        { // Operator
            tokens.emplace_back(TokenType::OPERATOR, std::string(1, c), pos);
            pos++;
            OPERATOR++; // track count of operator token type
        }
        else if (c == '=')
        { // Assignment
            tokens.emplace_back(TokenType::ASSIGNMENT, std::string(1, c), pos);
            pos++;
            ASSIGNMENT++; // track count of assignment token type
        }
        else if (c == '(')
        { // Left Parentheses
            tokens.emplace_back(TokenType::LEFT_PAREN, "(", pos);
            pos++;
            PARENTHESES++; // track count of parentheses token type
        }
        else if (c == ')')
        { // Right Parentheses
            tokens.emplace_back(TokenType::RIGHT_PAREN, ")", pos);
            pos++;
            PARENTHESES++; // track count of parentheses token type
        }
        else if (c == ';')
        { // Statement Terminator
            tokens.emplace_back(TokenType::STATEMENT_TERMINATOR, ";", pos);
            pos++;
            STATEMENT_TERMINATOR++; // track count of statement terminator token type
        }

        else
        { // Invalid Case
            tokens.emplace_back(TokenType::INVALID, std::string(1, c), pos);
            pos++;
            INVALID++; // track count of invalid tokens
        }
    }

    for (const auto &t : tokens) 
    {
        if (t.type == TokenType::INVALID) {
            std::cerr << "Lexical error: invalid token '"
                    << t.value << "' at index " << t.start_pos << std::endl;
        }
    }



    return tokens;
}

void Lexer::summarize()
{
    std::vector<std::pair<std::string, int>> tokens = {
        {"Identifier", IDENTIFIER},
        {"Number", NUMBER},
        {"Operator", OPERATOR},
        {"Assignment", ASSIGNMENT},
        {"Parentheses", PARENTHESES},
        {"Statement Terminator", STATEMENT_TERMINATOR},
        {"Invalid", INVALID}};

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const std::string &name = tokens[i].first;
        int count = tokens[i].second;

        if (count > 0)
            std::cout << name << ": " << count << std::endl;
    }
}