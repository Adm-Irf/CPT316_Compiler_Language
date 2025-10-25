#include "../include/parser.hpp"
#include <iostream>
#include <queue>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;


Parser::Parser(const std::vector<Token> &toks) : tokens(toks), pos(0) {}

// for COMPY language the syntactic granularity is as follows:
// <stmt> → <expr> → <term> → <factor>

bool Parser::parse()
{
    root = parseStatement(); // build tree
    return root != nullptr;
}

Parser::Node *Parser::parseStatement()
{
    if (tokens[pos].type != TokenType::IDENTIFIER)
        return nullptr;
    Node *left = new Node(tokens[pos].value, TokenType::IDENTIFIER);
    pos++;

    if (tokens[pos].value != "=")
        return nullptr;
    pos++;

    Node *right = parseExpr(); // recursively parse expression
    if (!right)
        return nullptr;

    return new Node("=", TokenType::ASSIGNMENT, left, right);
}

Parser::Node *Parser::parseExpr()
{
    Node *left = parseTerm();
    while (pos < tokens.size() &&
           (tokens[pos].value == "+" || tokens[pos].value == "-"))
    {
        std::string op = tokens[pos].value;
        pos++;
        Node *right = parseTerm();
        left = new Node(op, TokenType::OPERATOR, left, right);
    }
    return left;
}

Parser::Node *Parser::parseTerm()
{
    Node *left = parseFactor();
    while (pos < tokens.size() &&
           (tokens[pos].value == "*" || tokens[pos].value == "/"))
    {
        std::string op = tokens[pos].value;
        pos++;
        Node *right = parseFactor();
        left = new Node(op, TokenType::OPERATOR, left, right);
    }
    return left;
}

Parser::Node *Parser::parseFactor()
{
    if (tokens[pos].type == TokenType::IDENTIFIER ||
        tokens[pos].type == TokenType::NUMBER)
    {
        return new Node(tokens[pos++].value, tokens[pos - 1].type);
    }
    else if (tokens[pos].value == "(")
    {
        pos++;
        Node *expr = parseExpr();
        if (tokens[pos].value != ")")
            return nullptr;
        pos++;
        return expr;
    }
    return nullptr;
}



int Parser::getTreeHeight(const Node* node)
{
    if (!node) return 0;
    return 1 + std::max(getTreeHeight(node->left), getTreeHeight(node->right));
}


/// ----------------------------------------------------------
// BEAUTIFUL TREE WITH TOKEN TYPE LABELS
// ----------------------------------------------------------

static std::string classifyToken(const std::string& val)
{
    if (val == "=") return "(ASSIGN)";
    if (val == "+" || val == "-" || val == "*" || val == "/") return "(OP)";
    if (val == "(" || val == ")") return "(PAREN)";
    if (val == ";") return "(END)";
    if (std::isalpha(val[0])) return "(ID)";
    if (std::isdigit(val[0])) return "(NUM)";
    return "(UNK)"; // fallback
}

static int measureWidth(const Parser::Node* node)
{
    if (!node) return 0;
    std::string label = node->value + classifyToken(node->value);
    int w = (int)label.size();
    int left = measureWidth(node->left);
    int right = measureWidth(node->right);
    if (!node->left && !node->right) return w;
    return std::max(4, left + right + 6);
}

static void renderTreeLabeled(const Parser::Node* node,
                              std::vector<std::string>& grid,
                              int row, int col, int width)
{
    if (!node) return;

    std::string label = node->value + classifyToken(node->value);
    int valWidth = (int)label.size();
    int start = col - valWidth / 2;

    // Draw node with label
    for (int i = 0; i < valWidth; ++i) {
        int x = start + i;
        if (row >= 0 && row < (int)grid.size() &&
            x >= 0 && x < (int)grid[row].size())
            grid[row][x] = label[i];
    }

    if (!node->left && !node->right)
        return;

    int leftW  = measureWidth(node->left);
    int rightW = measureWidth(node->right);
    int gap = std::max(6, (leftW + rightW) / 4 + 2);

    // Left child + connector
    if (node->left) {
        int childCol = col - gap;
        grid[row + 1][col - gap / 2] = '/';
        renderTreeLabeled(node->left, grid, row + 2, childCol, width / 2);
    }

    // Right child + connector
    if (node->right) {
        int childCol = col + gap;
        grid[row + 1][col + gap / 2] = '\\';
        renderTreeLabeled(node->right, grid, row + 2, childCol, width / 2);
    }
}

void Parser::printSyntaxTree()
{
    if (!root) {
        std::cout << "Syntax tree is empty.\n";
        return;
    }

    std::cout << "\n🌳 Syntax Tree (Annotated)\n\n";

    int height = getTreeHeight(root);
    int rows   = height * 2;
    int width  = std::max(100, measureWidth(root) * 2);

    std::vector<std::string> grid(rows, std::string(width, ' '));

    renderTreeLabeled(root, grid, 0, width / 2, width);

    for (auto& line : grid) {
        int end = (int)line.find_last_not_of(' ');
        if (end != std::string::npos)
            std::cout << line.substr(0, end + 1) << "\n";
    }
    std::cout << std::endl;
}
