// ===================================================
//                   TEAM MEMBERS
// ===================================================
//    Ahmed Abdelnabi Abdelrasol Ibrahim   (20220027)
//    George Rafaat Saber Reyad            (20220097)
//    Muhammad Mostafa Ali Elsayed         (20220309)
//    Yusuf Naser Muhammad Kamal           (20220416)
// ===================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
using namespace std;

// Strings /////////////////////////////////////////////////////////////////////////

bool Equals(const char* a, const char* b)
{
    return strcmp(a, b)==0;
}

bool StartsWith(const char* a, const char* b)
{
    int nb=strlen(b);
    return strncmp(a, b, nb)==0;
}

void Copy(char* a, const char* b, int n=0)
{
    if(n>0) {strncpy(a, b, n); a[n]=0;}
    else strcpy(a, b);
}

void AllocateAndCopy(char** a, const char* b)
{
    if(b==0) {*a=0; return;}
    int n=strlen(b);
    *a=new char[n+1];
    strcpy(*a, b);
}

// Input and Output ////////////////////////////////////////////////////////////////

#define MAX_LINE_LENGTH 10000

struct InFile
{
    FILE* file;
    int cur_line_num;

    char line_buf[MAX_LINE_LENGTH];
    int cur_ind, cur_line_size;

    InFile(const char* str) {file=0; if(str) file=fopen(str, "r"); cur_line_size=0; cur_ind=0; cur_line_num=0;}
    ~InFile(){if(file) fclose(file);}

    void SkipSpaces()
    {
        while(cur_ind<cur_line_size)
        {
            char ch=line_buf[cur_ind];
            if(ch!=' ' && ch!='\t' && ch!='\r' && ch!='\n') break;
            cur_ind++;
        }
    }

    bool SkipUpto(const char* str)
    {
        while(true)
        {
            SkipSpaces();
            while(cur_ind>=cur_line_size) {if(!GetNewLine()) return false; SkipSpaces();}

            if(StartsWith(&line_buf[cur_ind], str))
            {
                cur_ind+=strlen(str);
                return true;
            }
            cur_ind++;
        }
        return false;
    }

    bool GetNewLine()
    {
        cur_ind=0; line_buf[0]=0;
        if(!fgets(line_buf, MAX_LINE_LENGTH, file)) return false;
        cur_line_size=strlen(line_buf);
        if(cur_line_size==0) return false; // End of file
        cur_line_num++;
        return true;
    }

    char* GetNextTokenStr()
    {
        SkipSpaces();
        while(cur_ind>=cur_line_size) {if(!GetNewLine()) return 0; SkipSpaces();}
        return &line_buf[cur_ind];
    }

    void Advance(int num)
    {
        cur_ind+=num;
    }
};

struct OutFile
{
    FILE* file;
    OutFile(const char* str) {file=0; if(str) file=fopen(str, "w");}
    ~OutFile(){if(file) fclose(file);}

    void Out(const char* s)
    {
        fprintf(file, "%s\n", s); fflush(file);
    }
};

// Compiler Parameters /////////////////////////////////////////////////////////////

struct CompilerInfo
{
    InFile in_file;
    OutFile out_file;
    OutFile debug_file;

    CompilerInfo(const char* in_str, const char* out_str, const char* debug_str)
                : in_file(in_str), out_file(out_str), debug_file(debug_str)
    {
    }
};

// Scanner /////////////////////////////////////////////////////////////////////////

#define MAX_TOKEN_LEN 40

enum TokenType{
                IF, THEN, ELSE, END, REPEAT, UNTIL, READ, WRITE,
                // new type keywords
                INT_TYPE, REAL_TYPE, BOOL_TYPE, TRUE, FALSE,
                ASSIGN, EQUAL, LESS_THAN,
                PLUS, MINUS, TIMES, DIVIDE, POWER,
                SEMI_COLON,
                LEFT_PAREN, RIGHT_PAREN,
                LEFT_BRACE, RIGHT_BRACE,
                ID, INT_VALUE, REAL_VALUE,
                ENDFILE, ERROR
              };

