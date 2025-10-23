#include "../include/parser.hpp"
#include <iostream>

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

void Parser::printSyntaxTree()
{
    if (!root)
    {
        std::cout << "Syntax tree is empty.\n";
        return;
    }
    printNode(root, 0);
}

void Parser::printNode(const Node *node, int depth)
{
    if (!node)
        return;

    // indentation
    for (int i = 0; i < depth; ++i)
        std::cout << "  ";

    // print current node
    std::cout << node->value << " (" << tokenTypeToString(node->type) << ")" << std::endl;

    // recursively print left and right children
    if (node->left)
        printNode(node->left, depth + 1);
    if (node->right)
        printNode(node->right, depth + 1);
}

std::string Parser::tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TokenType::IDENTIFIER:
        return "ID";
    case TokenType::NUMBER:
        return "NUM";
    case TokenType::OPERATOR:
        return "OP";
    case TokenType::ASSIGNMENT:
        return "ASSIGN";
    default:
        return "UNKNOWN";
    }
}
