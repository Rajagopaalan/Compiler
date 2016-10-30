/*----------------------------------------------------------------------------
 Note: the code in this file is not to be shared with anyone or posted online.
 (c) Rida Bazzi, 2015, Adam Doupe, 2015
 ----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "syntax.h"

/* ------------------------------------------------------- */
/* -------------------- LEXER SECTION -------------------- */
/* ------------------------------------------------------- */
#define DEBUG 0
#define TRUE 1
#define FALSE 0

#define KEYWORDS  11
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

int nextTypeNum = 14;
int symbolCount = 0;



struct symbolNode {
	char* id;
	struct symbolNode* next;
};

struct typeNode {
	char* id;
	struct symbolNode* head;
	struct symbolNode* curr;
};

struct typeNode types[MAX_SYMBOLS];






struct symbol {
	char* id; // String Token
	int declType; // EXPLICIT or IMPLICIT
	int typeNum; // Type Number for checking
	int form; // TYPE_DECL or VAR_DECL
};

// Declare Array to Store Symbols
struct symbol symbol_table[MAX_SYMBOLS];

typedef enum
{
    END_OF_FILE = -1, VAR = 1, WHILE, INT, REAL, STRING, BOOLEAN,
    TYPE, LONG, DO, CASE, SWITCH,
    PLUS, MINUS, DIV, MULT, EQUAL,
    COLON, COMMA, SEMICOLON,
    LBRAC, RBRAC, LPAREN, RPAREN, LBRACE, RBRACE,
    NOTEQUAL, GREATER, LESS, LTEQ, GTEQ, DOT,
    ID, NUM, REALNUM,
    ERROR
} token_type;

const char *reserved[] = {"",
    "VAR", "WHILE", "INT", "REAL", "STRING", "BOOLEAN",
    "TYPE", "LONG", "DO", "CASE", "SWITCH",
    "+", "-", "/", "*", "=",
    ":", ",", ";",
    "[", "]", "(", ")", "{", "}",
    "<>", ">", "<", "<=", ">=", ".",
    "ID", "NUM", "REALNUM",
    "ERROR"
};

// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100
char token[MAX_TOKEN_LENGTH]; // token string
token_type t_type; // token type
bool activeToken = false;
int tokenLength;
int line_no = 1;

void skipSpace()
{
    char c;
    
    c = getchar();
    line_no += (c == '\n');
    while (!feof(stdin) && isspace(c))
    {
        c = getchar();
        line_no += (c == '\n');
    }
    ungetc(c, stdin);
}

int isKeyword(char *s)
{
    int i;
    
    for (i = 1; i <= KEYWORDS; i++)
    {
        if (strcmp(reserved[i], s) == 0)
        {
            return i;
        }
    }
    return false;
}

/*
 * ungetToken() simply sets a flag so that when getToken() is called
 * the old t_type is returned and the old token is not overwritten.
 * NOTE: BETWEEN ANY TWO SEPARATE CALLS TO ungetToken() THERE MUST BE
 * AT LEAST ONE CALL TO getToken(). CALLING TWO ungetToken() WILL NOT
 * UNGET TWO TOKENS
 */
void ungetToken()
{
    activeToken = true;
}

token_type scan_number()
{
    char c;
    
    c = getchar();
    if (isdigit(c))
    {
        // First collect leading digits before dot
        // 0 is a NUM by itself
        if (c == '0')
        {
            token[tokenLength] = c;
            tokenLength++;
            token[tokenLength] = '\0';
        }
        else
        {
            while (isdigit(c))
            {
                token[tokenLength] = c;
                tokenLength++;
                c = getchar();
            }
            ungetc(c, stdin);
            token[tokenLength] = '\0';
        }
        // Check if leading digits are integer part of a REALNUM
        c = getchar();
        if (c == '.')
        {
            c = getchar();
            if (isdigit(c))
            {
                token[tokenLength] = '.';
                tokenLength++;
                while (isdigit(c))
                {
                    token[tokenLength] = c;
                    tokenLength++;
                    c = getchar();
                }
                token[tokenLength] = '\0';
                if (!feof(stdin))
                {
                    ungetc(c, stdin);
                }
                return REALNUM;
            }
            else
            {
                ungetc(c, stdin);
                c = '.';
                ungetc(c, stdin);
                return NUM;
            }
        }
        else
        {
            ungetc(c, stdin);
            return NUM;
        }
    }
    else
    {
        return ERROR;
    }
}

token_type scan_id_or_keyword()
{
    token_type the_type;
    int k;
    char c;
    
    c = getchar();
    if (isalpha(c))
    {
        while (isalnum(c))
        {
            token[tokenLength] = c;
            tokenLength++;
            c = getchar();
        }
        if (!feof(stdin))
        {
            ungetc(c, stdin);
        }
        token[tokenLength] = '\0';
        k = isKeyword(token);
        if (k == 0)
        {
            the_type = ID;
        }
        else
        {
            the_type = (token_type) k;
        }
        return the_type;
    }
    else
    {
        return ERROR;
    }
}

