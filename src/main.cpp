#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include "tree_draw.hpp"

int main()
{
    std::string input;
    std::cout << "==================================================\n";
    std::cout << "      Welcome to the COMPY Language Compiler\n";
    std::cout << "==================================================\n";
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

        // A. Lexing
        Lexer lexer(input);
        std::vector<Token> tokens = lexer.tokenize();

        // B. Parsing
        Parser parser(tokens);
        bool success = parser.parse();

        // C. Summary (Need to print even if fail - Requirement)
        std::cout << "---------------------------------------\n";
        std::cout << "             Token Summary\n";
        std::cout << "---------------------------------------\n";
        lexer.summarize();
        std::cout << std::endl;

        // D. Log and Report Error
        // Check if Error Persisted (Print Tree or Error - Requirement lepas summary)
        if (success && !parser.hasErrors()) {
            std::cout << "---> Valid syntax.\n";
            // draw with graphics.h
            TreeDraw::initCanvas();
            TreeDraw::drawParseTree(parser.getRoot());
            TreeDraw::closeCanvas();
        } else {
            std::cout << "Errors found:\n";
            parser.printErrors();
        }
            }
    return 0;
}