// Used for debugging only /////////////////////////////////////////////////////////
const char* TokenTypeStr[]=
            {
                "If", "Then", "Else", "End", "Repeat", "Until", "Read", "Write",
                "IntType", "RealType", "BoolType", "True", "False",
                "Assign", "Equal", "LessThan",
                "Plus", "Minus", "Times", "Divide", "Power",
                "SemiColon",
                "LeftParen", "RightParen",
                "LeftBrace", "RightBrace",
                "ID", "IntValue", "RealValue",
                "EndFile", "Error"
            };

struct Token
{
    TokenType type;
    char str[MAX_TOKEN_LEN+1];

    Token(){str[0]=0; type=ERROR;}
    Token(TokenType _type, const char* _str) {type=_type; Copy(str, _str);}
};

const Token reserved_words[]=
{
    Token(IF, "if"),
    Token(THEN, "then"),
    Token(ELSE, "else"),
    Token(END, "end"),
    Token(REPEAT, "repeat"),
    Token(UNTIL, "until"),
    Token(READ, "read"),
    Token(WRITE, "write"),
    Token(INT_TYPE, "int"),
    Token(REAL_TYPE, "real"),
    Token(BOOL_TYPE, "bool"),
    Token(TRUE, "true"),
    Token(FALSE, "false")
};
const int num_reserved_words=sizeof(reserved_words)/sizeof(reserved_words[0]);

