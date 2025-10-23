#pragma once
#include "token.hpp"
#include <vector>

class Parser
{
private:
    struct Node
    {
        std::string value;
        TokenType type;
        Node *left = nullptr; // C++ default member initialization
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
