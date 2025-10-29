#include "../include/parser.hpp"
#include <iostream>
#include <queue>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

// 1. Construct New parser
Parser::Parser(const std::vector<Token> &toks): tokens(toks), pos(0), errorOccurred(false) {}

// 2. Differentiate type of Token (For Precedence)
static inline bool isAddSubOp(const std::string &s) { return s == "+" || s == "-"; }
static inline bool isMulDivOp(const std::string &s) { return s == "*" || s == "/"; }

// 3. Check validity for Token start
static inline bool isFactorStart(const Token &t)
{
    return t.type == TokenType::IDENTIFIER || t.type == TokenType::NUMBER || t.value == "(";
}

// 4. Check if Tree have error Nodes
static bool containsErrorNode(const Parser::Node* node)
{
    if (!node) return false;
    if (node->value == "error") return true;
    return containsErrorNode(node->left) || containsErrorNode(node->right);
}

// 5. Error handling Function (Need to error handle before reading)
void Parser::reportError(const std::string &msg) { reportError(msg, -1); }

// 5.1 Log error with position (Overload)
void Parser::reportError(const std::string &msg, int position)
{
    std::string where = (position >= 0 ? std::to_string(position) : "end");
    std::string full = "SyntaxError at position " + where + ": " + msg;
    errorMessages.push_back(full);
    errorOccurred = true;
}

// 5.2 Print Logged Error
void Parser::printErrors() const
{
    for (const auto &err : errorMessages)
        std::cerr << err << std::endl;
}

// 5.3 Check Error Log
bool Parser::hasErrors() const { return !errorMessages.empty(); }

// 6. Parsing Function
// 6.1 If Parsing Succeeded
bool Parser::parse()
{
    // Reser state for new parse
    errorMessages.clear();
    errorOccurred = false;
    root = nullptr;
    pos = 0;

    // Empty Input
    if (tokens.empty()) 
    {
        reportError("Empty input.");
        return false;
    }

    // A. Parse the first only allowed statement
    root = parseStatement();

    if (!root) 
    {
        // If critical error in parseStatement then skip
        while (pos < tokens.size()) 
            ++pos; // Continue remaining Token
        return false;
    }

    // B. Check Statement Terminator
    if (pos < tokens.size() && tokens[pos].value == ";") 
    {
        ++pos;
    } 
    else { // If termiantor not found
        int p = (pos < (int)tokens.size() ? tokens[pos].start_pos : -1);
        reportError("missing statement terminator ';'.", p);
    }

    // C. Check Extra token after a termination
    if (pos < tokens.size()) 
    {
        reportError("more expressions found after ';' (only one statement allowed).", tokens[pos].start_pos);
        
        // Need to read it all (although we only take the First Expression)
        while (pos < tokens.size())
        {
            parseStatement();   // Parse and discord (Need to log error/Read it all)
            if (pos < tokens.size() && tokens[pos].value == ";") {
                ++pos;
            } 
        }
    }

    // D. Final Check
    bool treeHasError = containsErrorNode(root);
    return !hasErrors() && !treeHasError && root != nullptr;
}

// 6.2 The rules (Grammar) | [ <stmt> -> id = <expr> ; ]
Parser::Node *Parser::parseStatement()
{
    Node *left = nullptr;

    // A. Need an Identifier at start
    if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) 
    {
        reportError("statement must start with an identifier, cannot start with '" + tokens[pos].value + "'", tokens[pos].start_pos);
        // Need to continue Parsing Till the end
        left = new Node("error_id", TokenType::INVALID);
    }
    else // Found Identifier, Continue
    {
        left = new Node(tokens[pos].value, TokenType::IDENTIFIER);
        ++pos;
    }

    // B. Assignment Operator After Identifier
    if (pos >= tokens.size() || tokens[pos].value != "=") 
    {
        if (pos < tokens.size())
            reportError("missing '=' after identifier before '" + tokens[pos].value + "'", tokens[pos].start_pos);
        else
            reportError("missing '=' after identifier.");

        // Skip Token
        if (pos < tokens.size()) ++pos; 
    } 
    else    // Continue to check if "==" occur
    {
        if ((pos + 1) < tokens.size() && tokens[pos + 1].value == "=") 
        {
            reportError("extra assignment '==' is not allowed.", tokens[pos].start_pos);
            pos += 2; // Skip both "==" becouse it has different meaning (Same as)
        } else {
            ++pos; 
        }
    }

    // C. Check for empty Right Parenthesis
    if (pos < tokens.size() && tokens[pos].value == ";") {
        reportError("missing right-hand expression after '='.", tokens[pos].start_pos);
    }

    // D. Parse the right-hand Expression
    Node *right = parseExpr();
    if (!right) right = new Node("error", TokenType::INVALID);

    // Return the assignment Node
    return new Node("=", TokenType::ASSIGNMENT, left, right);
}