// if there is tokens like < <=, sort them such that sub-tokens come last: <= <
// the closing comment should come immediately after opening comment
const Token symbolic_tokens[]=
{
    Token(ASSIGN, ":="),
    Token(EQUAL, "="),
    Token(LESS_THAN, "<"),
    Token(PLUS, "+"),
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
const int num_symbolic_tokens=sizeof(symbolic_tokens)/sizeof(symbolic_tokens[0]);

inline bool IsDigit(char ch){return (ch>='0' && ch<='9');}
inline bool IsLetter(char ch){return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z')); }
inline bool IsLetterOrUnderscore(char ch){return (IsLetter(ch) || ch=='_'); }

void GetNextToken(CompilerInfo* pci, Token* ptoken)
{
    ptoken->type=ERROR;
    ptoken->str[0]=0;

    int i;
    char* s=pci->in_file.GetNextTokenStr();
    if(!s)
    {
        ptoken->type=ENDFILE;
        ptoken->str[0]=0;
        return;
    }

    for(i=0;i<num_symbolic_tokens;i++)
    {
        if(StartsWith(s, symbolic_tokens[i].str))
            break;
    }

    if(i<num_symbolic_tokens)
    {
        if(symbolic_tokens[i].type==LEFT_BRACE)
        {
            pci->in_file.Advance(strlen(symbolic_tokens[i].str));
            if(!pci->in_file.SkipUpto(symbolic_tokens[i+1].str)) return;
            return GetNextToken(pci, ptoken);
        }
        ptoken->type=symbolic_tokens[i].type;
        Copy(ptoken->str, symbolic_tokens[i].str);
    }
    else if(IsDigit(s[0]))
    {
        int j=1;
        while(IsDigit(s[j])) j++;
        if (s[j]=='.'){
            j++;
            while(IsDigit(s[j])) j++;
            ptoken->type=REAL_VALUE;
        } else {
            ptoken->type=INT_VALUE;
        }
        Copy(ptoken->str, s, j);
    }
    else if(IsLetterOrUnderscore(s[0]))
    {
        int j=1;
        while(IsLetterOrUnderscore(s[j])) j++;

        ptoken->type=ID;
        Copy(ptoken->str, s, j);

        for(i=0;i<num_reserved_words;i++)
        {
            if(Equals(ptoken->str, reserved_words[i].str))
            {
                ptoken->type=reserved_words[i].type;
                break;
            }
        }
    }

    int len=strlen(ptoken->str);
    if(len>0) pci->in_file.Advance(len);
}

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
// term -> factor { (*|/) factor }       left associative
// factor -> newexpr { ^ newexpr }       right associative
// newexpr -> ( mathexpr ) | number | identifier

enum NodeKind{
                IF_NODE, REPEAT_NODE, ASSIGN_NODE, READ_NODE, WRITE_NODE,
                OPER_NODE, ID_NODE, INT_NODE, BOOL_NODE, REAL_NODE
             };

// Used for debugging only /////////////////////////////////////////////////////////
const char* NodeKindStr[]=
            {
                "If", "Repeat", "Assign", "Read", "Write",
                "Oper", "Num", "ID", "Int", "Bool", "Real"
            };

enum ExprDataType {VOID, INTEGER, REAL, BOOLEAN};

// Used for debugging only /////////////////////////////////////////////////////////
const char* ExprDataTypeStr[]=
            {
                "Void", "Integer", "Real", "Boolean"
            };

#define MAX_CHILDREN 3

struct TreeNode
{
    TreeNode* child[MAX_CHILDREN];
    TreeNode* sibling; // used for sibling statements only

    NodeKind node_kind;

    union{TokenType oper; int num; char* id; double real; bool boolean;}; // defined for expression/int/identifier only
    ExprDataType expr_data_type; // defined for expression/int/identifier only

    int line_num;

    TreeNode() {int i; for(i=0;i<MAX_CHILDREN;i++) child[i]=0; sibling=0; expr_data_type=VOID;}
};

struct ParseInfo
{
    Token next_token;
};

void Match(CompilerInfo* pci, ParseInfo* ppi, TokenType expected_token_type)
{
    pci->debug_file.Out("Start Match");

    if(ppi->next_token.type!=expected_token_type) throw 0;
    GetNextToken(pci, &ppi->next_token);

    fprintf(pci->debug_file.file, "[%d] %s (%s)\n", pci->in_file.cur_line_num, ppi->next_token.str, TokenTypeStr[ppi->next_token.type]); fflush(pci->debug_file.file);
}

TreeNode* Expr(CompilerInfo*, ParseInfo*);

// newexpr -> ( mathexpr ) | number | identifier
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

    if(ppi->next_token.type==INT_VALUE)
    {
        tree->node_kind=INT_NODE;
        tree->num=0; while(*str) tree->num=tree->num*10+((*str++)-'0');
    }
    else if(ppi->next_token.type==REAL_VALUE)
    {
        tree->node_kind=REAL_NODE;
        tree->real=atof(str);
    }
    else if(ppi->next_token.type==TRUE || ppi->next_token.type==FALSE)
    {
        tree->node_kind=BOOL_NODE;
        tree->boolean=(ppi->next_token.type==TRUE);
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

// factor -> newexpr { ^ newexpr }    right associative
TreeNode* Factor(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Factor");

    TreeNode* tree=NewExpr(pci, ppi);

    if(ppi->next_token.type==POWER)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        pci->debug_file.Out("End Factor");
        return new_tree;
    }
    pci->debug_file.Out("End Factor");
    return tree;
}

// term -> factor { (*|/) factor }    left associative
TreeNode* Term(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Term");

    TreeNode* tree=Factor(pci, ppi);

    while(ppi->next_token.type==TIMES || ppi->next_token.type==DIVIDE)
    {
        TreeNode* new_tree=new TreeNode;
        new_tree->node_kind=OPER_NODE;
        new_tree->oper=ppi->next_token.type;
        new_tree->line_num=pci->in_file.cur_line_num;

        new_tree->child[0]=tree;
        Match(pci, ppi, ppi->next_token.type);
        new_tree->child[1]=Factor(pci, ppi);

        tree=new_tree;
    }
    pci->debug_file.Out("End Term");
    return tree;
}

// mathexpr -> term { (+|-) term }    left associative
TreeNode* MathExpr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start MathExpr");

    TreeNode* tree=Term(pci, ppi);

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
TreeNode* Expr(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Expr");

    TreeNode* tree=MathExpr(pci, ppi);

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

// writestmt -> write expr
TreeNode* WriteStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start WriteStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=WRITE_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, WRITE);
    tree->child[0]=Expr(pci, ppi);

    pci->debug_file.Out("End WriteStmt");
    return tree;
}

// readstmt -> read identifier
TreeNode* ReadStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start ReadStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=READ_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, READ);
    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);

    pci->debug_file.Out("End ReadStmt");
    return tree;
}

