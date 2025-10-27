#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

int main()
{
    std::string input;

    // Start an infinite loop
    while (true)
    {
        std::cout << "Enter expression (or 'exit' to quit): ";
        std::getline(std::cin, input);

        // Add an exit condition
        if (input == "exit")
        {
            break; // This breaks out of the while loop
        }

        // --- All of your original code now runs inside the loop ---
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
        }

        std::cout << "Token Summary\n";
        lexer.summarize();
        std::cout << std::endl;
    }

    return 0;
}