#pragma once                // Header Guard
#include "token.hpp"        // Include Token Class (enum TokenType & struct Token)
#include <vector>

class Parser
{

// Private : Accessible only inside Parser Class (Encapsulation)
public: 
    struct Node          // Represent Node in syntax tree
    {
        std::string value;          // Actual Token string ("3","z","+")     
        TokenType type;             // Token Type (NUM,ID,OP)
        Node *left = nullptr;       // C++ default member initialization
        Node *right = nullptr;
        int x_pos = 0;
        int y_pos = 0;  

        // constructor
        Node(const std::string &val, TokenType t, Node *l = nullptr, Node *r = nullptr): value(val), type(t), left(l), right(r) {}
    };

private: 
    const std::vector<Token> &tokens;
    size_t pos;
    Node *root = nullptr; 

    Node *parseStatement();
    Node *parseExpr();
    Node *parseTerm();
    Node *parseFactor();

    bool errorOccurred = false;                
    std::vector<std::string> errorMessages;

    void printNode(const Node *, int);
    std::string tokenTypeToString(TokenType);

    int getTreeHeight(const Node* node);
    void fillTree(const Node* node, std::vector<std::string>& grid, int row, int col, int offset, int height);


public:
    explicit Parser(const std::vector<Token> &toks);
    bool parse();           
    void printSyntaxTree(); 
    void reportError(const std::string &msg);              
    void reportError(const std::string &msg, int position); 
    void printErrors() const;
    bool hasErrors() const;
};
