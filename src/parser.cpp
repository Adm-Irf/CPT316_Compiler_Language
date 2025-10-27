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
    : tokens(toks), pos(0), errorOccurred(false) {}

static inline bool isAddOp(const std::string &s) { return s == "+" || s == "-"; }
static inline bool isMulOp(const std::string &s) { return s == "*" || s == "/"; }
static inline bool isFactorStart(const Token &t)
{
    return t.type == TokenType::IDENTIFIER ||
           t.type == TokenType::NUMBER ||
           t.value == "(";
}

static bool containsErrorNode(const Parser::Node* node)
{
    if (!node) return false;
    if (node->value == "error") return true;
    return containsErrorNode(node->left) || containsErrorNode(node->right);
}

void Parser::reportError(const std::string &msg) { reportError(msg, -1); }

void Parser::reportError(const std::string &msg, int position)
{
    std::string where = (position >= 0 ? std::to_string(position) : "end");
    std::string full = "SyntaxError at position " + where + ": " + msg;
    errorMessages.push_back(full);
    errorOccurred = true;
}

void Parser::printErrors() const
{
    for (const auto &err : errorMessages)
        std::cerr << err << std::endl;
}

bool Parser::hasErrors() const { return !errorMessages.empty(); }

bool Parser::parse()
{
    errorMessages.clear();
    errorOccurred = false;
    root = nullptr;
    pos = 0;

    if (tokens.empty()) {
        reportError("empty input.");
        return false;
    }

    root = parseStatement();

    if (!root) {
        while (pos < tokens.size()) ++pos;
        return false;
    }

    // Handle is Missing ; at te end of code
    if (pos < tokens.size() && tokens[pos].value == ";") {
        ++pos;
    } else {
        int p = (pos < (int)tokens.size() ? tokens[pos].start_pos : -1);
        reportError("missing statement terminator ';'.", p);
    }

    if (pos < tokens.size()) {
        reportError("more expressions found after ';' (only one statement allowed).",
                    tokens[pos].start_pos);
        
        // RECOVERY: Try to parse the next statement(s) to find more errors
        while (pos < tokens.size()) {
            Node* nextStatement = parseStatement();
            // We discard the resulting node, but parseStatement()
            // will have reported any errors it found.

            if (pos < tokens.size() && tokens[pos].value == ";") {
                ++pos;
            } 
            // If it's not a ';', the next loop of parseStatement()
            // will report an error, which is correct.
        }
    }

    bool treeHasError = containsErrorNode(root);
    return !hasErrors() && !treeHasError && root != nullptr;
}

Parser::Node *Parser::parseStatement()
{
    Node *left = nullptr;

    if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
        if (pos < tokens.size())
            reportError("statement must start with an identifier, starts with '" +
                        tokens[pos].value + "'", tokens[pos].start_pos);
        else
            reportError("statement must start with an identifier.");
        
        left = new Node("error_id", TokenType::INVALID);
        
    }
    else {
        left = new Node(tokens[pos].value, TokenType::IDENTIFIER);
        ++pos;
    }

    if (pos >= tokens.size() || tokens[pos].value != "=") {
        if (pos < tokens.size())
            reportError("missing '=' after identifier before '" +
                        tokens[pos].value + "'", tokens[pos].start_pos);
        else
            reportError("missing '=' after identifier.");
        if (pos < tokens.size()) ++pos; 
    } else {
        // Check for '=='
        if ((pos + 1) < tokens.size() && tokens[pos + 1].value == "=") {
            reportError("extra assignment '==' is not allowed.", tokens[pos].start_pos);
            pos += 2;
        } else {
            ++pos; 
        }
    }

    if (pos < tokens.size() && tokens[pos].value == ";") {
        reportError("missing right-hand expression after '='.", tokens[pos].start_pos);
    }

    // expression
    Node *right = parseExpr();
    if (!right) right = new Node("error", TokenType::INVALID);

    return new Node("=", TokenType::ASSIGNMENT, left, right);
}