// assignstmt -> identifier := expr
TreeNode* AssignStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start AssignStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=ASSIGN_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    if(ppi->next_token.type==ID) AllocateAndCopy(&tree->id, ppi->next_token.str);
    Match(pci, ppi, ID);
    Match(pci, ppi, ASSIGN); tree->child[0]=Expr(pci, ppi);

    pci->debug_file.Out("End AssignStmt");
    return tree;
}

TreeNode* StmtSeq(CompilerInfo*, ParseInfo*);

// repeatstmt -> repeat stmtseq until expr
TreeNode* RepeatStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start RepeatStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=REPEAT_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, REPEAT); tree->child[0]=StmtSeq(pci, ppi);
    Match(pci, ppi, UNTIL); tree->child[1]=Expr(pci, ppi);

    pci->debug_file.Out("End RepeatStmt");
    return tree;
}

// ifstmt -> if exp then stmtseq [ else stmtseq ] end
TreeNode* IfStmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start IfStmt");

    TreeNode* tree=new TreeNode;
    tree->node_kind=IF_NODE;
    tree->line_num=pci->in_file.cur_line_num;

    Match(pci, ppi, IF); tree->child[0]=Expr(pci, ppi);
    Match(pci, ppi, THEN); tree->child[1]=StmtSeq(pci, ppi);
    if(ppi->next_token.type==ELSE) {Match(pci, ppi, ELSE); tree->child[2]=StmtSeq(pci, ppi);}
    Match(pci, ppi, END);

    pci->debug_file.Out("End IfStmt");
    return tree;
}

// stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
TreeNode* Stmt(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start Stmt");

    // Compare the next token with the First() of possible statements
    TreeNode* tree=0;
    if(ppi->next_token.type==IF) tree=IfStmt(pci, ppi);
    else if(ppi->next_token.type==REPEAT) tree=RepeatStmt(pci, ppi);
    else if(ppi->next_token.type==ID) tree=AssignStmt(pci, ppi);
    else if(ppi->next_token.type==READ) tree=ReadStmt(pci, ppi);
    else if(ppi->next_token.type==WRITE) tree=WriteStmt(pci, ppi);
    else throw 0;

    pci->debug_file.Out("End Stmt");
    return tree;
}

// stmtseq -> stmt { ; stmt }
TreeNode* StmtSeq(CompilerInfo* pci, ParseInfo* ppi)
{
    pci->debug_file.Out("Start StmtSeq");

    TreeNode* first_tree=Stmt(pci, ppi);
    TreeNode* last_tree=first_tree;

    // If we did not reach one of the Follow() of StmtSeq(), we are not done yet
    while(ppi->next_token.type!=ENDFILE && ppi->next_token.type!=END &&
          ppi->next_token.type!=ELSE && ppi->next_token.type!=UNTIL)
    {
        Match(pci, ppi, SEMI_COLON);
        TreeNode* next_tree=Stmt(pci, ppi);
        last_tree->sibling=next_tree;
        last_tree=next_tree;
    }

    pci->debug_file.Out("End StmtSeq");
    return first_tree;
}

// Declarations parsing and symbol table integration ///////////////////////////////

const int SYMBOL_HASH_SIZE=10007;

struct LineLocation
{
    int line_num;
    LineLocation* next;
};

enum VarType {VT_INT, VT_REAL, VT_BOOL};

struct VariableInfo
{
    char* name;
    VarType type;      // variable type
    int memloc;        // index in the corresponding pool (int/real/bool)
    LineLocation* head_line; // source line locations linked list
    LineLocation* tail_line;
    VariableInfo* next_var;
};

struct SymbolTable
{
    int num_vars; // total number of distinct variables (keeps track of symbol count)
    VariableInfo* var_info[SYMBOL_HASH_SIZE];

