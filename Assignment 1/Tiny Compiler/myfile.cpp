// ===================================================
//                   TEAM MEMBERS
// ===================================================
//   Ahmed Abdelnabi Abdelrasol Ibrahim   (20220027)
//   Muhammad Mostafa Ali Elsayed         (20220309)
//   Yusuf Naser Muhammad Kamal           (20220416)
// ===================================================

/*
read a;     { a = 5 }
read b;     { b = 3 }

{1}  c := a & b;                   
     { 25 - 9 = 16 }
     { c = 16 }
write c;

{2}  d := a & b & c;               
     { 5 & 3 = 16 }
     { 16 & 16 = 0 }
     { d = 0 }
write d;

{3}  e := a & (b + c);             
     { b + c = 19 }
     { 5 & 19 = 25 - 361 = -336 }
     { e = -336 }
write e;

{4}  f := (a + b) & (c - b);       
     { a + b = 8, c - b = 13 }
     { 64 - 169 = -105 }
     { f = -105 }
write f;

{5}  g := a & b + c;               
     { a & b = 16 }
     { 16 + 16 = 32 }
     { g = 32 }
write g;

{6}  h := a + b & c;               
     { b & c = 9 - 256 = -247 }
     { 5 + (-247) = -242 }
     { h = -242 }
write h;

{7}  x := a * b & c;               
     { b&c = -247 }
     { 5 * -247 = -1235 }
     { x = -1235 }
write x;

{8}  y := a & b * c;               
     { a&b = 16 }
     { 16 * 16 = 256 }
     { y = 256 }
write y;

{9}  z := a ^ b & c;               
     { 5^3 = 125 }
     { 15625 - 256 = 15369 }
     { z = 15369 }
write z;

{10} t := a & b ^ c;               
      { (a&(b^c)) will produce an overflown positive number }
write t;

{11} a := a & a;                   
      { 25 - 25 = 0 }
      { a = 0 }
write a;

{12} b := 4; c := 2;               { reset values }
      u := b & c;                  
      { 4*4 - 2*2 = 16 - 4 = 12 }
      { u = 12 }
write u;

{13} v := (b + c) & (b - c);       
      { 6 & 2 = 36 - 4 = 32 }
      { v = 32 }
write v;

{14} w := b & (c * 2);             
      { c*2 = 4 }
      { 16 - 16 = 0 }
      { w = 0 }
write w;

{15} k := (b * 3) & (c + 5);       
      { b*3 = 12, c+5 = 7 }
      { 144 - 49 = 95 }
      { k = 95 }
write k;

{16} m := (b ^ 2) & c;             
      { b^2 = 16 }
      { 256 - 4 = 252 }
      { m = 252 }
write m;

{17} n := b & (c ^ 2);             
      { c^2 = 4 }
      { 16 - 16 = 0 }
      { n = 0 }
write n;

{18} p := (b + 1) & (c + 1);       
      { 5 & 3 = 25 - 9 = 16 }
      { p = 16 }
write p;

{19} q := (b - 1) & (2 * c);       
      { b-1 = 3, 2*c = 4 }
      { 9 - 16 = -7 }
      { q = -7 }
write q;

{20} r := (b / 2) & (c + 3);       
      { b/2 = 2, c+3 = 5 }
      { 4 - 25 = -21 }
write r
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
    pci->debug_file.Out("Start Term");                      // Log entry into Term parsing function

    TreeNode* tree=AndOpr(pci, ppi);                        // Parse first AndOpr (includes & operator)

    while(ppi->next_token.type==TIMES || ppi->next_token.type==DIVIDE) // While next token is * or /
    {
        TreeNode* new_tree=new TreeNode;                    // Create a new operator node
        new_tree->node_kind=OPER_NODE;                      // Mark node as an operator
        new_tree->oper=ppi->next_token.type;                // Set operator type to * or /
        new_tree->line_num=pci->in_file.cur_line_num;      // Store line number

        new_tree->child[0]=tree;                            // Left operand is the previously parsed tree
        Match(pci, ppi, ppi->next_token.type);              // Consume the operator token
        new_tree->child[1]=AndOpr(pci, ppi);               // Parse right operand

        tree=new_tree;                                      // Update tree for left associativity
    }
    pci->debug_file.Out("End Term");                        // Log exit from Term parsing function
    return tree;                                            // Return the parsed term tree
}

// Evaluate - Recursively evaluates an expression tree
// Returns the computed value of the expression
int Evaluate(TreeNode* node, SymbolTable* symbol_table, int* variables)
{
    // Base case 1: Number literal
    if(node->node_kind==NUM_NODE) return node->num;

    // Base case 2: Variable (look up in symbol table and get value)
    if(node->node_kind==ID_NODE) return variables[symbol_table->Find(node->id)->memloc];

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
    if(node->oper==AND) return (a*a)-(b*b);      // My & operator: computes a*a - b*b

    // Error: unknown operator
    throw 0;
    return 0;
}