Parser::Node *Parser::parseExpr()
{
    Node *left = parseTerm();

    if (!left)
    {
        if (pos < tokens.size() && (tokens[pos].value == "+" || tokens[pos].value == "-")) {
            reportError("missing operand before '" + tokens[pos].value + "'", tokens[pos].start_pos);
            ++pos; 
        }
        left = new Node("error", TokenType::INVALID);
    }

    while (pos < tokens.size() && isAddOp(tokens[pos].value))
    {
        std::string op = tokens[pos].value;
        ++pos;

        Node *right = parseTerm();
        if (!right) {
            reportError("missing operand after '" + op + "'", tokens[pos - 1].start_pos);
            right = new Node("error", TokenType::INVALID);
        }

        left = new Node(op, TokenType::OPERATOR, left, right);
    }
    
    if (pos < tokens.size())
    {
        const auto &t = tokens[pos];
        bool isEnd = (t.value == ")" || t.value == ";");

        if (t.value == "=") {
            reportError("chained assignment is not allowed (found '=' in expression).", t.start_pos);
            ++pos; // RECOVERY: Skip the '=' token
            
            // Check for the next error (missing operator)
            if (pos < tokens.size()) {
                const auto &t_next = tokens[pos];
                bool isEnd_next = (t_next.value == ")" || t_next.value == ";");
                if (isFactorStart(t_next) && !isEnd_next) {
                    reportError("missing operator before '" + t_next.value + "'", t_next.start_pos);
                    parseTerm(); // Parse and discard
                }
            }
        }
        else if (isFactorStart(t) && !isEnd) {
            reportError("missing operator before '" + t.value + "'", t.start_pos);
            // Try to parse the unexpected term to find more errors
            // within it (like '()') and consume its tokens.
            parseTerm(); // We call it and discard the result.
        }
    }
    return left;
}

Parser::Node *Parser::parseTerm()
{
    Node *left = parseFactor();
    if (!left) left = new Node("error", TokenType::INVALID);

    while (pos < tokens.size() && isMulOp(tokens[pos].value))
    {
        std::string op = tokens[pos].value;
        ++pos;

        Node *right = parseFactor();
        if (!right) {
            reportError("missing operand after '" + op + "'", tokens[pos - 1].start_pos);
            right = new Node("error", TokenType::INVALID);
        }

        if (op == "/" && right && right->type == TokenType::NUMBER && right->value == "0") {
            reportError("division by zero is not allowed.", tokens[pos - 1].start_pos);
        }

        left = new Node(op, TokenType::OPERATOR, left, right);
    }

    if (pos < tokens.size())
    {
        const auto &t = tokens[pos];
        bool isAdditive = (t.value == "+" || t.value == "-");
        bool isEnd = (t.value == ")" || t.value == ";");

        if (t.value == "=") {
             reportError("chained assignment is not allowed (found '=' in expression).", t.start_pos);
            ++pos;
            
            if (pos < tokens.size()) {
                const auto &t_next = tokens[pos];
                bool isEnd_next = (t_next.value == ")" || t_next.value == ";");
                bool isAdd_next = (t_next.value == "+" || t_next.value == "-");
                if (isFactorStart(t_next) && !isEnd_next && !isAdd_next) {
                    reportError("missing operator before '" + t_next.value + "'", t_next.start_pos);
                    parseFactor();
                }
            }
        }
        else if (isFactorStart(t) && !isAdditive && !isEnd) {
            reportError("missing operator before '" + t.value + "'", t.start_pos);
            parseFactor(); 
        }
    }

    return left;
}

