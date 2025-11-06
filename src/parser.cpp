#include "../include/parser.hpp"
#include <iostream>
#include <queue>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>

// 1. Construct New parser
Parser::Parser(const std::vector<Token> &toks) : tokens(toks), pos(0), errorOccurred(false) {}

// 2. Differentiate type of Token (For Precedence)
static inline bool isAddSubOp(const std::string &s) { return s == "+" || s == "-"; }
static inline bool isMulDivOp(const std::string &s) { return s == "*" || s == "/"; }

// 3. Check validity for Token start
static inline bool isFactorStart(const Token &t)
{
    return t.type == TokenType::IDENTIFIER || t.type == TokenType::NUMBER || t.value == "(";
}

// 4. Check if Tree have error Nodes
static bool containsErrorNode(const Parser::Node *node)
{
    if (!node)
        return false;
    if (node->value == "error")
        return true;
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
    // Reset state
    errorMessages.clear();
    errorOccurred = false;
    root = nullptr;
    pos = 0;

    if (tokens.empty())
    {
        reportError("Empty input.");
        return false;
    }

    bool firstStatement = true;

    // Parse statements separated by semicolons
    while (pos < tokens.size())
    {
        // If we already parsed one statement and encounter another token that starts one,
        // log that multiple statements are not allowed — but still continue parsing for errors.
        if (!firstStatement)
        {
            reportError("more expressions found after ';' (only one statement allowed).", 
                        tokens[pos].start_pos);
        }

        Node *stmtRoot = parseStatement();
        if (!root && stmtRoot) // keep the first valid tree only
            root = stmtRoot;

        // Check for statement terminator
        if (pos < tokens.size() && tokens[pos].value == ";")
        {
            ++pos; // consume ';'
        }
        else
        {
            // missing semicolon – recover until next ';' or end
            int p = (pos < (int)tokens.size() ? tokens[pos].start_pos : -1);
            reportError("missing statement terminator ';'.", p);
            while (pos < tokens.size() && tokens[pos].value != ";")
                ++pos;
            if (pos < tokens.size())
                ++pos;
        }

        firstStatement = false; // next statement triggers "more expressions" error
    }

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
        if (pos < tokens.size())
            ++pos;
    }
    else // Continue to check if "==" occur
    {
        if ((pos + 1) < tokens.size() && tokens[pos + 1].value == "=")
        {
            reportError("extra assignment '==' is not allowed.", tokens[pos].start_pos);
            pos += 2; // Skip both "==" because it has different meaning (Same as)
        }
        else
        {
            ++pos;
        }
    }

    // C. Check for missing right-hand expression
    // Only report if there is truly nothing to parse after '='
    if (pos >= tokens.size() || tokens[pos].value == ";")
    {
        reportError("missing right-hand expression after '='.", 
                    (pos < tokens.size() ? tokens[pos].start_pos : -1));
    }

    // D. Parse the right-hand Expression
    Node *right = parseExpr();
    if (!right)
        right = new Node("error", TokenType::INVALID);

    // Return the assignment Node
    return new Node("=", TokenType::ASSIGNMENT, left, right);

}