// 6.3 Addition/Subtraction Expression | [ <expr> -> <term> { ('+' | '-') <term> } ]
Parser::Node *Parser::parseExpr()
{
    // Parse the first
    Node *left = parseTerm();

    // Handle the missing Operand
    if (!left)
    {
        // To create the Error Node
        if (pos < tokens.size() && (tokens[pos].value == "+" || tokens[pos].value == "-")) 
        {
            reportError("missing operand before '" + tokens[pos].value + "'", tokens[pos].start_pos);
            ++pos; 
        }
        left = new Node("error", TokenType::INVALID);
    }

    // Loop for + - operator
    while (pos < tokens.size() && isAddSubOp(tokens[pos].value))
    {
        std::string op = tokens[pos].value;
        ++pos;

        Node *right = parseTerm();
        if (!right) {
            reportError("missing operand after '" + op + "'", tokens[pos - 1].start_pos);
            right = new Node("error", TokenType::INVALID);
        }

        // For Building tree node
        left = new Node(op, TokenType::OPERATOR, left, right);
    }
    
    // Loop for error when unexpected tokens
    while (pos < tokens.size())
    {
        const auto &t = tokens[pos];
        bool isEnd = (t.value == ")" || t.value == ";");
        if (isEnd) break; 

        // List of Print Error
        if (t.type == TokenType::INVALID) {
            reportError("unexpected token '" + t.value + "'", t.start_pos);
            ++pos; 
        }
        else if (t.value == "=") {
            reportError("chained assignment is not allowed (found '=' in expression).", t.start_pos);
            ++pos; 
        }
        else if (isFactorStart(t)) {
            reportError("missing operator before '" + t.value + "'", t.start_pos);
            parseTerm(); // Parse and discard
        }
        else { // Any other unexpected token
            reportError("unexpected token '" + t.value + "'", t.start_pos);
            ++pos;
        }
    }
    return left;
}

// 6.4 Parses a term (Multiplication/Division) | [ <term> -> <factor> { ('*' | '/') <factor> } ]
Parser::Node *Parser::parseTerm()
{
    // Parse the first factor
    Node *left = parseFactor();

    // Loop for * / factor
    while (pos < tokens.size() && isMulDivOp(tokens[pos].value))
    {
        std::string op = tokens[pos].value;
        ++pos;

        Node *right = parseFactor();
        // Check for missing operand
        if (!right) {
            reportError("missing operand after '" + op + "'", tokens[pos - 1].start_pos);
            right = new Node("error", TokenType::INVALID);
        }

        // Check for division by 0 (Logically error)
        if (op == "/" && right && right->type == TokenType::NUMBER && right->value == "0") {
            reportError("division by zero is not allowed.", tokens[pos - 1].start_pos);
        }

        // For building Tree Node
        left = new Node(op, TokenType::OPERATOR, left, right);
    }

    // Loop for error when unexpected Token
    while (pos < tokens.size())
    {
        const auto &t = tokens[pos];
        bool isAdditive = (t.value == "+" || t.value == "-");
        bool isEnd = (t.value == ")" || t.value == ";");
        if (isAdditive || isEnd) break;

        // List of Print Error
        if (t.type == TokenType::INVALID) {
            reportError("unexpected token '" + t.value + "'", t.start_pos);
            ++pos;
        }
        else if (t.value == "=") {
             reportError("chained assignment is not allowed (found '=' in expression).", t.start_pos);
            ++pos;
        }
        else if (isFactorStart(t)) {
            reportError("missing operator before '" + t.value + "'", t.start_pos);
            parseFactor(); // Parse and discard
        }
        else {  // Any other unexpected Token
            reportError("unexpected token '" + t.value + "'", t.start_pos);
            ++pos;
        }
    }

    return left;
}

