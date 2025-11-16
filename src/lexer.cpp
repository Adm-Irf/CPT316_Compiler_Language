#include "../include/lexer.hpp"     // Reference to Class header
#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>
#include <map>
#include <sstream>

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

        if (std::isalpha(c))    // 2. Identifier (lowercase single)
        { 
            size_t start_pos = pos;
            std::string letters;
            
            while (pos < input.size() && std::isalpha(input[pos]))
            {
                letters += input[pos];
                pos++;
            }
            
            if (letters.length() == 1 && std::islower(letters[0]))
            {
                tokens.emplace_back(TokenType::IDENTIFIER, letters, start_pos);
                IDENTIFIER++;
            }
            else
            {
                tokens.emplace_back(TokenType::INVALID, letters, start_pos);
                INVALID++;
                pos++;
            }
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
        else if (c == '(')  // 6. Left Parentheses
        { 
            tokens.emplace_back(TokenType::LEFT_PAREN, "(", pos);
            pos++;
            PARENTHESES++;
        }
        else if (c == ')')  // 7. Right Parentheses
        { 
            tokens.emplace_back(TokenType::RIGHT_PAREN, ")", pos);
            pos++;
            PARENTHESES++;
        }
        else if (c == ';')  // 8. Statement Terminator
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

    // Collect Lexical Errors (Case 9 : Invalid Token) found
    lexicalErrors.clear();
    for (const auto &t : tokens)
    {
        if (t.type == TokenType::INVALID)
        {
            std::stringstream ss;
            ss << "LexicalError at position " << t.start_pos
            << ": invalid character '" << t.value << "'";
            lexicalErrors.push_back(ss.str());
        }
    }
    return tokens;
}

// Print summary of Token Count
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

// Print token stream table
void Lexer::printTokenStreamTable(const std::vector<Token> &tokens)
{
    std::cout << "\n-----------------------------------------------------------\n";
    std::cout << "                   Token Stream Table\n";
    std::cout << "-----------------------------------------------------------\n";

    std::cout << std::left
              << std::setw(5)  << "No."
              << std::setw(15) << "Lexeme"
              << std::setw(25) << "Token Types"
              << std::setw(20) << "Token" << "\n";
    std::cout << "-----------------------------------------------------------\n";

    // Categorized TokenType
    int i = 1;
    for (const auto &t : tokens)
    {
        std::string tokenType, tokenFormatted;

        switch (t.type)
        {
        case TokenType::IDENTIFIER:
            tokenType = "Identifier";
            tokenFormatted = "<id, \"" + t.value + "\">";
            break;
        case TokenType::NUMBER:
            tokenType = "Number";
            tokenFormatted = "<" + t.value + ">";
            break;
        case TokenType::OPERATOR:
            tokenType = "Operator";
            tokenFormatted = "< " + t.value + " >";
            break;
        case TokenType::ASSIGNMENT:
            tokenType = "Assignment";
            tokenFormatted = "< = >";
            break;
        case TokenType::LEFT_PAREN:
            tokenType = "Left Parenthesis";
            tokenFormatted = "< ( >";
            break;
        case TokenType::RIGHT_PAREN:
            tokenType = "Right Parenthesis";
            tokenFormatted = "< ) >";
            break;
        case TokenType::STATEMENT_TERMINATOR:
            tokenType = "Statement Terminator";
            tokenFormatted = "< ; >";
            break;
        case TokenType::INVALID:
            tokenType = "Error";
            tokenFormatted = "Error";
            break;
        default:
            tokenType = "Unknown";
            tokenFormatted = "Error";
        }

        std::cout << std::left
                  << std::setw(7)  << i++
                  << std::setw(13) << t.value
                  << std::setw(25) << tokenType
                  << std::setw(20) << tokenFormatted
                  << "\n";
    }

    std::cout << "-----------------------------------------------------------\n\n";
}

// Print combined collected lexical error
void Lexer::printLexicalErrors() const
{
    if (!lexicalErrors.empty())
    {
        for (const auto &err : lexicalErrors)
            std::cout << err << std::endl;
    }
}