token_type getToken()
{
    char c;
    
    if (activeToken)
    {
        activeToken = false;
        return t_type;
    }
    skipSpace();
    tokenLength = 0;
    c = getchar();
    switch (c)
    {
        case '.': return DOT;
        case '+': return PLUS;
        case '-': return MINUS;
        case '/': return DIV;
        case '*': return MULT;
        case '=': return EQUAL;
        case ':': return COLON;
        case ',': return COMMA;
        case ';': return SEMICOLON;
        case '[': return LBRAC;
        case ']': return RBRAC;
        case '(': return LPAREN;
        case ')': return RPAREN;
        case '{': return LBRACE;
        case '}': return RBRACE;
        case '<':
            c = getchar();
            if (c == '=')
            {
                return LTEQ;
            }
            else if (c == '>')
            {
                return NOTEQUAL;
            }
            else
            {
                ungetc(c, stdin);
                return LESS;
            }
        case '>':
            c = getchar();
            if (c == '=')
            {
                return GTEQ;
            }
            else
            {
                ungetc(c, stdin);
                return GREATER;
            }
        default:
            if (isdigit(c))
            {
                ungetc(c, stdin);
                return scan_number();
            }
            else if (isalpha(c))
            {
                ungetc(c, stdin);
                return scan_id_or_keyword();
            }
            else if (c == EOF)
            {
                return END_OF_FILE;
            }
            else
            {
                return ERROR;
            }
    }
}

/* ----------------------------------------------------------------- */
/* -------------------- SYNTAX ANALYSIS SECTION -------------------- */
/* ----------------------------------------------------------------- */

void syntax_error(const char* msg)
{
    printf("Syntax error while parsing %s line %d\n", msg, line_no);
    exit(1);
}

void semantic_error(int error) {
	printf("ERROR CODE %d\n", error);
}
/* -------------------- PRINTING PARSE TREE -------------------- */
void print_parse_tree(struct programNode* program)
{
    print_decl(program->decl);
    print_body(program->body);
}

void print_decl(struct declNode* dec)
{
    if (dec->type_decl_section != NULL)
    {
        print_type_decl_section(dec->type_decl_section);
    }
    if (dec->var_decl_section != NULL)
    {
        print_var_decl_section(dec->var_decl_section);
    }
}

void print_body(struct bodyNode* body)
{
    printf("{\n");
    print_stmt_list(body->stmt_list);
    printf("}\n");
}

void print_var_decl_section(struct var_decl_sectionNode* varDeclSection)
{
    printf("VAR\n");
    if (varDeclSection->var_decl_list != NULL)
    {
        print_var_decl_list(varDeclSection->var_decl_list);
    }
}

void print_var_decl_list(struct var_decl_listNode* varDeclList)
{
    print_var_decl(varDeclList->var_decl);
    if (varDeclList->var_decl_list != NULL)
    {
        print_var_decl_list(varDeclList->var_decl_list);
    }
}

void print_var_decl(struct var_declNode* varDecl)
{
    print_id_list(varDecl->id_list);
    printf(": ");
    print_type_name(varDecl->type_name);
    printf(";\n");
}

void print_type_decl_section(struct type_decl_sectionNode* typeDeclSection)
{
    printf("TYPE\n");
    if (typeDeclSection->type_decl_list != NULL)
    {
        print_type_decl_list(typeDeclSection->type_decl_list);
    }
}

void print_type_decl_list(struct type_decl_listNode* typeDeclList)
{
    print_type_decl(typeDeclList->type_decl);
    if (typeDeclList->type_decl_list != NULL)
    {
        print_type_decl_list(typeDeclList->type_decl_list);
    }
}

void print_type_decl(struct type_declNode* typeDecl)
{
    print_id_list(typeDecl->id_list);
    printf(": ");
    print_type_name(typeDecl->type_name);
    printf(";\n");
}

void print_type_name(struct type_nameNode* typeName)
{
    if (typeName->type != ID)
    {
        printf("%s ", reserved[typeName->type]);
    }
    else
    {
        printf("%s ", typeName->id);
    }
}

void print_id_list(struct id_listNode* idList)
{
    printf("%s ", idList->id);
    if (idList->id_list != NULL)
    {
        printf(", ");
        print_id_list(idList->id_list);
    }
}

void print_stmt_list(struct stmt_listNode* stmt_list)
{
    print_stmt(stmt_list->stmt);
    if (stmt_list->stmt_list != NULL)
    {
        print_stmt_list(stmt_list->stmt_list);
    }
    
}

void print_assign_stmt(struct assign_stmtNode* assign_stmt)
{
    printf("%s ", assign_stmt->id);
    printf("= ");
    print_expression_prefix(assign_stmt->expr);
    printf("; \n");
}

void print_stmt(struct stmtNode* stmt)
{
    switch (stmt->stmtType)
    {
        case ASSIGN:
            print_assign_stmt(stmt->assign_stmt);
            break;
        case WHILE:
            print_while_stmt(stmt->while_stmt);
            break;
        case DO:
            print_do_stmt(stmt->while_stmt);
            break;
        case SWITCH:
            print_switch_stmt(stmt->switch_stmt);
            break;
    }
}

void print_expression_prefix(struct exprNode* expr)
{
    if (expr->tag == EXPR)
    {
        printf("%s ", reserved[expr->op]);
        print_expression_prefix(expr->leftOperand);
        print_expression_prefix(expr->rightOperand);
    }
    else if (expr->tag == PRIMARY)
    {
        if (expr->primary->tag == ID)
        {
            printf("%s ", expr->primary->id);
        }
        else if (expr->primary->tag == NUM)
        {
            printf("%d ", expr->primary->ival);
        }
        else if (expr->primary->tag == REALNUM)
        {
            printf("%.4f ", expr->primary->fval);
        }
    }
}

void print_while_stmt(struct while_stmtNode* while_stmt)
{
    // TODO: implement this for your own debugging purposes
printf("while statement");
print_condition(while_stmt->condition);
printf("\n");
print_body(while_stmt->body);
printf("\n");

}





void print_do_stmt(struct while_stmtNode* do_stmt)
{
    // TODO: implement this for your own debugging purposes
printf("Do statement");
print_body(do_stmt->body);
printf("WHILE");
print_condition(do_stmt->condition);
printf("\n");
}

