#include "tree_draw.hpp"
#include <graphics.h>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>

// ---- layout helpers: compute x/y positions by in-order traversal ----
namespace {
    struct Placed {
        const Parser::Node* node;
        int x;
        int y;
    };

    const int NODE_RADIUS = 18;
    const int LEVEL_DY = 85;         // vertical spacing between levels
    const int X_GAP_MIN = 30;        // minimal horizontal gap
    const int FONTW = 8;             // approx monospace width (for center text)
    const int FONTH = 12;

    int treeHeight(const Parser::Node* n) {
        if (!n) return 0;
        return 1 + std::max(treeHeight(n->left), treeHeight(n->right));
    }

    void collectInorder(const Parser::Node* n, std::vector<const Parser::Node*>& inorder) {
        if (!n) return;
        collectInorder(n->left, inorder);
        inorder.push_back(n);
        collectInorder(n->right, inorder);
    }

    void computePositions(
        const Parser::Node* root,
        std::vector<Placed>& placed,
        int canvasW, int startY = 80
    ){
        if (!root) return;

        // 1) inorder index -> base X
        std::vector<const Parser::Node*> inorder;
        collectInorder(root, inorder);

        int cols = std::max(1, (int)inorder.size());
        int gap = std::max(X_GAP_MIN, (canvasW - 120) / cols);
        std::vector<int> baseX(cols);
        for (int i = 0; i < cols; ++i) baseX[i] = 60 + i * gap;

        // 2) assign inorder index for each node
        std::function<void(const Parser::Node*, int&, int)> assign;
        std::unordered_map<const Parser::Node*, int> idx;
        assign = [&](const Parser::Node* n, int& cur, int depth){
            if (!n) return;
            assign(n->left, cur, depth + 1);
            idx[n] = cur++;
            assign(n->right, cur, depth + 1);
        };
        int cur = 0;
        assign(root, cur, 0);

        // 3) get depth for each node
        std::function<void(const Parser::Node*, int)> walkDepth;
        std::unordered_map<const Parser::Node*, int> depth;
        walkDepth = [&](const Parser::Node* n, int d){
            if (!n) return;
            depth[n] = d;
            walkDepth(n->left, d + 1);
            walkDepth(n->right, d + 1);
        };
        walkDepth(root, 0);

        // 4) fill placed vector
        placed.clear();
        placed.reserve(inorder.size());
        for (auto* n : inorder) {
            placed.push_back({ n, baseX[idx[n]], startY + depth[n] * LEVEL_DY });
        }
    }

    std::string labelOf(const Parser::Node* n){
        if (!n) return "";
        // Reuse your classify rules: ASSIGN, OP, ID, NUM, PAREN, END
        auto &v = n->value;
        if (v == "=") return "= (ASSIGN)";
        if (v == "+" || v == "-" || v == "*" || v == "/") return v + " (OP)";
        if (!v.empty() && std::isdigit((unsigned char)v[0])) return v + " (NUM)";
        if (!v.empty() && std::isalpha((unsigned char)v[0])) return v + " (ID)";
        return v;
    }

    const Placed* findPlaced(const std::vector<Placed>& P, const Parser::Node* node){
        for (auto &p : P) if (p.node == node) return &p;
        return nullptr;
    }
}

namespace TreeDraw {

void initCanvas(int width, int height){
    int gd = DETECT, gm;
    initgraph(&gd, &gm, ""); // classic BGI init
    initwindow(width, height, "COMPY Syntax Tree");
    setbkcolor(BLACK);
    cleardevice();
    setcolor(WHITE);
}

void closeCanvas(){
    // Wait for a key so window won’t vanish immediately
    outtextxy(10, 10, (char*)"Press any key to close the tree window...");
    getch();
    closegraph();
}

void drawParseTree(const Parser::Node* root){
    if (!root){
        outtextxy(30, 40, (char*)"Empty syntax tree.");
        return;
    }

    int width = getmaxx();
    std::vector<Placed> P;
    computePositions(root, P, width);

    // Draw edges first
    setcolor(WHITE);
    for (auto &pl : P){
        auto* n = pl.node;
        if (!n) continue;
        if (n->left){
            auto *L = findPlaced(P, n->left);
            if (L) line(pl.x, pl.y, L->x, L->y);
        }
        if (n->right){
            auto *R = findPlaced(P, n->right);
            if (R) line(pl.x, pl.y, R->x, R->y);
        }
    }

    // Draw nodes on top
    for (auto &pl : P){
        setcolor(GREEN);
        circle(pl.x, pl.y, NODE_RADIUS);
        setfillstyle(SOLID_FILL, GREEN);
        floodfill(pl.x, pl.y, GREEN);

        // text
        auto label = labelOf(pl.node);
        setbkcolor(GREEN);
        setcolor(BLACK);
        int textX = pl.x - (int)label.size() * FONTW / 4; // center-ish
        int textY = pl.y - FONTH/2;
        outtextxy(textX, textY, (char*)label.c_str());
        setbkcolor(BLACK);
        setcolor(WHITE);
    }
}

} // namespace TreeDraw
