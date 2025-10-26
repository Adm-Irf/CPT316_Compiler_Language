#include "../include/parser.hpp"
#include <iostream>
#include <queue>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

Parser::Parser(const std::vector<Token> &toks)
    : tokens(toks), pos(0) {}

// Single reporter: only the FIRST error is printed
void Parser::reportError(const std::string &msg) {
    if (!errorOccurred) {
        std::cerr << "Syntax error: " << msg << std::endl;
        errorOccurred = true;
    }
}

// <stmt> ::= IDENTIFIER '=' <expr> [';']
bool Parser::parse() {
    if (tokens.empty()) {
        reportError("empty input.");
        return false;
    }

    root = parseStatement();
    if (errorOccurred || !root) return false;

    // Allow a single optional ';' at the end
    if (pos < tokens.size() && tokens[pos].value == ";") {
        ++pos;
    }

    // Anything left after a valid statement is an error
    if (pos < tokens.size()) {
        const auto &t = tokens[pos];
        reportError(
            "extra tokens after valid statement near '" + t.value +
            "' at index " + std::to_string(t.start_pos));
        return false;
    }
    return true;
}

Parser::Node *Parser::parseStatement() {
    if (errorOccurred) return nullptr;

    // IDENTIFIER
    if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
        if (pos < tokens.size()) {
            reportError("statement must start with an identifier, got '" +
                        tokens[pos].value + "' at index " +
                        std::to_string(tokens[pos].start_pos));
        } else {
            reportError("statement must start with an identifier.");
        }
        return nullptr;
    }

    Node *left = new Node(tokens[pos].value, TokenType::IDENTIFIER);
    ++pos;

    // '='
    if (pos >= tokens.size() || tokens[pos].value != "=") {
        if (pos < tokens.size()) {
            reportError("missing '=' after identifier before '" +
                        tokens[pos].value + "' at index " +
                        std::to_string(tokens[pos].start_pos));
        } else {
            reportError("missing '=' after identifier.");
        }
        return nullptr;
    }
    ++pos;

    // <expr>
    Node *right = parseExpr();
    if (!right) return nullptr;

    return new Node("=", TokenType::ASSIGNMENT, left, right);
}

Parser::Node *Parser::parseExpr() {
    if (errorOccurred) return nullptr;

    Node *left = parseTerm();
    if (!left) return nullptr;

    while (!errorOccurred && pos < tokens.size() &&
           (tokens[pos].value == "+" || tokens[pos].value == "-")) {
        std::string op = tokens[pos].value;
        ++pos;

        Node *right = parseTerm();
        if (!right) return nullptr;

        left = new Node(op, TokenType::OPERATOR, left, right);
    }

    // If next token is a factor-start, operator is missing
    if (!errorOccurred && pos < tokens.size()) {
        const auto &t = tokens[pos];
        bool looksLikeFactor =
            (t.type == TokenType::IDENTIFIER) ||
            (t.type == TokenType::NUMBER) ||
            (t.value == "(");

        if (looksLikeFactor) {
            reportError("missing operator before '" + t.value +
                        "' at index " + std::to_string(t.start_pos));
            return nullptr;
        }
    }

    return left;
}

Parser::Node *Parser::parseTerm() {
    if (errorOccurred) return nullptr;

    Node *left = parseFactor();
    if (!left) return nullptr;

    while (!errorOccurred && pos < tokens.size() &&
           (tokens[pos].value == "*" || tokens[pos].value == "/")) {
        std::string op = tokens[pos].value;
        ++pos;

        Node *right = parseFactor();
        if (!right) return nullptr;

        left = new Node(op, TokenType::OPERATOR, left, right);
    }

    // If next token is a factor-start, operator is missing
    if (!errorOccurred && pos < tokens.size()) {
        const auto &t = tokens[pos];
        bool looksLikeFactor =
            (t.type == TokenType::IDENTIFIER) ||
            (t.type == TokenType::NUMBER) ||
            (t.value == "(");

        if (looksLikeFactor) {
            reportError("missing operator before '" + t.value +
                        "' at index " + std::to_string(t.start_pos));
            return nullptr;
        }
    }

    return left;
}

Parser::Node *Parser::parseFactor() {
    if (errorOccurred) return nullptr;
    if (pos >= tokens.size()) {
        reportError("unexpected end of expression.");
        return nullptr;
    }

    const auto &t = tokens[pos];

    if (t.type == TokenType::IDENTIFIER || t.type == TokenType::NUMBER) {
        ++pos;
        return new Node(t.value, t.type);
    }

    if (t.value == "(") {
        ++pos; // consume '('
        Node *expr = parseExpr();
        if (!expr) return nullptr;

        if (pos >= tokens.size() || tokens[pos].value != ")") {
            if (pos < tokens.size()) {
                reportError("missing closing parenthesis before '" +
                            tokens[pos].value + "' at index " +
                            std::to_string(tokens[pos].start_pos));
            } else {
                reportError("missing closing parenthesis.");
            }
            return nullptr;
        }
        ++pos; // consume ')'
        return expr;
    }

    reportError("unexpected token '" + t.value +
                "' at index " + std::to_string(t.start_pos));
    return nullptr;
}

int Parser::getTreeHeight(const Node* node) {
    if (!node) return 0;
    return 1 + std::max(getTreeHeight(node->left), getTreeHeight(node->right));
}

// ---------- pretty printer (unchanged) ----------
static std::string classifyToken(const std::string& val)
{
    if (val == "=") return "(ASSIGN)";
    if (val == "+" || val == "-" || val == "*" || val == "/") return "(OP)";
    if (val == "(" || val == ")") return "(PAREN)";
    if (val == ";") return "(END)";
    if (std::isalpha(val[0])) return "(ID)";
    if (std::isdigit(val[0])) return "(NUM)";
    return "(UNK)";
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

    if (node->left) {
        int childCol = col - gap;
        grid[row + 1][col - gap / 2] = '/';
        renderTreeLabeled(node->left, grid, row + 2, childCol, width / 2);
    }

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
