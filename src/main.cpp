#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include <iostream>
#include <vector>

int main()
{
    std::string input;
    std::cout << "Enter expression: ";
    std::getline(std::cin, input);

    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    if (parser.parse())
    {
        std::cout << "Valid syntax.\n";
        parser.printSyntaxTree();
        lexer.summarize(); // Print summary which is tokens types + the number of time they appeared in the statement.
    }
    else
    {
        std::cerr << "Syntax error.\n";
    }
}
