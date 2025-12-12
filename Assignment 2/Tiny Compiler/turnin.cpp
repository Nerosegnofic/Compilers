// Ahmed Abdelnabi (20220027)
// George Rafaat (20220097)
// Muhammad Mostafa (20220309)
// Yusuf Naser (20220416)

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
using namespace std;

/*
int a;
real b;
bool c;
int x;
real y;
bool z;
int ra;
real rb;
int rc;
bool ta;
bool tb;
bool tc;

a := 5;
b := 3.5;
c := true;

{1}
x := a + 3;
{ 5 + 3 = 8 }
{ x = 8 }
write x;

{2}
y := b * 2.0;
{ 3.5 * 2.0 = 7.0 }
{ y = 7.0 }
write y;

{3}
z := a < 10;
{ 5 < 10 = true }
write z;

{4}
ta := (a = 5);
{ true }
write ta;

{5}
tb := (b < 1.0);
{ false }
write tb;

{6}
ra := a + a;
{ 5 + 5 = 10 }
write ra;

{7}
rb := b + a;
{ 3.5 + 5 = 8.5 }
write rb;

{8}
rc := a + (a + 1);
{ 5 + (5 + 1) = 11}
write rc;

{9}
write a^b;

{10}
write b^a;

{11}
a := (a ^ 2);
{ 5^2 = 25 }
write a;

{12}
b := (b ^ 3);
{ b = 3.5^3 }
{ b = 42.875}
write b;

{13}
c := (5 < 2);
{ c = false }
write c;

{14}
if c = false then
  write 1
else
  write 0
end;

{15}
if a < 10 then
  write 1
else
  write 0
end;

{16}
x := 0;
repeat
  x := x + 1;
  write x
until x = 3;

{17}
x := 5;
tc := false;
repeat
  x := x - 1;
  write x;
  if x < 0 then
    tc := true
  end
until tc;

{18}
if false then
  if true then
    write 1
  else
    write 2
  end
else
  if true then
    write 3
  else
    write 4
  end
end;

{19}
if true then
  if true then
    write 1
  else
    write 2
  end
else
  if true then
    write 3
  else
    write 4
  end
end;

{20}
y := a;
write y;

{21}
a := y;
write a

{22} { int a } {All variables must be declared at the beginning of the code }

{23} { c := a } { Can't assign integers to booleans }

{24} { c := b } { Can't assign reals to booleans }

{25} { a := c + 1 } { Can't perform arithmetic operations on boolean variables }

{26} { a := c * 1 }  { Can't perform arithmetic operations on boolean variables }

{27} { if a then write 5 end } { If condition must have boolean variables or boolean conditions }

{28} { repeat write 1 until a } { Until condition must have boolean variables or boolean conditions }
*/

enum TokenType
{
    // new token types, removed NUM token
    INT_TYPE, REAL_TYPE, BOOL_TYPE, TRUE, FALSE, INT_VALUE, REAL_VALUE
};


const char* TokenTypeStr[]=
{
    "IntType", "RealType", "BoolType", "True", "False", "IntValue", "RealValue"
};

const Token reserved_words[]=
{
    // .. rest
    // adding the new reserved words
    Token(INT_TYPE, "int"),
    Token(REAL_TYPE, "real"),
    Token(BOOL_TYPE, "bool"),
    Token(TRUE, "true"),
    Token(FALSE, "false")
};

void GetNextToken(CompilerInfo* pci, Token* ptoken)
{
    // .. rest
    else if(IsDigit(s[0]))
    {
        int j=1;
        while(IsDigit(s[j])) j++;
        if (s[j]=='.'){ // if real
            j++;
            while(IsDigit(s[j])) j++;
            ptoken->type=REAL_VALUE;
        } else { // else -> int
            ptoken->type=INT_VALUE;
        }
        Copy(ptoken->str, s, j);
    }
    // .. rest
}