void print_primary(struct primaryNode* prim){
switch(prim->tag){
case(ID):
printf("%s",prim->id);
case(NUM):
printf("%d",prim->ival);
break;
default:
printf("%f",prim->fval);
}

}





















//----------------------------------------------//
void print_symbol_table() {
	int i;

	printf("\tSYMBOL\tTYPE\t\tNUM\tFORM\n");
	printf("\t======\t====\t\t===\t====\n");

	for (i = 0; i < symbolCount; i++) {
		char* declType = (symbol_table[i].declType) ? "EXPLICIT":"IMPLICIT";
		char* form = (symbol_table[i].form) ? "VAR":"TYPE";

		printf("\t%s\t%s\t%d\t%s\n", symbol_table[i].id, declType, symbol_table[i].typeNum, form);
	}

	return;
}

// defined_type Method
//
//
bool defined_type(int index) {
	return (strcmp(types[index].id, "") == 0) ? false:true;
}


struct symbolNode* add_to_list(struct symbolNode* head, char* id) {
	struct symbolNode* ptr = (struct symbolNode*) malloc(sizeof (struct symbolNode));

	// Make sure Memory Allocated
	if (ptr == NULL) {
		printf("\nERROR Could not create Node\n");
		return NULL;
	}

	// Insert Into Linked List
	if (head == NULL) {
		// Insert First Node
		ptr->id = id;
		ptr->next = NULL;
		head = ptr;
	} else {
		// Insert to End of List
		struct symbolNode* curr = head;

		ptr->id = id;
		ptr->next = NULL;

		while (curr->next != NULL) {
			curr = curr->next;
		}

		curr->next = ptr;
		curr = ptr;
	}

	// Return the head of the Linked List
	if (DEBUG) printf("SYMBOL INSERTED: ID (%s)\n", ptr->id);
	return head;
}


//--------------------------------------------------------------//
// print_types Method						//
//								//
// Creates the Linked Lists of Types defined in the program.	//
// Defines the Type Names and adds Symbols to List.		//
//--------------------------------------------------------------//
void print_types() {
	int i, index;

	// Initialize Types
	for (i = 0; i < (nextTypeNum - 10); i++) {
		types[i].id = "";
		types[i].head = NULL;
		types[i].curr = NULL;
	}	

	if (DEBUG) printf("\n\n");

	// Loop through Symbol Table to gather Defined Types
	for (i = 0; i < symbolCount; i++) {
		index = symbol_table[i].typeNum - 10;

		if (!defined_type(index)) {
			if (DEBUG) printf("TYPE INSERTED: ID (%s)\n", symbol_table[i].id);
			types[index].id = symbol_table[i].id;
		}
	}		

	// Loop through Symbol Table and Add Explicitly Defined Types
	for (i = 0; i < symbolCount; i++) {
		index = symbol_table[i].typeNum - 10;

		if ((symbol_table[i].declType == EXPLICIT) & (symbol_table[i].form == TYPE_DECL) & (strcmp(types[index].id, symbol_table[i].id) != 0)) {
			if (DEBUG) printf("INSERTING: ID (%s) TYPENUM (%d)\n", symbol_table[i].id, symbol_table[i].typeNum);
			types[index].head = add_to_list(types[index].head, symbol_table[i].id);
		}
	}

	// Loop through Symbol Table and Add Implicityl Defined Types
	for (i = 0; i < symbolCount; i++) {
		index = symbol_table[i].typeNum - 10;

		if ((symbol_table[i].declType == IMPLICIT) & (symbol_table[i].form == TYPE_DECL) & (strcmp(types[index].id, symbol_table[i].id) != 0)) {
			if (DEBUG) printf("INSERTING: ID (%s) TYPENUM (%d)\n", symbol_table[i].id, symbol_table[i].typeNum);
			types[index].head = add_to_list(types[index].head, symbol_table[i].id);
		}
	}

	// Loop through Symbol Table and Add Variables
	for (i = 0; i < symbolCount; i++) {
		index = symbol_table[i].typeNum - 10;
		
		if ((symbol_table[i].form == VAR_DECL) & (strcmp(types[index].id, symbol_table[i].id) != 0)) {
			if (DEBUG) printf("INSERTING: ID (%s) TYPENUM (%d)\n", symbol_table[i].id, symbol_table[i].typeNum);
			types[index].head = add_to_list(types[index].head, symbol_table[i].id);
		}
	}

	// Loop through Types and Print Symbols
	for (i = 0; i < (nextTypeNum - 10); i++) {
		if (types[i].head != NULL) {
			printf("%s : ", types[i].id);
			
			struct symbolNode* symbol = types[i].head;

			do {
				printf("%s ", symbol->id);
				symbol = symbol->next;
			} while (symbol != NULL);

			printf("#\n");
		} else {
			if (DEBUG) printf("SKIPPING: No Symbols for Type (%s)\n", types[i].id);
		}
	}

	return;
}

//------------------------------------------------------//
// insert_explicit_types Method				//
//							//
// Static function used to create Built-In types.	//
//------------------------------------------------------//