// 6.5 Parse a factor (NUM,ID,EXPR) \ [ <factor> -> NUMBER | IDENTIFIER | '(' <expr> ') ]
Parser::Node *Parser::parseFactor()
{
    if (pos >= tokens.size()) {
        reportError("unexpected end of expression.");
        return nullptr;
    }

    const auto &t = tokens[pos];

    // A. Number or Identifier
    if (t.type == TokenType::IDENTIFIER || t.type == TokenType::NUMBER) {
        ++pos;
        return new Node(t.value, t.type);
    }

    // B. Parenthesis
    if (t.value == "(") {
        // Check for empthy Parenthesis
        if ((pos + 1) < tokens.size() && tokens[pos + 1].value == ")") {
            reportError("empty parenthesis '()' is not a valid factor.", tokens[pos].start_pos);
            pos += 2; // Discard both and continue
            return new Node("error", TokenType::INVALID);
        }

        // If found then continue
        ++pos;
        Node *expr = parseExpr();   // Parse inner-Expression

        // Need to Close Parenthesis
        if (pos >= tokens.size() || tokens[pos].value != ")") {
            if (pos < tokens.size())
                reportError("missing closing parenthesis before '" + tokens[pos].value + "'", tokens[pos].start_pos);
            else
                reportError("missing closing parenthesis.");
            return expr; // Still need to return inner Expression
        }

        // If Close parenthesis found. continue
        ++pos; 
        return expr;
    }

    // C. Unexpected Token (if found 2 in one after another)
    if (t.value == "+" || t.value == "-") {
        reportError("unexpected operator '" + t.value + "' (skipping)", t.start_pos);
        ++pos; 
        return parseFactor(); // Try to parse again
    }

    if (t.value == "*" || t.value == "/") {
        reportError("unexpected operator '" + t.value + "' (skipping)", t.start_pos);
        ++pos;
        return parseFactor(); // Try to parse again
    }

    // End of expression
    if (t.value == ")" || t.value == ";") {
        return nullptr; // To signal missing operand (Refer back)
    }

    // Catch all error
    reportError("unexpected token '" + t.value + "'", t.start_pos);
    ++pos; // 
    return nullptr;
}

// 7. Tree Printing Function
// 7.1 Get tree height to estimate the wideness (Tapi lari sikit)
int Parser::getTreeHeight(const Node *node)
{
    if (!node) return 0;
    return 1 + std::max(getTreeHeight(node->left), getTreeHeight(node->right));
}

// 7.2 Print the Name in Tree 
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

// 7.3 In-Order Transversal Tree (using x,y coordinate)
void assignCoordinates(Parser::Node* node, int depth, int& current_col)
{
    if (!node) return;

    // A. Go Left
    // Add 2 to depth for child row (to make space for / \)
    assignCoordinates(node->left, depth + 2, current_col);

    // B. Visit Root
    std::string label = node->value + classifyToken(node->value);
    int label_width = (int)label.size();

    // Assign coordinates
    node->y_pos = depth;
    node->x_pos = current_col + (label_width / 2);

    // C. Move the column counter
    current_col += label_width + 1; // +1 for at least one space

    // D. Go Right
    assignCoordinates(node->right, depth + 2, current_col);
}

// 7.4 Put the nodes and branches onto the grid
void renderNodes(const Parser::Node* node, std::vector<std::string>& grid) {
    if (!node) return;

    // A. Put Node Label
    std::string label = node->value + classifyToken(node->value);
    int label_width = (int)label.size();
    int start_col = node->x_pos - (label_width / 2);

    for (int i = 0; i < label_width; ++i) {

        // Avoid writting outside grid
        if (node->y_pos < (int)grid.size() && (start_col + i) < (int)grid[0].size()) { grid[node->y_pos][start_col + i] = label[i];
        }
    }

    // B. Put branch for the left child
    if (node->left) {
        if (node->y_pos + 1 < (int)grid.size() && node->x_pos - 1 > 0) {
             grid[node->y_pos + 1][node->x_pos - 1] = '/';
        }
        renderNodes(node->left, grid);  // Recursive
    }

    // C. Put branch for the right child
    if (node->right) {
        if (node->y_pos + 1 < (int)grid.size() && node->x_pos + 1 < (int)grid[0].size()) {
            grid[node->y_pos + 1][node->x_pos + 1] = '\\';
        }
        renderNodes(node->right, grid); // Recursive
    }   
}

// 8. Print the Tree
void Parser::printSyntaxTree()
{
    if (!root) {
        std::cout << "Syntax tree is empty.";
        return;
    }

    std::cout << " \n";
    std::cout << "---------------------------------------\n";
    std::cout << "             Syntax Tree\n";
    std::cout << "---------------------------------------\n\n";

    // A. Assign all coordinates
    int current_col = 0;
    int height = getTreeHeight(root);
    assignCoordinates(root, 0, current_col);

    // B. Create the grid (Matrix)
    int rows = height * 2;
    int cols = current_col + 1; // Total width
    std::vector<std::string> grid(rows, std::string(cols, ' '));

    // C. Render all nodes and branches
    renderNodes(root, grid);

    // D. Print the grid
    for (auto &line : grid) {
        int end = (int)line.find_last_not_of(' ');
        if (end != std::string::npos)
            std::cout << line.substr(0, end + 1) << "\n";
    }
    std::cout << std::endl;
}