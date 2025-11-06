#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include <iostream>
#include <vector>
#include <iomanip>

int main()
{
    system(""); // enable color for Windows CMD
    std::string input;
    int testCount = 0;

    std::cout << "\n\033[38;5;117m===========================================================\n";
    std::cout << "          Welcome to the COMPY Language Compiler\n";
    std::cout << "===========================================================\n";
    // Loop (Easy for Testing)
    while (true)
    {   
        std::cout << " \n";
        std::cout << "Please enter an expression. (Example: x = 2 + 4;)\n";
        std::cout << "Type 'exit' to quit the program.\n";
        std::cout << "Enter here: ";
        std::getline(std::cin, input);

        // For Exit command
        if (input == "exit")
        {
            break;
        }

        testCount++;
        std::cout << "\033[1;33m\n======================< TEST CASE " << testCount << " >======================\033[0m\n";

        // A. Lexing
        Lexer lexer(input);
        std::vector<Token> tokens = lexer.tokenize();

        std::cout << "\033[1;33m";
        lexer.printTokenStreamTable(tokens);

        // B. Parsing
        Parser parser(tokens);
        bool success = parser.parse();

        // C. Summary (Need to print even if fail - Requirement)
        lexer.summarize();
        std::cout << "\033[0m";
        std::cout << std::endl;

        // D. Log and Report Error
        bool hasLexicalError = lexer.hasLexicalErrors();
        bool hasSyntaxError = parser.hasErrors();

        if (hasLexicalError || hasSyntaxError)
        {
            std::cout << "\033[1;31mErrors found:\n";

            // 1. Print lexical errors first (if any)
            if (hasLexicalError)
                lexer.printLexicalErrors();

            // 2. Then print syntax errors (if any)
            if (hasSyntaxError)
                parser.printErrors();
        }
        else if (success)
        {
            std::cout << "\033[1;32m---> Valid syntax.\n";
            parser.printSyntaxTree();
            std::cout << "\033[0m";
        }

        std::cout << "\033[38;5;117m\n=========================<COMPLETE>========================\n";

    }
    return 0;
}