static void insert_explicit_types() {
	struct symbol integer, real, str, boolean;

	// Declare INT Built-In Type
	integer.id = "INT";
	integer.declType = EXPLICIT;
	integer.typeNum = 10;
	integer.form = TYPE_DECL;
	symbol_table[symbolCount] = integer;
	symbolCount++;

	// Declare REAL Built-In Type
	real.id = "REAL";
	real.declType = EXPLICIT;
	real.typeNum = 11;
	real.form = TYPE_DECL;
	symbol_table[symbolCount] = real;
	symbolCount++;

	// Declare STRING Built-In Type
	str.id = "STRING";
	str.declType = EXPLICIT;
	str.typeNum = 12;
	str.form = TYPE_DECL;
	symbol_table[symbolCount] = str;
	symbolCount++;

	// Declare BOOLEAN Built-In Type
	boolean.id = "BOOLEAN";
	boolean.declType = EXPLICIT;
	boolean.typeNum = 13;
	boolean.form = TYPE_DECL;
	symbol_table[symbolCount] = boolean;
	symbolCount++;
	
	return;
}

//--------------------------------------------------------------//
// unify Method							//
//								//
// Modifies the Type Numbers of Symbols in the Table to match	//
// the number given in parameter t1. Checks for Type Mismatch	//
//--------------------------------------------------------------//
int unify(int t1, int t2) {
	if (t1 == t2) {
		return t1;
	}

	if ((t1 < 14) & (t2 < 14)) {
		// Type Mismatch
		// Throw ERROR CODE 3
		semantic_error(3);
		exit(0);
	} else {
		int i;

		// Loop through Symbols
		for (i = 0; i < symbolCount; i++) {
			// Check for Matching Type Number
			if (symbol_table[i].typeNum == t2) {
				// Assign Type Number for Symbol
				symbol_table[i].typeNum = t1;
			}
		}
	}

	return t1;
}

//----------------------------------------------------------------------//
// lookup_symbol_table Method						//
//									//
// Loops through Symbol Table to find matching ID.			//
// If Not Found insert into table and return Type Number.		//
// Else if in Type Declaration or Variable Declaration ERROR Found.	//
//----------------------------------------------------------------------//









int lookup_symbol_table(char* id, int declType, int form, int code, int typeNum) {
	int i;
	int found = FALSE;

	// Loop through Symbol Table Return Type Number
	for (i = 0; i < symbolCount; i++) {
		if (strcmp(id, symbol_table[i].id) == 0) {
			// ID found in Symbol Table
			found = TRUE;

			switch (code) {
				case TYPE_NAME:

					// Type Name Search
					// Return Type Number
					return symbol_table[i].typeNum;}
				//case TYPE_ID:
					// Type Declaration ID List Search
					// ID found while Error checking
					// Create Semantic Error 0
				semantic_error(0);
					exit(0);
				//case VAR_ID:
				
	// Var Declaration ID List Search
					// ID found while Error checking
					// Create Semantic Error 1 or 2
					if (symbol_table[i].form == TYPE_DECL) {
						// Type re-declared as Variable
						semantic_error(1);
					} else if (symbol_table[i].form == VAR_DECL) {
						// Variable declared more than once
						semantic_error(2);
					} else {
						// Should Never Reach Here
						// Only will occur if Symbol not inserted correctly
						printf("ERROR UNKNOWN SYMBOL FORM");	
					}
					exit(0);
			case COND_ID:
					// Verify ID is not Type re-declaration
					if (symbol_table[i].form == TYPE_DECL) {
						// Type re-declared as Variable
						// Throw ERROR CODE 1
						semantic_error(1);
						exit(0);
					

					// Verify cond -> ID is of Type BOOLEAN
					if (symbol_table[i].typeNum != BOOLEAN) {
						if (symbol_table[i].typeNum < BOOLEAN) {
							// Type Mismatch
							// Throw ERROR CODE 3
							semantic_error(3);
							exit(0);
						} else {
							// Type Implicitly Declared Must be BOOLEAN
							int tempTypeNum = symbol_table[i].typeNum;
							symbol_table[i].typeNum = BOOLEAN;
							return tempTypeNum;
						}
					}

					break;

		case COND_PRI:

					// Primary Type ID found
					// Verify ID is not Type re-declaration
					// Return Type Number
					if (symbol_table[i].form == TYPE_DECL) {
						// Type re-declared as Variable
						// Throw ERROR CODE 1
						semantic_error(1);
						exit(0);
					

					return symbol_table[i].typeNum;}
			case STMT:
					// ID found
					// Verify ID is not Type re-declaration
					// Return Type Number
					if (symbol_table[i].form == TYPE_DECL) {
						// Type re-declared as Variable
						// Throw ERROR CODE 1
						semantic_error(1);
						exit(0);
					}

					return symbol_table[i].typeNum;
				//default:

					// Should Never Reach Here
					// Only will occur if incorrect code given
					printf("ERROR UNKNOWN CODE GIVEN");
					//break;
			}
		}
	}

	// Insert ID into Symbol Table if not found
	if (!found) {
		// ID not found
		struct symbol newSymbol;

		// Assign Symbol Variables
		newSymbol.id = id;
		newSymbol.declType = declType;
		newSymbol.form = form;
		newSymbol.typeNum = typeNum;

		// Insert into Symbol Table
		symbol_table[symbolCount] = newSymbol;

		// Increment Counters
		if ((code == TYPE_NAME) | (code == COND_PRI) | (code == STMT)) {
			nextTypeNum++;
		}

		symbolCount++;

		// Return Type Number
		return symbol_table[i].typeNum;
	}
      return 0;
}

