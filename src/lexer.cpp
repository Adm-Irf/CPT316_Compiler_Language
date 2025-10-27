#include "../include/lexer.hpp"     // Reference to Class header
#include <iostream>
#include <string>
#include <cctype>
#include <map>

// Constructor Lexer (Initialize counter to 0/start)
Lexer::Lexer(const std::string &text) : input(text), pos(0), IDENTIFIER(0), NUMBER(0), OPERATOR(0), ASSIGNMENT(0), PARENTHESES(0), STATEMENT_TERMINATOR(0), INVALID(0) {}

// Tokenize : Scan input string & return vector token (9 Cases)
std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    // Loop through input string one by one (Almost like a pointer)
    while (pos < input.size())
    {
        char c = input[pos];

        if (std::isspace(c))    // 1. Skip Whitespace
        {
            pos++;
            continue;
        }

        if (std::islower(c))    // 2. identifier (All Lowercase)
        { 
            std::string id(1, c);
            tokens.emplace_back(TokenType::IDENTIFIER, id, pos);
            pos++;
            IDENTIFIER++;
        }
        else if (std::isdigit(c))   // 3. Number (Integer but cannot have space in between)
        { 
            size_t start_pos = pos;
            std::string num;
            while (pos < input.size() && std::isdigit(input[pos]))
            {
                num += input[pos++];
            }

            tokens.emplace_back(TokenType::NUMBER, num, start_pos);
            NUMBER++;
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')  // 4. Operator
        {
            tokens.emplace_back(TokenType::OPERATOR, std::string(1, c), pos);
            pos++;
            OPERATOR++;
        }
        else if (c == '=')  // 5. Assignment ("=")
        {
            tokens.emplace_back(TokenType::ASSIGNMENT, std::string(1, c), pos);
            pos++;
            ASSIGNMENT++;
        }
        else if (c == '(')  // 6. left Parenthesis
        { 
            tokens.emplace_back(TokenType::LEFT_PAREN, "(", pos);
            pos++;
            PARENTHESES++;
        }
        else if (c == ')')  // 7. Right Parenthesis
        { 
            tokens.emplace_back(TokenType::RIGHT_PAREN, ")", pos);
            pos++;
            PARENTHESES++;
        }
        else if (c == ';')  // 8. Statement terminator
        {
            tokens.emplace_back(TokenType::STATEMENT_TERMINATOR, ";", pos);
            pos++;
            STATEMENT_TERMINATOR++;
        }
        else    // 9. Invalid Token
        {
            tokens.emplace_back(TokenType::INVALID, std::string(1, c), pos);
            pos++;
            INVALID++;
        }
    }

    // Print Lexical Error (Case 9 : Invalid Token) found
    for (const auto &t : tokens) 
    {
        if (t.type == TokenType::INVALID) {
            std::cerr << "Lexical error: invalid token '"
                    << t.value << "' at index " << t.start_pos << std::endl;
        }
    }
    return tokens;
}

// Print the summary of Token Count
void Lexer::summarize()
{
    // Vector counts
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

        // Avoid Printing Extra Token category if non-exist/ Only print when exist (avoid printing all)
        if (count > 0)
            std::cout << name << ": " << count << std::endl;
    }
}