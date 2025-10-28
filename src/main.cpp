#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

int main()
{
    std::string input;
    std::cout << "===================================\n";
    std::cout << "    Welcome to the Mini-Compiler\n";
    std::cout << "===================================\n";

    // Loop (Easy for Testing)
    while (true)
    {
        std::cout << "Enter expression (or 'exit' to quit): ";
        std::getline(std::cin, input);

        // For Exit command
        if (input == "exit")
        {
            break;
        }

        // A. Lexing
        Lexer lexer(input);
        std::vector<Token> tokens = lexer.tokenize();

        // B. Parsing
        Parser parser(tokens);
        bool success = parser.parse();

        // C. Summary (Need to print even if fail - Requiremenr)
        std::cout << "Token Summary\n";
        lexer.summarize();
        std::cout << std::endl;

        // D. Log and Report Error
        // Check if Error Persisted (Print Tree or Error - Requirement lepas sumaary)
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
    }
    return 0;
}