// 6.3 Addition/Subtraction Expression | [ <expr> -> <term> { ('+' | '-') <term> } ]
Parser::Node *Parser::parseExpr()
{
    // Parse the first
    // Skip invalid tokens before starting expression
    while (pos < tokens.size() && tokens[pos].type == TokenType::INVALID)
        ++pos;

    Node *left = parseTerm();


    // Handle the missing Operand
    if (!left)
    {
        // To create the Error Node
        if (pos < tokens.size() && (tokens[pos].value == "+" || tokens[pos].value == "-"))
        {
            reportError("missing operand before '" + tokens[pos].value + "'", tokens[pos].start_pos);
            left = new Node("error", TokenType::INVALID); // keep structure alive
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
        if (!right)
        {
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

        // Stop cleanly if we reach end of expression or statement
        if (t.value == ")" || t.value == ";")
            break;

        if (t.type == TokenType::INVALID)
        {
            ++pos;
            continue;
        }

        if (t.value == "=")
        {
            reportError("chained assignment is not allowed (found '=' in expression).", t.start_pos);
            ++pos;
            continue;
        }

        if (isFactorStart(t))
        {
            // Only flag missing operator if this isn't following a valid operator
            if (!(pos > 0 && (tokens[pos - 1].type == TokenType::OPERATOR)))
            {
                reportError("missing operator before '" + t.value + "'", t.start_pos);
            }
            // Don’t consume semicolon or valid factor here
            ++pos;
            continue;
        }

        // Any other unexpected token
        ++pos;
    }
    return left;
}

// 6.4 Parses a term (Multiplication/Division) | [ <term> -> <factor> { ('*' | '/') <factor> } ]
Parser::Node *Parser::parseTerm()
{
    // Parse the first factor
    Node *left = nullptr;

    // Skip invalid tokens before starting
    while (pos < tokens.size() && tokens[pos].type == TokenType::INVALID)
        ++pos;

    left = parseFactor();


    // Loop for * / factor
    while (pos < tokens.size() && isMulDivOp(tokens[pos].value))
    {
        std::string op = tokens[pos].value;
        ++pos;

        Node *right = parseFactor();
        // Check for missing operand
        if (!right)
        {
            reportError("missing operand after '" + op + "'", tokens[pos - 1].start_pos);
            right = new Node("error", TokenType::INVALID);
        }

        // Check for division by 0 (Logically error)
        if (op == "/" && right && right->type == TokenType::NUMBER && right->value == "0")
        {
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
        if (isAdditive || isEnd)
            break;

        // List of Print Error
        if (t.type == TokenType::INVALID)
        {
            //reportError("unexpected token '" + t.value + "'", t.start_pos);
            ++pos;
            continue;
        }
        else if (t.value == "=")
        {
            reportError("chained assignment is not allowed (found '=' in expression).", t.start_pos);
            ++pos;
        }
        else if (isFactorStart(t))
        {
            reportError("missing operator before '" + t.value + "'", t.start_pos);
            parseFactor(); // Parse and discard
        }
        else
        { // Any other unexpected Token
            //reportError("unexpected token '" + t.value + "'", t.start_pos);
            ++pos;
        }
    }

    return left;
}

// 6.5 Parse a factor (NUM,ID,EXPR) \ [ <factor> -> NUMBER | IDENTIFIER | '(' <expr> ') ]
Parser::Node *Parser::parseFactor()
{
    if (pos >= tokens.size())
    {
        reportError("unexpected end of expression.");
        return nullptr;
    }

    const auto &t = tokens[pos];

    // A. Number or Identifier
    if (t.type == TokenType::IDENTIFIER || t.type == TokenType::NUMBER)
    {
        ++pos;
        return new Node(t.value, t.type);
    }

    // B. Parenthesis
    if (t.value == "(")
    {
        // Check for empthy Parenthesis
        if ((pos + 1) < tokens.size() && tokens[pos + 1].value == ")")
        {
            reportError("empty parenthesis '()' is not a valid factor.", tokens[pos].start_pos);
            pos += 2; // Discard both and continue
            return new Node("error", TokenType::INVALID);
        }

        // If found then continue
        ++pos;
        Node *expr = parseExpr(); // Parse inner-Expression

        // Need to Close Parenthesis
        if (pos >= tokens.size() || tokens[pos].value != ")")
        {
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
    if (t.value == "+" || t.value == "-")
    {
        reportError("unexpected operator '" + t.value + "'", t.start_pos);
        ++pos;
        return parseFactor(); // Try to parse again
    }

    if (t.value == "*" || t.value == "/")
    {
        reportError("unexpected operator '" + t.value + "'", t.start_pos);
        ++pos;
        return parseFactor(); // Try to parse again
    }

    // End of expression
    if (t.value == ")" || t.value == ";")
    {
        return nullptr; // To signal missing operand (Refer back)
    }

    // Catch all error
    //reportError("unexpected token '" + t.value + "'", t.start_pos);
    ++pos; //
    return nullptr;
}

// 7. Tree Printing Function
// 7.1 Get tree height to estimate the wideness (Tapi lari sikit)
int Parser::getTreeHeight(const Node *node) const
{
    if (!node)
        return 0;
    return 1 + std::max(getTreeHeight(node->left), getTreeHeight(node->right));
}

// 7.2 Print the Name in Tree
static std::string classifyToken(const std::string &val)
{
    if (val == "=")
        return "(ASSIGN)";
    if (val == "+" || val == "-" || val == "*" || val == "/")
        return "(OP)";
    if (val == "(" || val == ")")
        return "(PAREN)";
    if (val == ";")
        return "(END)";
    if (!val.empty() && std::isalpha(static_cast<unsigned char>(val[0])))
        return "(ID)";
    if (!val.empty() && std::isdigit(static_cast<unsigned char>(val[0])))
        return "(NUM)";
    return "(UNK)";
}

using display_rows = std::vector<std::vector<Parser::cell_display>>;
display_rows Parser::get_row_display() const
{
    // start off by traversing the tree to
    // build a vector of vectors of Node pointers
    std::vector<Node *> traversal_stack;
    std::vector<std::vector<Node *>> rows;
    if (!root)
        return display_rows();

    Node *p = root;
    const int max_depth = getTreeHeight(root);
    rows.resize(max_depth);
    int depth = 0;
    for (;;)
    {
        // Max-depth Nodes are always a leaf or null
        // This special case blocks deeper traversal
        if (depth == max_depth - 1)
        {
            rows[depth].push_back(p);
            if (depth == 0)
                break;
            --depth;
            continue;
        }

        // First visit to node?  Go to left child.
        if (traversal_stack.size() == depth)
        {
            rows[depth].push_back(p);
            traversal_stack.push_back(p);
            if (p)
                p = p->left;
            ++depth;
            continue;
        }

        // Odd child count? Go to right child.
        if (rows[depth + 1].size() % 2)
        {
            p = traversal_stack.back();
            if (p)
                p = p->right;
            ++depth;
            continue;
        }

        // Time to leave if we get here

        // Exit loop if this is the root
        if (depth == 0)
            break;

        traversal_stack.pop_back();
        p = traversal_stack.back();
        --depth;
    }

    // Use rows of Node pointers to populate rows of cell_display structs.
    // All possible slots in the tree get a cell_display struct,
    // so if there is no actual Node at a struct's location,
    // its boolean "present" field is set to false.
    // The struct also contains a string representation of
    // its Node's value, created using a std::stringstream object.
    display_rows rows_disp;
    std::stringstream ss;
    for (const auto &row : rows)
    {
        rows_disp.emplace_back();
        for (Node *pn : row)
        {
            if (pn)
            {
                // this affect how the value is printed on the tree
                ss << pn->value;
                rows_disp.back().push_back(cell_display(ss.str()));
                ss = std::stringstream();
            }
            else
            {
                rows_disp.back().push_back(cell_display());
            }
        }
    }
    return rows_disp;
}

// row_formatter takes the vector of rows of cell_display structs
// generated by get_row_display and formats it into a test representation
// as a vector of strings
std::vector<std::string> Parser::row_formatter(const display_rows &rows_disp) const
{
    using s_t = std::string::size_type;

    // First find the maximum value string length and put it in cell_width
    s_t cell_width = 0;
    for (const auto &row_disp : rows_disp)
    {
        for (const auto &cd : row_disp)
        {
            if (cd.present && cd.valstr.length() > cell_width)
            {
                cell_width = cd.valstr.length();
            }
        }
    }

    // make sure the cell_width is an odd number
    if (cell_width % 2 == 0)
        ++cell_width;

    // allows leaf nodes to be connected when they are
    // all with size of a single character
    if (cell_width < 3)
        cell_width = 3;

    // formatted_rows will hold the results
    std::vector<std::string> formatted_rows;

    // some of these counting variables are related,
    // so its should be possible to eliminate some of them.
    s_t row_count = rows_disp.size();

    // this row's element count, a power of two
    s_t row_elem_count = 1 << (row_count - 1);

    // left_pad holds the number of space charactes at the beginning of the bottom row
    s_t left_pad = 0;

    // Work from the level of maximum depth, up to the root
    // ("formatted_rows" will need to be reversed when done)
    for (s_t r = 0; r < row_count; ++r)
    {
        const auto &cd_row = rows_disp[row_count - r - 1]; // r reverse-indexes the row
        // "space" will be the number of rows of slashes needed to get
        // from this row to the next.  It is also used to determine other
        // text offsets.
        s_t space = (s_t(1) << r) * (cell_width + 1) / 2 - 1;
        // "row" holds the line of text currently being assembled
        std::string row;
        // iterate over each element in this row
        for (s_t c = 0; c < row_elem_count; ++c)
        {
            // add padding, more when this is not the leftmost element
            row += std::string(c ? left_pad * 2 + 1 : left_pad, ' ');
            if (cd_row[c].present)
            {
                // This position corresponds to an existing Node
                const std::string &valstr = cd_row[c].valstr;
                // Try to pad the left and right sides of the value string
                // with the same number of spaces.  If padding requires an
                // odd number of spaces, right-sided children get the longer
                // padding on the right side, while left-sided children
                // get it on the left side.
                s_t long_padding = cell_width - valstr.length();
                s_t short_padding = long_padding / 2;
                long_padding -= short_padding;
                row += std::string(c % 2 ? short_padding : long_padding, ' ');
                row += valstr;
                row += std::string(c % 2 ? long_padding : short_padding, ' ');
            }
            else
            {
                // This position is empty, Nodeless...
                row += std::string(cell_width, ' ');
            }
        }
        // A row of spaced-apart value strings is ready, add it to the result vector
        formatted_rows.push_back(row);

        // The root has been added, so this loop is finsished
        if (row_elem_count == 1)
            break;

        // Add rows of forward- and back- slash characters, spaced apart
        // to "connect" two rows' Node value strings.
        // The "space" variable counts the number of rows needed here.
        s_t left_space = space + 1;
        s_t right_space = space - 1;
        for (s_t sr = 0; sr < space; ++sr)
        {
            std::string row;
            for (s_t c = 0; c < row_elem_count; ++c)
            {
                if (c % 2 == 0)
                {
                    row += std::string(c ? left_space * 2 + 1 : left_space, ' ');
                    row += cd_row[c].present ? '/' : ' ';
                    row += std::string(right_space + 1, ' ');
                }
                else
                {
                    row += std::string(right_space, ' ');
                    row += cd_row[c].present ? '\\' : ' ';
                }
            }
            formatted_rows.push_back(row);
            ++left_space;
            --right_space;
        }
        left_pad += space + 1;
        row_elem_count /= 2;
    }

    // Reverse the result, placing the root node at the beginning (top)
    std::reverse(formatted_rows.begin(), formatted_rows.end());

    return formatted_rows;
}

// Trims an equal number of space characters from
// the beginning of each string in the vector.
// At least one string in the vector will end up beginning
// with no space characters.
void Parser::trim_rows_left(std::vector<std::string> &rows)
{
    if (!rows.size())
        return;
    auto min_space = rows.front().length();
    for (const auto &row : rows)
    {
        auto i = row.find_first_not_of(' ');
        if (i == std::string::npos)
            i = row.length();
        if (i == 0)
            return;
        if (i < min_space)
            min_space = i;
    }
    for (auto &row : rows)
    {
        row.erase(0, min_space);
    }
}

// Dumps a representation of the tree to cout
void Parser::displayTree() const
{
    const int d = getTreeHeight(root);

    // If this tree is empty, tell someone
    if (d == 0)
    {
        std::cout << " <empty tree>\n";
        return;
    }

    // This tree is not empty, so get a list of node values...
    const auto rows_disp = get_row_display();
    // then format these into a text representation...
    auto formatted_rows = row_formatter(rows_disp);
    // then trim excess space characters from the left sides of the text...
    trim_rows_left(formatted_rows);
    // then dump the text to cout.
    for (const auto &row : formatted_rows)
    {
        std::cout << ' ' << row << '\n';
    }
}

// 8. Print the Tree
void Parser::printSyntaxTree()
{
    if (!root)
    {
        std::cout << "Syntax tree is empty.";
        return;
    }

    std::cout << " \n";
    std::cout << "-----------------------------------------------------------\n";
    std::cout << "                       Syntax Tree\n";
    std::cout << "-----------------------------------------------------------\n\n";

    // print the tree
    displayTree();

    std::cout << std::endl;
}
