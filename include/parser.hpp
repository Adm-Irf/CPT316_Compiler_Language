#pragma once         // Header Guard
#include "token.hpp" // Include Token Definition
#include <vector>

class Parser
{

    // Public Member (Need to define node before use in private member)
public:
    struct Node // Represent Node in syntax tree
    {
        std::string value;    // Token Value ("1","a","+")
        TokenType type;       // Token Type (NUM,ID,OP)
        Node *left = nullptr; // Default member initialization
        Node *right = nullptr;

        // Constructor (Check parameter format must be same)
        Node(const std::string &val, TokenType t, Node *l = nullptr, Node *r = nullptr) : value(val), type(t), left(l), right(r) {}
    };

    explicit Parser(const std::vector<Token> &toks); // Contructor
    bool parse();                                    // Parsing Function : Return true when successful (False when error)

    void printSyntaxTree();                                 // Print generated Tree
    bool hasErrors() const;                                 // Error Function : Return true when Error logged
    void reportError(const std::string &msg);               // Log Syntax Error Message
    void reportError(const std::string &msg, int position); // Same log but with position (overload)
    void printErrors() const;                               // Print all logged error (can print more than 1)

    struct cell_display
    {
        std::string valstr;
        bool present;
        cell_display() : present(false) {}
        cell_display(std::string valstr) : valstr(valstr), present(true) {}
    };

    using display_rows = std::vector<std::vector<Parser::cell_display>>; 
    display_rows get_row_display() const;

    std::vector<std::string> row_formatter(const display_rows &rows_disp) const;
    static void trim_rows_left(std::vector<std::string> &rows);
    void Dump() const;
    // Private Member
private:
    // Reference to Token Vector
    const std::vector<Token> &tokens;
    size_t pos;
    Node *root = nullptr;

    // Main Parsing Function
    Node *parseStatement();
    Node *parseExpr();
    Node *parseTerm();
    Node *parseFactor();

    // Error Handling Function
    bool errorOccurred = false;
    std::vector<std::string> errorMessages;

    // Tree calculation
    int getTreeHeight(const Node *node) const;
};