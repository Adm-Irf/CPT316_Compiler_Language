#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

int main()
{
    std::string input;
    std::cout << "Enter expression: ";
    std::getline(std::cin, input);

    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    bool success = parser.parse();

    if (success && !parser.hasErrors())
    {
        std::cout << "Valid syntax.\n";
        parser.printSyntaxTree();
    }
    else
    {
        std::cout << "Errors found:\n";
        parser.printErrors();
        lexer.summarize();
    }

    std::cout << "Token Summary\n";
    lexer.summarize();
    std::cout << std::endl;

    return 0;
}