Parser::Node *Parser::parseFactor()
{
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
        if ((pos + 1) < tokens.size() && tokens[pos + 1].value == ")") {
            reportError("empty parenthesis '()' is not a valid factor.", tokens[pos].start_pos);
            pos += 2; 
            return new Node("error", TokenType::INVALID);
        }

        ++pos;
        Node *expr = parseExpr();

        if (pos >= tokens.size() || tokens[pos].value != ")") {
            if (pos < tokens.size())
                reportError("missing closing parenthesis before '" +
                            tokens[pos].value + "'", tokens[pos].start_pos);
            else
                reportError("missing closing parenthesis.");
            return expr; 
        }

        ++pos; 
        return expr;
    }

    if (t.value == "+" || t.value == "-") {
        reportError("unexpected operator '" + t.value + "' (skipping)", t.start_pos);
        ++pos; 
        return parseFactor(); // RECOVERY: Try to parse again
    }

    if (t.value == "*" || t.value == "/") {
        reportError("unexpected operator '" + t.value + "' (skipping)", t.start_pos);
        ++pos;
        return parseFactor(); // RECOVERY: Try to parse again
    }

    if (t.value == ")" || t.value == ";") {
        return nullptr;
    }

    reportError("unexpected token '" + t.value + "'", t.start_pos);
    ++pos; // 
    return nullptr;
}

int Parser::getTreeHeight(const Node *node)
{
    if (!node) return 0;
    return 1 + std::max(getTreeHeight(node->left), getTreeHeight(node->right));
}

static std::string classifyToken(const std::string &val)
{
    if (val == "=")        return "(ASSIGN)";
    if (val == "+" || val == "-" || val == "*" || val == "/") return "(OP)";
    if (val == "(" || val == ")") return "(PAREN)";
    if (val == ";")        return "(END)";
    if (!val.empty() && std::isalpha(static_cast<unsigned char>(val[0]))) return "(ID)";
    if (!val.empty() && std::isdigit(static_cast<unsigned char>(val[0]))) return "(NUM)";
    return "(UNK)";
}

// NEW printSyntaxTree in parser.cpp

void assignCoordinates(Parser::Node* node, int depth, int& current_col)
{
    if (!node) return;

    // 1. Go Left
    // Add 2 to depth for child row (to make space for / \)
    assignCoordinates(node->left, depth + 2, current_col);

    // 2. Visit Root
    std::string label = node->value + classifyToken(node->value);
    int label_width = (int)label.size();

    // Assign coordinates
    node->y_pos = depth;
    node->x_pos = current_col + (label_width / 2);

    // 3. Move the column counter
    current_col += label_width + 1; // +1 for at least one space

    // 4. Go Right
    assignCoordinates(node->right, depth + 2, current_col);
}

// Helper to "stamp" the nodes and branches onto the grid
void renderNodes(const Parser::Node* node, std::vector<std::string>& grid) {
    if (!node) return;

    // 1. Stamp the node label
    std::string label = node->value + classifyToken(node->value);
    int label_width = (int)label.size();
    int start_col = node->x_pos - (label_width / 2);

    for (int i = 0; i < label_width; ++i) {
        if (node->y_pos < (int)grid.size() && (start_col + i) < (int)grid[0].size()) {
            grid[node->y_pos][start_col + i] = label[i];
        }
    }

    // 2. Stamp the branch for the left child
    if (node->left) {
        if (node->y_pos + 1 < (int)grid.size() && node->x_pos - 1 > 0) {
             grid[node->y_pos + 1][node->x_pos - 1] = '/';
        }
        renderNodes(node->left, grid);
    }

    // 3. Stamp the branch for the right child
    if (node->right) {
        if (node->y_pos + 1 < (int)grid.size() && node->x_pos + 1 < (int)grid[0].size()) {
            grid[node->y_pos + 1][node->x_pos + 1] = '\\';
        }
        renderNodes(node->right, grid);
    }
}


void Parser::printSyntaxTree()
{
    if (!root) {
        std::cout << "Syntax tree is empty.";
        return;
    }

    std::cout << "Syntax Tree\n\n\n";

    // 1. Assign all coordinates
    int current_col = 0;
    int height = getTreeHeight(root);
    assignCoordinates(root, 0, current_col);

    // 2. Create the grid
    int rows = height * 2;
    int cols = current_col + 1; // Total width
    std::vector<std::string> grid(rows, std::string(cols, ' '));

    // 3. Render all nodes and branches
    renderNodes(root, grid);

    // 4. Print the grid
    for (auto &line : grid) {
        int end = (int)line.find_last_not_of(' ');
        if (end != std::string::npos)
            std::cout << line.substr(0, end + 1) << "\n";
    }
    std::cout << std::endl;
}