    // counts for each type pool
    int num_int_vars;
    int num_real_vars;
    int num_bool_vars;

    SymbolTable() { num_vars=0; num_int_vars=0; num_real_vars=0; num_bool_vars=0; int i; for(i=0;i<SYMBOL_HASH_SIZE;i++) var_info[i]=0; }

    int Hash(const char* name)
    {
        int i, len=strlen(name);
        int hash_val=11;
        for(i=0;i<len;i++) hash_val=(hash_val*17+(int)name[i])%SYMBOL_HASH_SIZE;
        return hash_val;
    }

    VariableInfo* Find(const char* name)
    {
        int h=Hash(name);
        VariableInfo* cur=var_info[h];
        while(cur)
        {
            if(Equals(name, cur->name)) return cur;
            cur=cur->next_var;
        }
        return 0;
    }

    // Declare a variable with a given type. If already declared, append line info.
    void Declare(const char* name, VarType vtype, int line_num)
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
                // already declared -> record additional line
                cur->tail_line->next=lineloc;
                cur->tail_line=lineloc;
                printf("ERROR redeclaration of variable %s at line %d\n", name, line_num - 1);
                return;
            }
            prev=cur;
            cur=cur->next_var;
        }

        VariableInfo* vi=new VariableInfo;
        vi->head_line=vi->tail_line=lineloc;
        vi->next_var=0;
        vi->memloc = 0;
        vi->type = vtype;
        AllocateAndCopy(&vi->name, name);

        // assign memloc according to type pool
        if(vtype==VT_INT) vi->memloc = num_int_vars++;
        else if(vtype==VT_REAL) vi->memloc = num_real_vars++;
        else /*VT_BOOL*/ vi->memloc = num_bool_vars++;

        vi->next_var = 0;
        if(!prev) var_info[h]=vi;
        else prev->next_var=vi;

        num_vars++;
    }

    void Print()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                const char* tstr = (curv->type==VT_INT) ? "int" : ((curv->type==VT_REAL) ? "real" : "bool");
                printf("[Var=%s][Type=%s][Mem=%d]", curv->name, tstr, curv->memloc);
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    printf("[Line=%d]", curl->line_num);
                    curl=curl->next;
                }
                printf("\n");
                curv=curv->next_var;
            }
        }
    }

    void Destroy()
    {
        int i;
        for(i=0;i<SYMBOL_HASH_SIZE;i++)
        {
            VariableInfo* curv=var_info[i];
            while(curv)
            {
                LineLocation* curl=curv->head_line;
                while(curl)
                {
                    LineLocation* pl=curl;
                    curl=curl->next;
                    delete pl;
                }
                VariableInfo* p=curv;
                curv=curv->next_var;
                delete p;
            }
            var_info[i]=0;
        }
    }
};

// Forward declare Parse (updated signature)
TreeNode* Parse(CompilerInfo* pci, SymbolTable* symbol_table);

// ParseDeclarations: parse declarations at program start like "int x; real y; bool z;"
void ParseDeclarations(CompilerInfo* pci, ParseInfo* ppi, SymbolTable* symbol_table)
{
    // permit zero or more declarations
    while(ppi->next_token.type==INT_TYPE || ppi->next_token.type==REAL_TYPE || ppi->next_token.type==BOOL_TYPE)
    {
        VarType vtype;
        if(ppi->next_token.type==INT_TYPE) vtype = VT_INT;
        else if(ppi->next_token.type==REAL_TYPE) vtype = VT_REAL;
        else vtype = VT_BOOL;

        Match(pci, ppi, ppi->next_token.type); // int/real/bool token

        if(ppi->next_token.type!=ID) { printf("ERROR Expected identifier in declaration at line %d\n", pci->in_file.cur_line_num); throw 0; }
        char name_buf[MAX_TOKEN_LEN+1];
        Copy(name_buf, ppi->next_token.str);
        Match(pci, ppi, ID);

        // require semicolon
        Match(pci, ppi, SEMI_COLON);

        // record declaration
        symbol_table->Declare(name_buf, vtype, pci->in_file.cur_line_num);
    }
}

