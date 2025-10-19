#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>
#include <vector>

int main() {
    std::string input;
    std::cout << "Enter expression: ";
    std::getline(std::cin, input);

    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    if (parser.parse()) {
        std::cout << "Valid syntax.\n";
        parser.printSyntaxTree();
    } else {
        std::cerr << "Syntax error.\n";
    }
}
