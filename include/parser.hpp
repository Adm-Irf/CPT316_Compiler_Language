#pragma once                // Header Guard
#include "token.hpp"        // Include Token Class (enum TokenType & struct Token)
#include <vector>

class Parser
{

// Private : Accessible only inside Parser Class (Encapsulation)
private: 
    struct Node          // Represent Node in syntax tree
    {
        std::string value;          // Actual Token string ("3","z","+")     
        TokenType type;             // Token Type (NUM,ID,OP)
        Node *left = nullptr;       // C++ default member initialization
        Node *right = nullptr;      

        // constructor
        Node(const std::string &val, TokenType t, Node *l = nullptr, Node *r = nullptr)
            : value(val), type(t), left(l), right(r) {}
    };

    const std::vector<Token> &tokens;
    size_t pos;
    Node *root = nullptr; // store the root of the tree

    Node *parseStatement();
    Node *parseExpr();
    Node *parseTerm();
    Node *parseFactor();

    void printNode(const Node *, int);
    std::string tokenTypeToString(TokenType);

public:
    explicit Parser(const std::vector<Token> &toks);
    bool parse();           // returns true if grammar valid
    void printSyntaxTree(); // simple visualization
};