//----------------------------------------------------------------------//
// lookup_type Method							//
//									//
// Type checking for Type Declarations. Looks up typeName, then		//
// inserts into Symbol Table if needed. Looks up idList, then inserts	//
// IDs into Symbol Table. Makes IDs Type Number same as typeName.	//
//----------------------------------------------------------------------//
void lookup_type(struct type_declNode* typeDecl) {
	int typeNum;
	struct id_listNode* id_listNode = typeDecl->id_list;

	// Lookup Type Name
	if (typeDecl->type_name->type == ID) {
		// If Type Name is ID then lookup in Symbol Table
		typeNum = lookup_symbol_table(typeDecl->type_name->id, IMPLICIT, TYPE_DECL, TYPE_NAME, nextTypeNum);
	} else {
		// Else it is a Built-In Type so assign to appropriate Type Number
		typeNum = typeDecl->type_name->type;
	}
	
	// Loop through IDs in ID List
	while (id_listNode != NULL) {
		lookup_symbol_table(id_listNode->id, EXPLICIT, TYPE_DECL, TYPE_ID, typeNum);	// Lookup ID in Symbol Table
		id_listNode = id_listNode->id_list;	// Iterate to next node in ID List
	}

	return;
}

//----------------------------------------------------------------------//
// lookup_var Method							//
//									//
// Type checking for Variable Declarations. Looks up typeName, then	//
// inserts into Symbol Table if needed. Looks up idList, then inserts	//
// IDs into Symbol Table. Makes IDs Type Number same as typeName.	//
//----------------------------------------------------------------------//
void lookup_var(struct var_declNode* varDecl) {
	int typeNum;
	struct id_listNode* id_listNode = varDecl->id_list;

	// Lookup Type Name
	if (varDecl->type_name->type == ID) {
		// If Type Name is ID then lookup in Symbol Table
		typeNum = lookup_symbol_table(varDecl->type_name->id, IMPLICIT, TYPE_DECL, TYPE_NAME, nextTypeNum);
	} else {
		// Else it is a Built-In Type so assign to appropriate Type Number
		typeNum = varDecl->type_name->type;
	}

	// Loop through IDs in ID List
	while (id_listNode != NULL) {
		lookup_symbol_table(id_listNode->id, EXPLICIT, VAR_DECL, VAR_ID, typeNum);	// Lookup ID in Symbol Table
		id_listNode = id_listNode->id_list;	// Iterate to next node in ID List
	}
	
	return;
}

//----------------------------------------------------------------------//
// lookup_expr Method							//
//									//
// Type checking for Expressions as part of the assignment statment.	//
// Looks up Primary Type Numbers and unifies the Type Numbers for non-	//
// matching types. Throw error when attempting to modify Built-In Types	//
//----------------------------------------------------------------------//
int lookup_expr(struct exprNode* expr) {
	int leftTypeNum, rightTypeNum;

	// Check for Primary Node
	if (expr->tag != PRIMARY) {
		leftTypeNum = lookup_expr(expr->leftOperand);
		rightTypeNum = lookup_expr(expr->rightOperand);

		// Unify Types of Expression
		// Return Type Number result of unify
		return unify(rightTypeNum, leftTypeNum);
	} else {
		// If Primary Node Return Type Number
		if (expr->primary->tag == ID) {
			// Rule 39 (primary -> ID)
			// Lookup Type Number and return
			return lookup_symbol_table(expr->primary->id, IMPLICIT, VAR_DECL, STMT, nextTypeNum);
		} else if (expr->primary->tag == NUM) {
			// Rule 40 (primary -> NUM)
			// Return INT (10)
			return INT;
		} else if (expr->primary->tag == REALNUM) {
			// Rule 41 (primary -> REALNUM)
			// Return REAL (11)
			return REAL;
		}
	}
    return 0;
}

//------------------------------------------------------//
// lookup_assign Method					//
//							//
// Type checking for assignment statements. Unify the	//
// Type Numbers of the expression to the ID.		//
//------------------------------------------------------//
void lookup_assign(struct assign_stmtNode* stmt) {
	int idTypeNum, exprTypeNum;

	idTypeNum = lookup_symbol_table(stmt->id, IMPLICIT, VAR_DECL, STMT, nextTypeNum);
	exprTypeNum = lookup_expr(stmt->expr);

	unify(exprTypeNum, idTypeNum);

	return;
}

//--------------------------------------------------------------//
// lookup_cond Method						//
//								//
// Checks for Semantic Type Errors in Condition Statements.	//
// Unify Type Numbers to the same if Rule 38 applies. Unify	//
// Type Numbers to BOOLEAN if Rule 37 applies.			//
//--------------------------------------------------------------//
void lookup_cond(struct conditionNode* cond) {
	int leftTypeNum, rightTypeNum;

	// Check for Condition Rule
	if (cond->right_operand != NULL) {
		// Rule 38 Applies (cond -> primary relop primary)

		// Lookup Left Operand Primary Type
		if (cond->left_operand->tag == ID) {
			leftTypeNum = lookup_symbol_table(cond->left_operand->id, IMPLICIT, VAR_DECL, COND_PRI, nextTypeNum); 
		} else if (cond->left_operand->tag == NUM) {
			leftTypeNum = INT;
		} else if (cond->left_operand->tag == REALNUM) {
			leftTypeNum = REAL;
		}

		// Lookup Right Operand Primary Type
		if (cond->right_operand->tag == ID) {
			rightTypeNum = lookup_symbol_table(cond->right_operand->id, IMPLICIT, VAR_DECL, COND_PRI, nextTypeNum); 
		} else if (cond->right_operand->tag == NUM) {
			rightTypeNum = INT;
		} else if (cond->right_operand->tag == REALNUM) {
			rightTypeNum = REALNUM;
		}

		// Unify Type Numbers
		unify(leftTypeNum, rightTypeNum);
	} else {
		// Rule 37 Applies (cond -> ID)
		leftTypeNum = lookup_symbol_table(cond->left_operand->id, IMPLICIT, VAR_DECL, COND_ID, BOOLEAN);
	
		// Variable declared as User-defined Type
		// Unify Type Numbers
		unify(BOOLEAN, leftTypeNum);
	}
	
	return;
}



































