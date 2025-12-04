// ===================================================
//                   TEAM MEMBERS
// ===================================================
//  • Ahmed Abdelnabi Abdelrasol Ibrahim   (20220027)
//  • Muhammad Mostafa Ali Elsayed         (20220309)
//  • Yusuf Naser Muhammad Kamal           (20220416)
// ===================================================

// Enumeration of all token types in the language
enum TokenType{
    // Keywords
    IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
    // Operators
    ASSIGN, EQUAL, LESS_THAN,
    PLUS, MINUS, TIMES, DIVIDE, POWER, AND,  // Added my custom operator (&)
    // Punctuation
    SEMI_COLON,
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    // Identifiers and literals
    ID, NUM,
    // Special tokens
    ENDFILE, ERROR
  };

// String representations of token types for debugging output
const char* TokenTypeStr[]=
{
    "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write",
    "Assign", "Equal", "LessThan",
    "Plus", "Minus", "Times", "Divide", "Power", "And",
    "SemiColon",
    "LeftParen", "RightParen",
    "LeftBrace", "RightBrace",
    "ID", "Num",
    "EndFile", "Error"
};

// Array of symbolic tokens mapping token types to their string representations
// NOTE: Order matters for tokens with common prefixes (e.g., <= must come before <)
// The closing comment should come immediately after opening comment
const Token symbolic_tokens[]=
{
    Token(ASSIGN, ":="),
    Token(EQUAL, "="),
    Token(LESS_THAN, "<"),
    Token(PLUS, "+"),
    Token(AND, "&"),              // The & operator I just added
    Token(MINUS, "-"),
    Token(TIMES, "*"),
    Token(DIVIDE, "/"),
    Token(POWER, "^"),
    Token(SEMI_COLON, ";"),
    Token(LEFT_PAREN, "("),
    Token(RIGHT_PAREN, ")"),
    Token(LEFT_BRACE, "{"),
    Token(RIGHT_BRACE, "}")
};

////////////////////////////////////////////////////////////////////////////////////
// Parser //////////////////////////////////////////////////////////////////////////

// program -> stmtseq
// stmtseq -> stmt { ; stmt }
// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
// ifstmt -> if exp then stmtseq [ else stmtseq ] end
// repeatstmt -> repeat stmtseq until expr
// assignstmt -> identifier := expr
// readstmt -> read identifier
// writestmt -> write expr
// expr -> mathexpr [ (<|=) mathexpr ]
// mathexpr -> term { (+|-) term }       left associative
// term -> andopr { (*|/) andopr }       left associative
// andopr -> factor { (&) factor }       left associative (My addition)
// factor -> newexpr { ^ newexpr }       right associative
// newexpr -> ( mathexpr ) | number | identifier


// Forward declaration for recursive grammar rules
TreeNode* MathExpr(CompilerInfo*, ParseInfo*);

// newexpr -> ( mathexpr ) | number | identifier
// Parses the lowest-level expressions: literals, identifiers, or parenthesized expressions
TreeNode* NewExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start NewExpr");

    // Case 1: Number literal (e.g., 42, 123)
    if(ppi->next_token.type==NUM)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=NUM_NODE;

        // Convert string to integer
        char* num_str=ppi->next_token.str;
        tree->num=0;
        while(*num_str) tree->num=tree->num*10+((*num_str++)-'0');

        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    // Case 2: Identifier (variable name)
    if(ppi->next_token.type==ID)
    {
        TreeNode* tree=new TreeNode;
        tree->node_kind=ID_NODE;
        AllocateAndCopy(&tree->id, ppi->next_token.str);
        tree->line_num=pci->in_file.cur_line_num;
        Match(pci, ppi, ppi->next_token.type);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    // Case 3: Parenthesized expression (e.g., (x + 5))
    if(ppi->next_token.type==LEFT_PAREN)
    {
        Match(pci, ppi, LEFT_PAREN);
        TreeNode* tree=MathExpr(pci, ppi);  // Recursively parse inner expression
        Match(pci, ppi, RIGHT_PAREN);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    // Syntax error: expected number, identifier, or left parenthesis
    throw 0;
    return 0;
}

// factor -> newexpr { ^ newexpr }    right associative
// Handles exponentiation operator (^) with RIGHT associativity
// Example: 2^3^2 is parsed as 2^(3^2) = 512
TreeNode* Factor(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Factor");

    TreeNode* tree=NewExpr(pci, ppi);

    // Right associativity: recursively call Factor for right operand
    if(ppi->next_token.type==POWER)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);  // Recursive call for right associativity

        pci->debug_file.Out("End Factor");
        return new_tree;
    }
    pci->debug_file.Out("End Factor");
    return tree;
}