// program -> declarations stmtseq
// declarations -> {(int|bool|real) identifier;}
// stmtseq -> stmt { ; stmt }
// .. rest

enum NodeKind
{
    // new node kinds, removed NUM_NODE
    INT_NODE, BOOL_NODE, REAL_NODE
};


const char* NodeKindStr[]=
{
    "Int", "Bool", "Real"
};

enum ExprDataType {VOID, INTEGER, REAL, BOOLEAN};

const char* ExprDataTypeStr[]=
            {
                "Void", "Integer", "Real", "Boolean" // add Real type
            };

struct TreeNode
{
    // .. rest
    union{TokenType oper; double num; char* id;}; // store all variables as doubles
    // .. rest
};

TreeNode* Expr(CompilerInfo*, ParseInfo*); // instead of MathExpr to allow parentheses for boolean expretions

// newexpr -> ( mathexpr ) | int | real | boolean | identifier
TreeNode* NewExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start NewExpr");

    // Compare the next token with the First() of possible statements

    if(ppi->next_token.type==LEFT_PAREN)
    {
        Match(pci, ppi, LEFT_PAREN);
        TreeNode* tree=Expr(pci, ppi);
        Match(pci, ppi, RIGHT_PAREN);

        pci->debug_file.Out("End NewExpr");
        return tree;
    }

    TreeNode* tree=new TreeNode;
    char* str=ppi->next_token.str;
    tree->line_num=pci->in_file.cur_line_num;

    if(ppi->next_token.type==INT_VALUE) // if int -> same as before
    {
        tree->node_kind=INT_NODE;
        int value=0; while(*str) value=value*10+((*str++)-'0');
        tree->num=value;
    }
    else if(ppi->next_token.type==REAL_VALUE) // if real -> use atof to get the double value
    {
        tree->node_kind=REAL_NODE;
        tree->num=atof(str);
    }
    else if(ppi->next_token.type==TRUE || ppi->next_token.type==FALSE) // true = 1, false = 0
    {
        tree->node_kind=BOOL_NODE;
        tree->num=(ppi->next_token.type==TRUE);
    }
    if(ppi->next_token.type==ID)
    {
        tree->node_kind=ID_NODE;
        AllocateAndCopy(&tree->id, str);
    }
    Match(pci, ppi, ppi->next_token.type);
    pci->debug_file.Out("End NewExpr");
    return tree;

    throw 0;
    return 0;
}

struct VariableInfo
{
    // .. rest
    ExprDataType type; // store variable datatype
    // .. rest
};

struct SymbolTable
{
    // .. rest

    VariableInfo* Find(const char* name, int line_num)
    {
        int h=Hash(name);
        VariableInfo* cur=var_info[h];
        while(cur)
        {
            if(Equals(name, cur->name)) return cur;
            cur=cur->next_var;
        }
        printf("ERROR using undeclared variable %s at line %d\n", name, line_num); // if not found, throw an error
        throw 0;
    }

    // Declare a variable with a given type. If already declared, append line info.
    void Insert(const char* name, int line_num, ExprDataType vtype)
    {
        LineLocation* lineloc=new LineLocation;
        lineloc->line_num=line_num;
        lineloc->next=0;

        int h=Hash(name);
        VariableInfo* prev=0;
        VariableInfo* cur=var_info[h];

        while(cur)
        {
            if(Equals(name, cur->name))
            {
                printf("ERROR redeclaration of variable %s at line %d\n", name, line_num); // if already exists, throw an error for redeclaration
                throw 0;
            }
            prev=cur;
            cur=cur->next_var;
        }

        VariableInfo* vi=new VariableInfo;
        vi->head_line=vi->tail_line=lineloc;
        vi->next_var=0;
        vi->memloc=num_vars++;
        vi->type = vtype; // store the type
        AllocateAndCopy(&vi->name, name);

        if(!prev) var_info[h]=vi;
        else prev->next_var=vi;
    }