void print_condition(struct conditionNode* condition)
{
    // TODO: implement this for your own debugging purposes
print_primary(condition->left_operand);
if(condition->right_operand!=NULL){
printf("%s",reserved[condition->relop]);
print_primary(condition->right_operand);
}
}


void print_case(struct caseNode* cas)
{
    // TODO: implement this for your own debugging purposes
}

void print_case_list(struct case_listNode* case_list)
{
    // TODO: implement this for your own debugging purposes
}

void print_switch_stmt(struct switch_stmtNode* switc)
{
    // TODO: implement this for your own debugging purposes
}

/* -------------------- PARSING AND BUILDING PARSE TREE -------------------- */

// Note that the following function is not
// called case because case is a keyword in C/C++
struct caseNode* cas()
{
    // TODO: implement this for EC
    return NULL;
}

struct case_listNode* case_list()
{
    // TODO: implement this for EC
return NULL;
}



struct switch_stmtNode* switch_stmt()
{
    return NULL;
}



    // TODO: implement this

// TODO: implement this for EC
struct while_stmtNode* do_stmt()
{
    // TODO: validate this
    struct while_stmtNode *stmtNode;
    stmtNode = ALLOC(struct while_stmtNode);
    t_type = getToken();
    if(t_type == DO){
        t_type = getToken();
        if(t_type == LBRACE){
            ungetToken();
           stmtNode->body = body();
        }
        else{
            syntax_error("do_stmt body expected");
        }
        t_type = getToken();
        if(t_type == WHILE){
            t_type = getToken();
            if(t_type == ID ||t_type == NUM || t_type == REALNUM)
            {
                ungetToken();
                stmtNode->condition = condition();
            }
            else{
               syntax_error("do_stmt condition expected");
            }
            t_type = getToken();
            if(t_type == SEMICOLON)
            {
//                                ungetToken();

            }
            else{
                syntax_error("do_stmt semicolon expected");
            }
        }
       else{
           syntax_error("do_stmt WHILE expected");
      }
     
   }
    else{
        syntax_error("do_stmt DO expected");
    }
    return stmtNode;
}


struct primaryNode* primary()
{
    // TODO: implement this
struct primaryNode* prim;
//prim=ALLOC(struct primaryNode);
t_type=getToken();
if(t_type==ID || t_type==NUM ||t_type==REALNUM){
prim=ALLOC(struct primaryNode);
prim->tag=t_type;
switch(t_type)
{
case(ID):
  prim->id=(char *)malloc(sizeof(token));
  strcpy(prim->id,token);
 break;
case(NUM):
  prim->ival=atoi(token);
  break;
default:
  prim->ival= atoi(token);
}
return prim;
}
else{syntax_error("primary ,ID , NUM , REALNUM expected");
return NULL;
}}

struct conditionNode* condition()
{
    // TODO: implement this
 struct conditionNode *condNode;
  //  condNode = ALLOC(struct conditionNode); 
    t_type = getToken();
    if(t_type == ID || t_type==REALNUM || t_type==NUM){
         ungetToken();
condNode = ALLOC(struct conditionNode); 
        condNode->left_operand=primary();
      token_type curr_token=t_type;
     t_type=getToken();
        if(t_type == GREATER || t_type== GTEQ || t_type==LESS || t_type==NOTEQUAL || t_type==LTEQ){
          //  ungetToken();
            condNode->relop = t_type;
            t_type=getToken();
             if(t_type == ID || t_type == REALNUM || t_type==NUM){
              ungetToken();
              condNode->right_operand=primary();
               
}
else{
            syntax_error("condition stmt. condition primary expected");
        }
}
else if(curr_token==ID && (t_type==SEMICOLON || t_type==LBRACE)) {
               ungetToken();
               condNode->right_operand=NULL;

}
        
else{syntax_error("condition stmt. condition relop expected");
}}
else{syntax_error("condition stmt. ID, primary expected");
}
    //return NULL;
     return condNode;
}

struct while_stmtNode* while_stmt()
{
   
    // TODO: implement this
struct while_stmtNode *whileNode;
  //  whileNode = ALLOC(struct while_stmtNode);
    t_type = getToken();
    if(t_type == WHILE){
        t_type = getToken();
        if((t_type == ID) || (t_type==NUM) || (t_type==REALNUM)){
            ungetToken();
whileNode = ALLOC(struct while_stmtNode);
            whileNode->condition = condition();     
               t_type = getToken();
        if(t_type == LBRACE){
            ungetToken();
           whileNode->body = body();
            return whileNode;
}
else{  syntax_error("while statement body expected");}
}
else{  syntax_error("while statement condition expected");}
}
else{  syntax_error("while statement condition expected");}   

    return NULL;
}

struct exprNode* factor()
{
    struct exprNode* facto;
    