// Parser functions (RepeatStmt, IfStmt, etc.) already implemented above

// Parse (program -> declarations then stmtseq)
TreeNode* Parse(CompilerInfo* pci, SymbolTable* symbol_table)
{
    ParseInfo parse_info;
    GetNextToken(pci, &parse_info.next_token);

    // first parse declarations (must be at start)
    ParseDeclarations(pci, &parse_info, symbol_table);

    TreeNode* syntax_tree = StmtSeq(pci, &parse_info);

    if(parse_info.next_token.type!=ENDFILE)
        pci->debug_file.Out("Error code ends before file ends");

    return syntax_tree;
}

// PrintTree and DestroyTree ///////////////////////////////////////////////////////

void PrintTree(TreeNode* node, int sh=0)
{
    int i, NSH=3;
    for(i=0;i<sh;i++) printf(" ");

    printf("[%s]", NodeKindStr[node->node_kind]);

    if(node->node_kind==OPER_NODE) printf("[%s]", TokenTypeStr[node->oper]);
    else if(node->node_kind==INT_NODE) printf("[%d]", node->num);
    else if(node->node_kind==REAL_NODE) printf("[%f]", node->real);
    else if(node->node_kind==BOOL_NODE) printf("[%s]", node->boolean? "true":"false");
    else if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE) printf("[%s]", node->id);

    if(node->expr_data_type!=VOID) printf("[%s]", ExprDataTypeStr[node->expr_data_type]);

    printf("\n");

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) PrintTree(node->child[i], sh+NSH);
    if(node->sibling) PrintTree(node->sibling, sh);
}

void DestroyTree(TreeNode* node)
{
    int i;

    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
        if(node->id) delete[] node->id;

    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) DestroyTree(node->child[i]);
    if(node->sibling) DestroyTree(node->sibling);

    delete node;
}

// Analyzer ////////////////////////////////////////////////////////////////////////