    // .. rest
};

// ParseDeclarations: parse declarations at program start
void ParseDeclarations(CompilerInfo* pci, ParseInfo* ppi, SymbolTable* symbol_table)
{
    // permit zero or more declarations
    while(ppi->next_token.type==INT_TYPE || ppi->next_token.type==REAL_TYPE || ppi->next_token.type==BOOL_TYPE)
    {
        ExprDataType vtype;
        if(ppi->next_token.type==INT_TYPE) vtype = INTEGER;
        else if(ppi->next_token.type==REAL_TYPE) vtype = REAL;
        else vtype = BOOLEAN;

        Match(pci, ppi, ppi->next_token.type); // int/real/bool token

        if(ppi->next_token.type!=ID) { printf("ERROR Expected identifier in declaration at line %d\n", pci->in_file.cur_line_num); throw 0; }

        // record declaration
        symbol_table->Insert(ppi->next_token.str, pci->in_file.cur_line_num, vtype); // store the variable with the type

        Match(pci, ppi, ID);

        // require semicolon
        Match(pci, ppi, SEMI_COLON);
    }
}

// Parse (program -> declarations then stmtseq)
TreeNode* Parse(CompilerInfo* pci, SymbolTable* symbol_table)
{
    ParseInfo parse_info;
    GetNextToken(pci, &parse_info.next_token);

    // first parse declarations (must be at start)
    ParseDeclarations(pci, &parse_info, symbol_table); // handle declarations before statements

    TreeNode* syntax_tree = StmtSeq(pci, &parse_info);

    if(parse_info.next_token.type!=ENDFILE)
        pci->debug_file.Out("Error code ends before file ends");

    return syntax_tree;
}

// PrintTree and DestroyTree ///////////////////////////////////////////////////////

void PrintTree(TreeNode* node, int sh=0)
{
    // .. rest
    // print correct type for each node
    if(node->node_kind==OPER_NODE) printf("[%s]", TokenTypeStr[node->oper]);
    else if(node->node_kind==INT_NODE) printf("[%d]", (int)node->num);
    else if(node->node_kind==REAL_NODE) printf("[%f]", node->num);
    else if(node->node_kind==BOOL_NODE) printf("[%s]", (node->num != 0)? "true":"false");
    else if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE) printf("[%s]", node->id);
    // .. rest
}

// The Analyze function now enforces declaration usage and type rules
void Analyze(TreeNode* node, SymbolTable* symbol_table)
{
    int i;

    // If encountering an identifier node, make sure it's declared and set its expr_data_type
    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
    {
        VariableInfo* vi = symbol_table->Find(node->id, node->line_num);
        // set node expr_data_type based on variable info
        node->expr_data_type = vi->type;
    }

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) Analyze(node->child[i], symbol_table);

    // If operator, determine expression type
    if(node->node_kind==OPER_NODE)
    {
        if(node->oper==EQUAL || node->oper==LESS_THAN) node->expr_data_type=BOOLEAN;
        else
        {
            // arithmetic operators: left and right must be numeric for arithmetic, boolean only for comparisons
            ExprDataType leftType = node->child[0]->expr_data_type;
            ExprDataType rightType = node->child[1]->expr_data_type;
            if(leftType==BOOLEAN || rightType==BOOLEAN)
            {
                printf("ERROR arithmetic operator applied to boolean at line %d\n", node->line_num);
                throw 0;
            }
            else node->expr_data_type = (leftType==REAL || rightType==REAL) ? REAL : INTEGER; // if one of them is REAL, cast the other to real
        }
    }
    else if(node->node_kind==INT_NODE)
    {
        node->expr_data_type=INTEGER;
    }
    else if (node->node_kind==REAL_NODE)
    {
        node->expr_data_type=REAL;
    }
    else if (node->node_kind==BOOL_NODE)
    {
        node->expr_data_type=BOOLEAN;
    }
    if(node->node_kind==IF_NODE && node->child[0]->expr_data_type!=BOOLEAN)
    {
        printf("ERROR If test must be BOOLEAN at line %d\n", node->line_num);
        throw 0;
    }
    if(node->node_kind==REPEAT_NODE && node->child[1]->expr_data_type!=BOOLEAN)
    {
        printf("ERROR Repeat test must be BOOLEAN at line %d\n", node->line_num);
        throw 0;
    }

    // Assignments: left side is ID node; ensure types match exactly
    if(node->node_kind==ASSIGN_NODE)
    {
        // left is variable (ID at node->id), right is child[0]
        VariableInfo* vi = symbol_table->Find(node->id, node->line_num);
        ExprDataType rhsType = node->child[0]->expr_data_type;
        ExprDataType lhsType = vi->type;

        if(lhsType!=rhsType)
        {
            // allow implicit int->real in rhs numeric expressions
            if(lhsType==BOOLEAN || rhsType==BOOLEAN)
            {
                printf("ERROR Type mismatch on assignment to %s at line %d\n", vi->name, node->line_num);
                throw 0;
            }
        }
    }

    if(node->sibling) Analyze(node->sibling, symbol_table);
}

