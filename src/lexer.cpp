#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string& text) : input(text), pos(0) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (pos < input.size()) {
        char c = input[pos];

        if (std::isspace(c)) { pos++; continue; }

        if (std::isalpha(c)) {
            std::string id;
            while (pos < input.size() && std::isalnum(input[pos])) id += input[pos++];
            tokens.push_back(Token(TokenType::IDENTIFIER, id));
        } else if (std::isdigit(c)) {
            std::string num;
            while (pos < input.size() && std::isdigit(input[pos])) num += input[pos++];
            tokens.push_back(Token(TokenType::NUMBER, num));
        } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=') {
            tokens.push_back(Token(TokenType::OPERATOR, std::string(1, c)));
            pos++;
        } else {
            tokens.push_back(Token(TokenType::INVALID, std::string(1, c)));
            pos++;
        }
    }

    return tokens;
}