// The Analyze function now enforces declaration usage and type rules
void Analyze(TreeNode* node, SymbolTable* symbol_table)
{
    if(!node) return;

    int i;

    // If encountering an identifier node, make sure it's declared and set its expr_data_type
    if(node->node_kind==ID_NODE || node->node_kind==READ_NODE || node->node_kind==ASSIGN_NODE)
    {
        VariableInfo* vi = symbol_table->Find(node->id);
        if(!vi)
        {
            printf("ERROR Undeclared variable %s at line %d\n", node->id, node->line_num);
            throw 0;
        }
        else
        {
            // set node expr_data_type based on variable info
            if(vi->type==VT_INT) node->expr_data_type = INTEGER;
            else if(vi->type==VT_REAL) node->expr_data_type = REAL;
            else node->expr_data_type = BOOLEAN;
        }
    }

    // Recurse into children
    for(i=0;i<MAX_CHILDREN;i++) if(node->child[i]) Analyze(node->child[i], symbol_table);

    // If operator, determine expression type
    if(node->node_kind==OPER_NODE)
    {
        // comparison operators produce boolean
        if(node->oper==EQUAL || node->oper==LESS_THAN) node->expr_data_type=BOOLEAN;
        else
        {
            // arithmetic or AND operator: left and right must be numeric for arithmetic, boolean only for comparisons
            ExprDataType leftType = node->child[0]->expr_data_type;
            ExprDataType rightType = node->child[1]->expr_data_type;
            if(leftType==BOOLEAN || rightType==BOOLEAN)
            {
                printf("ERROR arithmetic operator applied to boolean at line %d\n", node->line_num);
                throw 0;
            }
            else node->expr_data_type = (leftType==REAL || rightType==REAL) ? REAL : INTEGER;
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
    // ID_NODE assigned earlier

    // If-if and repeat conditions must be boolean
    if(node->node_kind==IF_NODE && node->child[0]->expr_data_type!=BOOLEAN)
    {
        printf("ERROR If test must be BOOLEAN at line %d\n", node->line_num);
        throw 0;
    }
    if(node->node_kind==REPEAT_NODE)
    {
        if(node->child[1]->expr_data_type!=BOOLEAN){
            printf("ERROR Repeat test must be BOOLEAN at line %d\n", node->line_num);
            throw 0;
        }
    }

    // Assignments: left side is ID node; ensure types match exactly
    if(node->node_kind==ASSIGN_NODE)
    {
        // left is variable (ID at node->id), right is child[0]
        VariableInfo* vi = symbol_table->Find(node->id);
        if(!vi){
            printf("ERROR Assignment to undeclared variable %s at line %d\n", node->id, node->line_num);
            throw 0;
        }
        else
        {
            ExprDataType rhsType = node->child[0]->expr_data_type;
            ExprDataType lhsType = (vi->type==VT_INT) ? INTEGER : (vi->type==VT_REAL ? REAL : BOOLEAN);

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
    }

    if(node->sibling) Analyze(node->sibling, symbol_table);
}

// Code Generator / Interpreter /////////////////////////////////////////////////

// Evaluate numeric expression as double (promotes ints to real if needed).
// For boolean results (comparisons), returns 1.0 for true, 0.0 for false.
double Evaluate(TreeNode* node, SymbolTable* st, int* int_vars, double* real_vars, bool* bool_vars)
{
    if(node->node_kind==INT_NODE) return (double)node->num;
    if(node->node_kind==REAL_NODE) return node->real;
    if(node->node_kind==BOOL_NODE) return node->boolean ? 1.0 : 0.0;
    if(node->node_kind==ID_NODE)
    {
        VariableInfo* vi = st->Find(node->id);
        if(!vi) { printf("Runtime ERROR: undeclared var %s\n", node->id); throw 0; }
        if(vi->type==VT_INT) return (double)int_vars[vi->memloc];
        if(vi->type==VT_REAL) return real_vars[vi->memloc];
        // using bool in numeric context should have been caught by semantic analyzer
        return bool_vars[vi->memloc] ? 1.0 : 0.0;
    }

    double a=Evaluate(node->child[0], st, int_vars, real_vars, bool_vars);
    double b=Evaluate(node->child[1], st, int_vars, real_vars, bool_vars);

    if(node->oper==EQUAL) return a==b;
    if(node->oper==LESS_THAN) return a<b;
    if(node->oper==PLUS) return a+b;
    if(node->oper==MINUS) return a-b;
    if(node->oper==TIMES) return a*b;
    if(node->oper==DIVIDE) return a/b;
    if(node->oper==POWER) return pow(a,b);
    throw 0;
    return 0.0;
}

// RunProgram: uses separate pools
void RunProgram(TreeNode* node, SymbolTable* st, int* int_vars, double* real_vars, bool* bool_vars)
{
    if(!node) return;

    if(node->node_kind==IF_NODE)
    {
        double cond = Evaluate(node->child[0], st, int_vars, real_vars, bool_vars);
        if(cond != 0) RunProgram(node->child[1], st, int_vars, real_vars, bool_vars);
        else if(node->child[2]) RunProgram(node->child[2], st, int_vars, real_vars, bool_vars);
    }
    else if(node->node_kind==ASSIGN_NODE)
    {
        VariableInfo* vi = st->Find(node->id);
        if(!vi) { printf("Runtime ERROR assignment to undeclared var %s\n", node->id); throw 0;}
        else
        {
            // assignment: evaluate right-hand side as numeric or boolean depending on var type
            double val = Evaluate(node->child[0], st, int_vars, real_vars, bool_vars);
            if(vi->type==VT_INT)
            {
                int_vars[vi->memloc] = val;
            }
            else if(vi->type==VT_REAL)
            {
                real_vars[vi->memloc] = val;
            }
            else // bool
            {
                bool_vars[vi->memloc] = (val!=0);
            }
        }
    }
    else if(node->node_kind==READ_NODE)
    {
        VariableInfo* vi = st->Find(node->id);
        if(!vi) { printf("Runtime ERROR read undeclared var %s\n", node->id); throw 0;}
        else
        {
            if(vi->type==VT_INT)
            {
                printf("Enter %s (int): ", node->id);
                scanf("%d", &int_vars[vi->memloc]);
            }
            else if(vi->type==VT_REAL)
            {
                printf("Enter %s (real): ", node->id);
                scanf("%lf", &real_vars[vi->memloc]);
            }
            else
            {
                printf("Enter %s (bool): ", node->id);
                char input[100];
                scanf("%s", input);
                if (Equals(input, "true"))
                    bool_vars[vi->memloc] = true;
                else if(Equals(input, "false"))
                    bool_vars[vi->memloc] = false;
                else{
                    printf("Runtime ERROR %s can't be assign't to bool (true/false)", node->id);
                    throw 0;
                }
            }
        }
    }
    else if(node->node_kind==WRITE_NODE)
    {
        TreeNode* expr = node->child[0];
        double value = Evaluate(expr, st, int_vars, real_vars, bool_vars);
        if(expr->expr_data_type==REAL)
        {
            printf("Val: %f\n", value);
        }
        else if(expr->expr_data_type==INTEGER)
        {;
            printf("Val: %d\n", (int)value);
        }
        else // boolean printing as 0/1
        {
            printf("Val: %s\n", (value != 0)? "true":"false");
        }
    }
    else if(node->node_kind==REPEAT_NODE)
    {
        do
        {
           RunProgram(node->child[0], st, int_vars, real_vars, bool_vars);
        }
        while(Evaluate(node->child[1], st, int_vars, real_vars, bool_vars) == 0);
    }

    if(node->sibling) RunProgram(node->sibling, st, int_vars, real_vars, bool_vars);
}

// Helper to allocate pools and start runtime
void RunProgram(TreeNode* syntax_tree, SymbolTable* symbol_table)
{
    int i;
    int* int_vars = new int[symbol_table->num_int_vars];
    double* real_vars = new double[symbol_table->num_real_vars];
    bool* bool_vars = new bool[symbol_table->num_bool_vars];

    for(i=0;i<symbol_table->num_int_vars;i++) int_vars[i]=0;
    for(i=0;i<symbol_table->num_real_vars;i++) real_vars[i]=0.0;
    for(i=0;i<symbol_table->num_bool_vars;i++) bool_vars[i]=false;

    RunProgram(syntax_tree, symbol_table, int_vars, real_vars, bool_vars);

    delete[] int_vars;
    delete[] real_vars;
    delete[] bool_vars;
}

// Scanner only //////////////////////////////////////////////////////////////////

void StartScanner(CompilerInfo* pci)
{
    Token token;

    while(true)
    {
        GetNextToken(pci, &token);
        printf("[%d] %s (%s)\n", pci->in_file.cur_line_num, token.str, TokenTypeStr[token.type]); fflush(NULL);
        if(token.type==ENDFILE || token.type==ERROR) break;
    }
}

// Scanner and Compiler //////////////////////////////////////////////////////////

void StartCompiler(CompilerInfo* pci)
{
    TreeNode* syntax_tree=0;

    // Create symbol table BEFORE parsing so Parse can record declarations
    SymbolTable symbol_table;

    syntax_tree = Parse(pci, &symbol_table);

    Analyze(syntax_tree, &symbol_table);

    printf("Symbol Table:\n");
    symbol_table.Print();
    printf("---------------------------------\n"); fflush(NULL);

    printf("Syntax Tree:\n");
    PrintTree(syntax_tree);
    printf("---------------------------------\n"); fflush(NULL);

    printf("Run Program:\n");
    RunProgram(syntax_tree, &symbol_table);
    printf("---------------------------------\n"); fflush(NULL);

    symbol_table.Destroy();
    DestroyTree(syntax_tree);
}

// main ///////////////////////////////////////////////////////////////////////////

int main()
{
    printf("Start main()\n"); fflush(NULL);

    CompilerInfo compiler_info("input.txt", "output.txt", "debug.txt");

    StartCompiler(&compiler_info);

    printf("End main()\n"); fflush(NULL);
    return 0;
}
