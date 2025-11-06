#pragma once
#include "parser.hpp"

// Simple BGI tree drawer
namespace TreeDraw {
    // Configure your canvas
    void initCanvas(int width = 1000, int height = 700);
    void closeCanvas();

    // Draw the parse tree using BGI graphics
    void drawParseTree(const Parser::Node* root);

    // Optional: export screenshot if your BGI build supports it (many don’t)
}
