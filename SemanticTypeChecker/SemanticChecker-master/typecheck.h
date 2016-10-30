#define IMPLICIT 0
#define EXPLICIT 1

#define TYPE_DECL 0
#define VAR_DECL 1

#define TYPE_NAME 0
#define TYPE_ID 1
#define VAR_ID 2
#define COND_ID 3
#define COND_PRI 4
#define STMT 5

#define MAX_SYMBOLS 1000

//-----------------------------//
// Semantic Counting Variables //
//-----------------------------//
int nextTypeNum = 14;
int symbolCount = 0;

struct symbol {
	char* id; // String Token
	int declType; // EXPLICIT or IMPLICIT
	int typeNum; // Type Number for checking
	int form; // TYPE_DECL or VAR_DECL
};

// Declare Array to Store Symbols
struct symbol symbol_table[MAX_SYMBOLS];

//---------------------------//
// Semantic Lookup Functions //
//---------------------------//
int lookup_symbol_table(char* id, int declType, int form, int code, int typeNum);

void lookup_type(struct type_declNode* typeDecl);
void lookup_var(struct var_declNode* varDecl);
void lookup_assign(struct assign_stmtNode* stmt);
void lookup_cond(struct conditionNode* cond);
int lookup_expr(struct exprNode* expr);

//----------------------------//
// Semantic Utility Functions //
//----------------------------//
void semantic_error(int error);
int unify(int t1, int t2);

//--------------------------//
// Semantic Print Functions //
//--------------------------//
bool defined_type(int index);
void print_symbol_table();
void print_types();
