// ===================================================
//                   TEAM MEMBERS
// ===================================================
//  • Ahmed Abdelnabi Abdelrasol Ibrahim   (20220027)
//  • Muhammad Mostafa Ali Elsayed         (20220309)
//  • Yusuf Naser Muhammad Kamal           (20220416)
// ===================================================

/*
  =======================
      TINY TEST SUITE
     for operator a & b
  =======================

  Semantics:  a & b = (a^2) - (b^2)
  Given input: a = 5, b = 3
  All other variables initially = 0

  TINY TEST PROGRAM WITH RESULTS:

  read a;     { a = 5 }
  read b;     { b = 3 }

  {1}  c := a & b;                   { 5² - 3² = 25 - 9 = 16 }
                                     { c = 16 }

  {2}  d := a & b & c;               { (a & b) = 16 }
                                     { 16 & 16 = 256 - 256 = 0 }
                                     { d = 0 }

  {3}  e := a & (b + c);             { b + c = 3 + 16 = 19 }
                                     { 5 & 19 = 25 - 361 = -336 }
                                     { e = -336 }

  {4}  f := (a + b) & (c - b);       { a + b = 8, c - b = 16 - 3 = 13 }
                                     { 8 & 13 = 64 - 169 = -105 }
                                     { f = -105 }

  {5}  g := a & b + c;               { a & b = 16, then + c = 16 + 16 = 32 }
                                     { g = 32 }

  {6}  h := a + b & c;               { b & c = 3² - 16² = 9 - 256 = -247 }
                                     { a + (b & c) = 5 + (-247) = -242 }
                                     { h = -242 }

  {7}  x := a * b & c;               { a*b = 15 }
                                     { 15 & 16 = 225 - 256 = -31 }
                                     { x = -31 }

  {8}  y := a & b * c;               { b*c = 3*16 = 48 }
                                     { 5 & 48 = 25 - 2304 = -2279 }
                                     { y = -2279 }

  {9}  z := a ^ b & c;               { a^b = 5^3 = 125 }
                                     { 125 & 16 = 15625 - 256 = 15369 }
                                     { z = 15369 }

  {10} t := a & b ^ c;               { b^c = 3^2 = 9 }
                                     { 5 & 9 = 25 - 81 = -56 }
                                     { t = -56 }

  {11} a := a & a;                   { 5 & 5 = 25 - 25 = 0 }
                                     { a = 0 }

  {12} b := (a & b) & (c & d);       { a & b = 0² - 3² = -9 }
                                     { c & d = 16² - 0² = 256 }
                                     { -9 & 256 = 81 - 65536 = -65455 }
                                     { b = -65455 }

  {13} c := a - b & c + d;           { b & c = (-65455)² - 16² = very large positive }
                                     { c = 0 - (very large) + 0 = very large negative }
                                     { c ≈ -4.284386769e9 }

  {14} d := a / b & c * d;           { a/b = 0, c*d = 0 }
                                     { 0 & 0 = 0 }
                                     { d = 0 }

  {15} if a & b < c then x := 1 else x := 2 end;
                                     { a & b = 0² - (-65455)² = -4.284386025e9 }
                                     { -4.28e9 < -4.28e9 (slightly smaller) → TRUE }
                                     { x = 1 }

  {16} if a < b & c then y := 3 else y := 4 end;
                                     { b & c = huge positive - huge positive = huge negative }
                                     { 0 < huge_negative → FALSE }
                                     { y = 4 }

  {17} repeat z := z & a until z < 0;
                                     { a = 0 }
                                     { z & 0 = z², always positive }
                                     { LOOP NEVER ENDS (infinite loop) }

  {18} f := (a & b) * (c & d);        { a & b = -4.284386025e9 }
                                     { c & d = (very large) }
                                     { result = very large negative }
                                     { f = very large negative }

  {19} g := (a + b) & (c * d);       { a + b = -65455 }
                                     { c * d = 0 }
                                     { g = (-65455)² = 4284387025 }
                                     { g = 4284387025 }

  {20} h := (a - b) & (c / d);       { c / d = division by zero → undefined }
                                     { h = undefined }

  {21} t := (a ^ 2) & (b ^ 2);       { a^2 = 0, b^2 = 4284387025 }
                                     { t = 0 - 4284387025 = -4284387025 }
                                     { t = -4284387025 }

  write c;   { prints ≈ -4.28e9 }
  write d;   { prints 0 }
  write e;   { prints -336 }
  write f;   { prints very large negative }
  write g;   { prints 4284387025 }
  write h;   { undefined (division by zero) }
  write x;   { prints 1 }
  write y;   { prints 4 }
  write z;   { infinite loop, never reached }
  write t;   { prints -4284387025 }
*/

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