// andopr -> factor { (&) factor }    left associative
// Handles the & operator (which I added) with LEFT associativity
// The & operator computes: a^2 - b^2 (difference of squares)
// Example: 5 & 3 = 25 - 9 = 16
// Example: 4 & 3 & 2 is parsed as ((4 & 3) & 2) due to left associativity
TreeNode* AndOpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start AndOpr");

    TreeNode* tree=Factor(pci, ppi);

    // Left associativity: use while loop to chain operations left-to-right
    while(ppi->next_token.type==AND)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;               // Left operand
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);   // Right operand

        tree=new_tree;  // Update tree for left associativity
    }
    pci->debug_file.Out("End AndOpr");
    return tree;
}

// term -> andopr { (*|/) andopr }    left associative
// Handles multiplication and division with LEFT associativity
// Example: 8 / 4 / 2 is parsed as (8 / 4) / 2 = 1
TreeNode* Term(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Term");

    TreeNode* tree=AndOpr(pci, ppi);

    // Left associativity: process all * and / operators from left to right
    while(ppi->next_token.type==TIMES || ppi->next_token.type==DIVIDE)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=AndOpr(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End Term");
    return tree;
}

// mathexpr -> term { (+|-) term }    left associative
// Handles addition and subtraction with LEFT associativity
// Example: 10 - 3 + 2 is parsed as (10 - 3) + 2 = 9
TreeNode* MathExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start MathExpr");

    TreeNode* tree=Term(pci, ppi);

    // Left associativity: process all + and - operators from left to right
    while(ppi->next_token.type==PLUS || ppi->next_token.type==MINUS)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Term(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End MathExpr");
    return tree;
}

// expr -> mathexpr [ (<|=) mathexpr ]
// Handles comparison operators: < (less than) and = (equality)
// Example: x < 10, y = 5
TreeNode* Expr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Expr");

    TreeNode* tree=MathExpr(pci, ppi);

    // Optional comparison operator
    if(ppi->next_token.type==EQUAL || ppi->next_token.type==LESS_THAN)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=MathExpr(pci, ppi);

        pci->debug_file.Out("End Expr");
        return new_tree;
    }
    pci->debug_file.Out("End Expr");
    return tree;
}

// Evaluate - Recursively evaluates an expression tree
// Returns the computed value of the expression
int Evaluate(TreeNode* node, SymbolTable* symbol_table, int* variables)
{
    // Base case 1: Number literal
    if(node->node_kind==NUM_NODE) return node->num;

    // Base case 2: Variable (look up in symbol table and get value)
    if(node->node_kind==ID_NODE)
        return variables[symbol_table->Find(node->id)->memloc];

    // Recursive case: Evaluate both children
    int a=Evaluate(node->child[0], symbol_table, variables);
    int b=Evaluate(node->child[1], symbol_table, variables);

    // Apply the operator
    if(node->oper==EQUAL) return a==b;           // Equality: returns 1 if equal, 0 otherwise
    if(node->oper==LESS_THAN) return a<b;        // Less than: returns 1 if true, 0 otherwise
    if(node->oper==PLUS) return a+b;             // Addition
    if(node->oper==MINUS) return a-b;            // Subtraction
    if(node->oper==TIMES) return a*b;            // Multiplication
    if(node->oper==DIVIDE) return a/b;           // Integer division
    if(node->oper==POWER) return Power(a,b);     // Exponentiation: a^b
    if(node->oper==AND) return (a*a)-(b*b);      // My & operator: computes a² - b²

    // Error: unknown operator
    throw 0;
    return 0;
}
