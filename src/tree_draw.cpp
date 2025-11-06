#include "../include/tree_draw.hpp"
#include <graphics.h>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>

// ----- Layout helpers -----
namespace {
    const int NODE_RADIUS = 30;
    const int Y_GAP = 120;
    const int X_SPREAD = 100;

    struct NodePos {
        const Parser::Node* node;
        int x, y;
    };

    void layout(const Parser::Node* node, int depth, int& x,
                std::vector<NodePos>& out, int xGap = X_SPREAD) {
        if (!node) return;
        layout(node->left, depth + 1, x, out, xGap);
        int posX = x * xGap + 150;
        int posY = depth * Y_GAP + 150;
        out.push_back({ node, posX, posY });
        x++;
        layout(node->right, depth + 1, x, out, xGap);
    }

    NodePos findNode(const std::vector<NodePos>& v, const Parser::Node* n) {
        for (auto& t : v)
            if (t.node == n)
                return t;
        return { nullptr, 0, 0 };
    }
}

// ----- Drawing functions -----
namespace TreeDraw {

void initCanvas(int width, int height) {
    int gd = DETECT, gm;
    char path[] = "";
    initgraph(&gd, &gm, path);
    initwindow(width, height, "COMPY Syntax Tree");

    // Soft background and header
    setbkcolor(WHITE);
    cleardevice();

    setcolor(DARKGRAY);
    settextstyle(EUROPEAN_FONT, HORIZ_DIR, 3);
    outtextxy(40, 40, (char*)"COMPY Syntax Tree");

    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
}

void drawParseTree(const Parser::Node* root) {
    if (!root) {
        outtextxy(50, 80, (char*)"Empty syntax tree");
        return;
    }

    std::vector<NodePos> nodes;
    int x = 0;
    layout(root, 0, x, nodes);

    // Smooth connecting lines
    setcolor(LIGHTGRAY);
    setlinestyle(SOLID_LINE, 0, 2); // thicker lines
    for (auto& t : nodes) {
        const Parser::Node* n = t.node;
        if (n->left) {
            NodePos L = findNode(nodes, n->left);
            line(t.x, t.y, L.x, L.y);
        }
        if (n->right) {
            NodePos R = findNode(nodes, n->right);
            line(t.x, t.y, R.x, R.y);
        }
    }

    // Draw modern blue nodes with white bold text
    for (auto& t : nodes) {
        setcolor(LIGHTBLUE);
        setfillstyle(SOLID_FILL, LIGHTBLUE);
        fillellipse(t.x, t.y, NODE_RADIUS, NODE_RADIUS);

        // White outline for contrast
        setcolor(WHITE);
        circle(t.x, t.y, NODE_RADIUS);

        // Draw text (white, larger, clean)
        std::string val = t.node->value;
        setbkcolor(LIGHTBLUE);
        setcolor(WHITE);
        settextstyle(SANS_SERIF_FONT, HORIZ_DIR, 3); // bigger, smoother font
        int textX = t.x - (int)val.size() * 6;
        int textY = t.y - 8;
        outtextxy(textX, textY, (char*)val.c_str());
        setbkcolor(WHITE);
    }

    setcolor(DARKGRAY);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
    outtextxy(30, getmaxy() - 30, (char*)"Press any key to close...");
}

void closeCanvas() {
    getch();
    closegraph();
}

} // namespace TreeDraw