double Evaluate(TreeNode* node, SymbolTable* symbol_table, double* variables) // return double instead of int
{
    if(node->node_kind==INT_NODE || node->node_kind==REAL_NODE || node->node_kind==BOOL_NODE) return node->num;
    if(node->node_kind==ID_NODE) return variables[symbol_table->Find(node->id, node->line_num)->memloc];

    double a=Evaluate(node->child[0], symbol_table, variables);
    double b=Evaluate(node->child[1], symbol_table, variables);

    // .. rest
    if(node->oper==POWER) return pow(a,b); // use power from cmath to allow fraction powers
    throw 0;
    return 0.0;
}

// RunProgram: uses separate pools
void RunProgram(TreeNode* node, SymbolTable* symbol_table, double* variables)
{
    // .. rest
    else if(node->node_kind==ASSIGN_NODE)
    {
        int v=Evaluate(node->child[0], symbol_table, variables);
        variables[symbol_table->Find(node->id, node->line_num)->memloc]=v;
    }
    else if(node->node_kind==READ_NODE)
    {
        VariableInfo* vi = symbol_table->Find(node->id, node->line_num);
        if(vi->type==READ_NODE) // specify the type depending on the variable type
        {
            int val;
            printf("Enter %s (int): ", node->id);
            scanf("%d", &val);
            variables[vi->memloc] = val;
        }
        else if(vi->type==REAL)
        {
            printf("Enter %s (real): ", node->id);
            scanf("%lf", &variables[vi->memloc]);
        }
        else
        {
            printf("Enter %s (bool): ", node->id);
            char input[100];
            scanf("%s", input);
            if (Equals(input, "true"))
                variables[vi->memloc] = 1.0;
            else if(Equals(input, "false"))
                variables[vi->memloc] = 0.0;
            else
            {
                printf("Runtime ERROR %s can't be assign't to bool (true/false)", node->id);
                throw 0;
            }
        }
    }
    else if(node->node_kind==WRITE_NODE) // write output depending on the type
    {
        TreeNode* expr = node->child[0];
        double value = Evaluate(expr, symbol_table, variables);
        if(expr->expr_data_type==REAL)
        {
            printf("Val: %f\n", value);
        }
        else if(expr->expr_data_type==INTEGER)
        {
            printf("Val: %d\n", (int)value);
        }
        else // boolean printing as 0/1
        {
            printf("Val: %s\n", (value != 0)? "true":"false");
        }
    }
    // .. rest
}

void RunProgram(TreeNode* syntax_tree, SymbolTable* symbol_table)
{
    int i;
    double* variables=new double[symbol_table->num_vars]; // double instead of int
    // .. rest
}