    t_type = getToken();
    if (t_type == LPAREN)
    {
        facto = expr();
        t_type = getToken();
        if (t_type == RPAREN)
        {
            return facto;
        }
        else
        {
            syntax_error("factor. RPAREN expected");
        }
    }
    else if (t_type == NUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = NUM;
        facto->primary->ival = atoi(token);
        return facto;
    }
    else if (t_type == REALNUM)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = REALNUM;
        facto->primary->fval = atof(token);
        return facto;
    }
    else if (t_type == ID)
    {
        facto = ALLOC(struct exprNode);
        facto->primary = ALLOC(struct primaryNode);
        facto->tag = PRIMARY;
        facto->op = NOOP;
        facto->leftOperand = NULL;
        facto->rightOperand = NULL;
        facto->primary->tag = ID;
        facto->primary->id = strdup(token);
        return facto;
    }
    else
    {
        syntax_error("factor. NUM, REALNUM, or ID, expected");
    }
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* term()
{
    struct exprNode* ter;
    struct exprNode* f;
    
    t_type = getToken();
    if (t_type == ID || t_type == LPAREN || t_type == NUM || t_type == REALNUM)
    {
        ungetToken();
        f = factor();
        t_type = getToken();
        if (t_type == MULT || t_type == DIV)
        {
            ter = ALLOC(struct exprNode);
            ter->op = t_type;
            ter->leftOperand = f;
            ter->rightOperand = term();
            ter->tag = EXPR;
            ter->primary = NULL;
            return ter;
        }
        else if (t_type == SEMICOLON || t_type == PLUS ||
                 t_type == MINUS || t_type == RPAREN)
        {
            ungetToken();
            return f;
        }
        else
        {
            syntax_error("term. MULT or DIV expected");
        }
    }
    else
    {
        syntax_error("term. ID, LPAREN, NUM, or REALNUM expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct exprNode* expr()
{
    struct exprNode* exp;
    struct exprNode* t;
    
    t_type = getToken();
    if (t_type == ID || t_type == LPAREN || t_type == NUM || t_type == REALNUM)
    {
        ungetToken();
        t = term();
        t_type = getToken();
        if (t_type == PLUS || t_type == MINUS)
        {
            exp = ALLOC(struct exprNode);
            exp->op = t_type;
            exp->leftOperand = t;
            exp->rightOperand = expr();
            exp->tag = EXPR;
            exp->primary = NULL;
            return exp;
        }
        else if (t_type == SEMICOLON || t_type == MULT ||
                 t_type == DIV || t_type == RPAREN)
        {
            ungetToken();
            return t;
        }
        else
        {
            syntax_error("expr. PLUS, MINUS, or SEMICOLON expected");
        }
    }
    else
    {
        syntax_error("expr. ID, LPAREN, NUM, or REALNUM expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct assign_stmtNode* assign_stmt()
{
    struct assign_stmtNode* assignStmt;
    
    t_type = getToken();
    if (t_type == ID)
    {
        assignStmt = ALLOC(struct assign_stmtNode);
        assignStmt->id = strdup(token);
        t_type = getToken();
        if (t_type == EQUAL)
        {
            assignStmt->expr = expr();
             lookup_assign(assignStmt); 
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                return assignStmt;
            }
            else
            {
                syntax_error("asign_stmt. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("assign_stmt. EQUAL expected");
        }
    }
    else
    {
        syntax_error("assign_stmt. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct stmtNode* stmt()
{
    struct stmtNode* stm;
    
    stm = ALLOC(struct stmtNode);
    t_type = getToken();
    if (t_type == ID) // assign_stmt
    {
        ungetToken();
        stm->assign_stmt = assign_stmt();
        stm->stmtType = ASSIGN;
    }
    else if (t_type == WHILE) // while_stmt
    {
        ungetToken();
        stm->while_stmt = while_stmt();
        stm->stmtType = WHILE;
    }
    else if (t_type == DO)  // do_stmt
    {
        ungetToken();
        stm->while_stmt = do_stmt();
        stm->stmtType = DO;
    }
    else if (t_type == SWITCH) // switch_stmt
    {
        ungetToken();
        stm->switch_stmt = switch_stmt();
        stm->stmtType = SWITCH;
    }
    else
    {
        syntax_error("stmt. ID, WHILE, DO or SWITCH expected");
    }
    return stm;
}

struct stmt_listNode* stmt_list()
{
    struct stmt_listNode* stmtList;
    
    t_type = getToken();
    if (t_type == ID || t_type == WHILE ||
        t_type == DO || t_type == SWITCH)
    {
        ungetToken();
        stmtList = ALLOC(struct stmt_listNode);
        stmtList->stmt = stmt();
        t_type = getToken();
        if (t_type == ID || t_type == WHILE ||
            t_type == DO || t_type == SWITCH)
        {
            ungetToken();
            stmtList->stmt_list = stmt_list();
            return stmtList;
        }
        else // If the next token is not in FOLLOW(stmt_list),
            // let the caller handle it.
        {
            ungetToken();
            stmtList->stmt_list = NULL;
            return stmtList;
        }
    }
    else
    {
        syntax_error("stmt_list. ID, WHILE, DO or SWITCH expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct bodyNode* body()
{
    struct bodyNode* bod;
    
    t_type = getToken();
    if (t_type == LBRACE)
    {
        bod = ALLOC(struct bodyNode);
        bod->stmt_list = stmt_list();
        t_type = getToken();
        if (t_type == RBRACE)
        {
            return bod;
        }
        else
        {
            syntax_error("body. RBRACE expected");
        }
    }
    else
    {
        syntax_error("body. LBRACE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_nameNode* type_name()
{
    struct type_nameNode* tName;
    
    tName = ALLOC(struct type_nameNode);
    t_type = getToken();
    if (t_type == ID || t_type == INT || t_type == REAL ||
        t_type == STRING || t_type == BOOLEAN || t_type == LONG)
    {
        tName->type = t_type;
        if (t_type == ID)
        {
            tName->id = strdup(token);
        }
        else
        {
            tName->id = NULL;
        }
        return tName;
    }
    else
    {
        syntax_error("type_name. type name expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct id_listNode* id_list()
{
    struct id_listNode* idList;
    
    idList = ALLOC(struct id_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        idList->id = strdup(token);
        t_type = getToken();
        if (t_type == COMMA)
        {
            idList->id_list = id_list();
            return idList;
        }
        else if (t_type == COLON)
        {
            ungetToken();
            idList->id_list = NULL;
            return idList;
        }
        else
        {
            syntax_error("id_list. COMMA or COLON expected");
        }
    }
    else
    {
        syntax_error("id_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_declNode* type_decl()
{
    struct type_declNode* typeDecl;
    
    typeDecl = ALLOC(struct type_declNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        typeDecl->id_list = id_list();
        t_type = getToken();
        if (t_type == COLON)
        {
            typeDecl->type_name = type_name();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                lookup_type(typeDecl);
           
                return typeDecl;
            }
            else
            {
                syntax_error("type_decl. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("type_decl. COLON expected");
        }
    }
    else
    {
        syntax_error("type_decl. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_declNode* var_decl()
{
    struct var_declNode* varDecl;
    
    varDecl = ALLOC(struct var_declNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        varDecl->id_list = id_list();
        t_type = getToken();
        if (t_type == COLON)
        {
            varDecl->type_name = type_name();
            t_type = getToken();
            if (t_type == SEMICOLON)
            {
                 lookup_var(varDecl);
                return varDecl;
            }
            else
            {
                syntax_error("var_decl. SEMICOLON expected");
            }
        }
        else
        {
            syntax_error("var_decl. COLON expected");
        }
    }
    else
    {
        syntax_error("var_decl. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_listNode* var_decl_list()
{
    struct var_decl_listNode* varDeclList;
    
    varDeclList = ALLOC(struct var_decl_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        varDeclList->var_decl = var_decl();
        t_type = getToken();
        if (t_type == ID)
        {
            ungetToken();
            varDeclList->var_decl_list = var_decl_list();
            return varDeclList;
        }
        else
        {
            ungetToken();
            varDeclList->var_decl_list = NULL;
            return varDeclList;
        }
    }
    else
    {
        syntax_error("var_decl_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_listNode* type_decl_list()
{
    struct type_decl_listNode* typeDeclList;
    
    typeDeclList = ALLOC(struct type_decl_listNode);
    t_type = getToken();
    if (t_type == ID)
    {
        ungetToken();
        typeDeclList->type_decl = type_decl();
        t_type = getToken();
        if (t_type == ID)
        {
            ungetToken();
            typeDeclList->type_decl_list = type_decl_list();
            return typeDeclList;
        }
        else
        {
            ungetToken();
            typeDeclList->type_decl_list = NULL;
            return typeDeclList;
        }
    }
    else
    {
        syntax_error("type_decl_list. ID expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct var_decl_sectionNode* var_decl_section()
{
    struct var_decl_sectionNode *varDeclSection;
    
    varDeclSection = ALLOC(struct var_decl_sectionNode);
    t_type = getToken();
    if (t_type == VAR)
    {
        // no need to ungetToken()
        varDeclSection->var_decl_list = var_decl_list();
        return varDeclSection;
    }
    else
    {
        syntax_error("var_decl_section. VAR expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct type_decl_sectionNode* type_decl_section()
{
    struct type_decl_sectionNode *typeDeclSection;
    
    typeDeclSection = ALLOC(struct type_decl_sectionNode);
    t_type = getToken();
    if (t_type == TYPE)
    {
        typeDeclSection->type_decl_list = type_decl_list();
        return typeDeclSection;
    }
    else
    {
        syntax_error("type_decl_section. TYPE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct declNode* decl()
{
    struct declNode* dec;
    
    dec = ALLOC(struct declNode);
    dec->type_decl_section = NULL;
    dec->var_decl_section = NULL;
    t_type = getToken();
    if (t_type == TYPE)
    {
        ungetToken();
        dec->type_decl_section = type_decl_section();
        t_type = getToken();
        if (t_type == VAR)
        {
            // type_decl_list is epsilon
            // or type_decl already parsed and the
            // next token is checked
            ungetToken();
            dec->var_decl_section = var_decl_section();
        }
        else
        {
            ungetToken();
            dec->var_decl_section = NULL;
        }
        return dec;
    }
    else
    {
        dec->type_decl_section = NULL;
        if (t_type == VAR)
        {
            // type_decl_list is epsilon
            // or type_decl already parsed and the
            // next token is checked
            ungetToken();
            dec->var_decl_section = var_decl_section();
            return dec;
        }
        else
        {
            if (t_type == LBRACE)
            {
                ungetToken();
                dec->var_decl_section = NULL;
                return dec;
            }
            else
            {
                syntax_error("decl. LBRACE expected");
            }
        }
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

struct programNode* program()
{
    struct programNode* prog;
    
    prog = ALLOC(struct programNode);
    t_type = getToken();
    if (t_type == TYPE || t_type == VAR || t_type == LBRACE)
    {
        ungetToken();
        prog->decl = decl();
        prog->body = body();
        return prog;
    }
    else
    {
        syntax_error("program. TYPE or VAR or LBRACE expected");
    }
    assert(false);
    return NULL; // control never reaches here, this is just for the sake of GCC
}

int main()
{
  struct programNode* parseTree;
    parseTree = program();
    // TODO: remove the next line after you complete the parser
 //   print_parse_tree(parseTree); // This is just for debugging purposes
    // TODO: do type checking & print output according to project specification

	//if (DEBUG) printf("\nSUCCESSFULLY PARSED INPUT!\n\n");
	//if (DEBUG) print_symbol_table();
	//print_types();

    return 0;
}
