#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/tree_draw.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <windows.h> // ✅ needed for SetForegroundWindow

int main()
{
    std::string input;
    std::cout << "==================================================\n";
    std::cout << "      Welcome to the COMPY Language Compiler\n";
    std::cout << "==================================================\n";

    while (true)
    {
        std::cout << "\nPlease enter an expression. (Example: x = 2 + 4;)\n";
        std::cout << "Type 'exit' to quit the program.\n";
        std::cout << "Enter here: ";

        // 🔧 Re-focus console before input (to fix typing issue)
        HWND console = GetConsoleWindow();
        SetForegroundWindow(console);

        std::getline(std::cin, input);

        if (input == "exit")
            break;

        // A. Lexical Analysis
        Lexer lexer(input);
        std::vector<Token> tokens = lexer.tokenize();

        // B. Syntax Analysis
        Parser parser(tokens);
        bool success = parser.parse();

        // C. Summary (always print)
        std::cout << "---------------------------------------\n";
        std::cout << "             Token Summary\n";
        std::cout << "---------------------------------------\n";
        lexer.summarize();
        std::cout << std::endl;

        // D. Output
        if (success && !parser.hasErrors())
        {
            std::cout << "---> Valid syntax.\n";

            // ✅ Only now open graphics window (so it won't block typing)
            TreeDraw::initCanvas();
            TreeDraw::drawParseTree(parser.getRoot());
            TreeDraw::closeCanvas();
        }
        else
        {
            std::cout << "Errors found:\n";
            parser.printErrors();
        }
    }

    std::cout << "\nThank you for using COMPY Compiler!\n";
    return 0;
}
