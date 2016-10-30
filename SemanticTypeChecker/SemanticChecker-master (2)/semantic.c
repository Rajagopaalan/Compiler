/*----------------------------------------------------------------------------
Note: the code in this file is not to be shared with anyone or posted online.
(c) Rida Bazzi, 2013
----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEBUG 0
#define TRUE 1
#define FALSE 0

#include "syntax.h"
#include "typecheck.h"
#include "printlist.h"

//----------------------------- token types ------------------------------
#define KEYWORDS 14
#define RESERVED 38
#define VAR 1
#define BEGIN 2
#define END 3
#define ASSIGN 4 
#define IF 5
#define WHILE 6 
#define DO 7
#define THEN 8
#define PRINT 9
#define INT 10
#define REAL 11
#define STRING 12
#define BOOLEAN 13
#define TYPE 14
#define PLUS 15
#define MINUS 16
#define DIV 17
#define MULT 18
#define EQUAL 19
#define COLON 20 
#define COMMA 21
#define SEMICOLON 22
#define LBRAC 23
#define RBRAC 24
#define LPAREN 25
#define RPAREN 26
#define NOTEQUAL 27
#define GREATER 28
#define LESS 29
#define LTEQ 30
#define GTEQ 31
#define DOT 32
#define ID 33
#define NUM 34
#define REALNUM 35
#define ERROR 36
#define LBRACE 37
#define RBRACE 38
#define NOOP 39

//------------------- reserved words and token strings -----------------------
char *reserved[] = {
	"",
	"VAR",
	"BEGIN",
	"END",
	"ASSIGN",
	"IF",
	"WHILE",
	"DO",
	"THEN",
	"print",
	"INT",
	"REAL",
	"STRING",
	"BOOLEAN",
	"TYPE",
	"+",
	"-",
	"/",
	"*",
	"=",
	":",
	",",
	";",
	"[",
	"]",
	"(",
	")",
	"<>",
	">",
	"<",
	"<=",
	">=",
	".",
	"ID",
	"NUM",
	"REALNUM",
	"ERROR",
	"{",
	"}"
};

int printToken(int ttype) {
	if (ttype <= RESERVED) {
		printf("%s\n", reserved[ttype]);
		return 1;
	} else {
		return 0;
	}
}

//---------------------------------------------------------

//---------------------------------------------------------
// Global Variables associated with the next input token
#define MAX_TOKEN_LENGTH 100

char token[MAX_TOKEN_LENGTH]; // token string
int ttype; // token type
int activeToken = FALSE;
int tokenLength;

int line_no = 1;

//----------------------------------------------------------

int skipSpace() {
	char c;

	c = getchar();
	line_no += (c == '\n');

	while (!feof(stdin) && isspace(c)) {
		c = getchar();
		line_no += (c == '\n');
	}

	// return character to input buffer if eof is not reached
	if (!feof(stdin))
		ungetc(c, stdin);
}

int isKeyword(char *s) {
	int i;

	for (i = 1; i <= KEYWORDS; i++)
		if (strcmp(reserved[i], s) == 0)
			return i;
	return FALSE;
}

// ungetToken() simply sets a flag so that when getToken() is called
// the old ttype is returned and the old token is not overwritten 
// NOTE: BETWEEN ANY TWO SEPARATE CALLS TO ungetToken() THERE MUST BE
// AT LEAST ONE CALL TO getToken()
// CALLING TWO ungetToken() WILL NOT UNGET TWO TOKENS  

void ungetToken() {
	activeToken = TRUE;
}

int scan_number() {
	char c;

	c = getchar();

	if (isdigit(c)) { // First collect leading digits before dot
		// 0 is a nNUM by itself
		if (c == '0') {
			token[tokenLength] = c;
			tokenLength++;
			token[tokenLength] = '\0';
		} else {
			while (isdigit(c)) {
				token[tokenLength] = c;
				tokenLength++;
				c = getchar();
			}

			ungetc(c, stdin);
			token[tokenLength] = '\0';
		}

		// Check if leading digits are integer part of a REALNUM
		c = getchar();

		if (c == '.') {
			c = getchar();

			if (isdigit(c)) {
				token[tokenLength] = '.';
				tokenLength++;

				while (isdigit(c)) {
					token[tokenLength] = c;
					tokenLength++;
					c = getchar();
				}

				token[tokenLength] = '\0';

				if (!feof(stdin))
					ungetc(c, stdin);

				return REALNUM;
			} else {
				ungetc(c, stdin); // note that ungetc returns characters on a stack, so we first
				c = '.'; // return the second character and set c to '.' and return c again
				ungetc(c, stdin);
				return NUM;
			}
		} else {
			ungetc(c, stdin);
			return NUM;
		}
	} else {
		return ERROR;
	}
}

int scan_id_or_keyword() {
	int ttype;
	char c;

	c = getchar();

	if (isalpha(c)) {
		while (isalnum(c)) {
			token[tokenLength] = c;
			tokenLength++;
			c = getchar();
		}

		if (!feof(stdin))
			ungetc(c, stdin);

		token[tokenLength] = '\0';
		ttype = isKeyword(token);

		if (ttype == 0)
			ttype = ID;
		return ttype;
	} else
		return ERROR;
}

int getToken() {
	char c;

	if (activeToken) {
		activeToken = FALSE;
		return ttype;
	} // we do not need an else because the function returns in the body 
	// of the if

	skipSpace();
	tokenLength = 0;
	c = getchar();

	switch (c) {
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

			if (c == '=') {
				return LTEQ;
			} else if (c == '>') {
				return NOTEQUAL;
			} else {
				ungetc(c, stdin);
				return LESS;
			}
		case '>':
			c = getchar();

			if (c == '=') {
				return GTEQ;
			} else {
				ungetc(c, stdin);
				return GREATER;
			}
		default:
			if (isdigit(c)) {
				ungetc(c, stdin);
				return scan_number();
			} else if (isalpha(c)) {
				ungetc(c, stdin);
				return scan_id_or_keyword();
			} else if (c == EOF) {
				return EOF;
			} else {
				return ERROR;
			}
	}
}

/*----------------------------------------------------------------------------
SYNTAX ANALYSIS SECTION
----------------------------------------------------------------------------*/
#define PRIMARY 0
#define EXPR 1

/*-------------------------------UTILITIE---------------------------*/
void syntax_error(char* NT, int line_no) {
	printf("Syntax error while parsing %s line %d\n", NT, line_no);
}

//------------------------//
// Display Semantic Error //
//------------------------//
void semantic_error(int error) {
	printf("ERROR CODE %d\n", error);
}

/*--------------------------------------------------------------------
  PRINTING PARSE TREE
---------------------------------------------------------------------*/
void print_parse_tree(struct programNode* program) {
	print_decl(program->decl);
	print_body(program->body);
}

void print_decl(struct declNode* dec) {
	if (dec->type_decl_section != NULL) {
		print_type_decl_section(dec->type_decl_section);
	}

	if (dec->var_decl_section != NULL) {
		print_var_decl_section(dec->var_decl_section);
	}
}

void print_body(struct bodyNode* body) {
	printf("{\n");
	print_stmt_list(body->stmt_list);
	printf("}\n");

}

void print_var_decl_section(struct var_decl_sectionNode* varDeclSection) {
	printf("VAR\n");
	if (varDeclSection->var_decl_list != NULL)
		print_var_decl_list(varDeclSection->var_decl_list);
}

void print_var_decl_list(struct var_decl_listNode* varDeclList) {
	print_var_decl(varDeclList->var_decl);
	if (varDeclList->var_decl_list != NULL)
		print_var_decl_list(varDeclList->var_decl_list);
}

void print_var_decl(struct var_declNode* varDecl) {
	print_id_list(varDecl->id_list);
	printf(": ");
	print_type_name(varDecl->type_name);
	printf(";\n");
}

void print_type_decl_section(struct type_decl_sectionNode* typeDeclSection) {
	printf("TYPE\n");
	if (typeDeclSection->type_decl_list != NULL)
		print_type_decl_list(typeDeclSection->type_decl_list);
}

void print_type_decl_list(struct type_decl_listNode* typeDeclList) {
	print_type_decl(typeDeclList->type_decl);
	if (typeDeclList->type_decl_list != NULL)
		print_type_decl_list(typeDeclList->type_decl_list);
}

void print_type_decl(struct type_declNode* typeDecl) {
	print_id_list(typeDecl->id_list);
	printf(": ");
	print_type_name(typeDecl->type_name);
	printf(";\n");
}

void print_type_name(struct type_nameNode* typeName) {
	if (typeName->type != ID)
		printf("%s ", reserved[typeName->type]);
	else
		printf("%s ", typeName->id);
}

void print_id_list(struct id_listNode* idList) {
	printf("%s ", idList->id);
	if (idList->id_list != NULL) {
		printf(", ");
		print_id_list(idList->id_list);
	}
}

void print_stmt_list(struct stmt_listNode* stmt_list) {
	print_stmt(stmt_list->stmt);
	if (stmt_list->stmt_list != NULL)
		print_stmt_list(stmt_list->stmt_list);

}

void print_assign_stmt(struct assign_stmtNode* assign_stmt) {
	printf("%s ", assign_stmt->id);
	printf("= ");
	print_expression_prefix(assign_stmt->expr);
	printf("; \n");
}

//-----------------------//
// Print While Statement //
//-----------------------//
void print_while_stmt(struct while_stmtNode* while_stmt) {
	printf("WHILE ");
	print_condition(while_stmt->condition);
	print_body(while_stmt->body);
}

void print_stmt(struct stmtNode* stmt) {
	if (stmt->stmtType == ASSIGN) {
		print_assign_stmt(stmt->assign_stmt);
	} else if (stmt->stmtType == WHILE) {
		print_while_stmt(stmt->while_stmt);
	}
}

void print_expression_prefix(struct exprNode* expr) {
	if (expr->tag == EXPR) {
		printf("%s ", reserved[expr->optr]);
		print_expression_prefix(expr->leftOperand);
		print_expression_prefix(expr->rightOperand);
	} else if (expr->tag == PRIMARY) {
		if (expr->primary->tag == ID) {
			printf("%s ", expr->primary->id);
		} else if (expr->primary->tag == NUM) {
			printf("%d ", expr->primary->ival);
		} else if (expr->primary->tag == REALNUM) {
			printf("%.4f ", expr->primary->fval);
		}
	}
}

//-------------------------//
// Print Primary Statement //
//-------------------------//
void print_primary(struct primaryNode* primary) {
	if (primary->tag == ID) {
		printf("%s ", primary->id);
	} else if (primary->tag == NUM) {
		printf("%d ", primary->ival);
	} else if (primary->tag == REALNUM) {
		printf("%.4f ", primary->fval);
	}
	
	return;
}

//---------------------------//
// Print Condition Statement //
//---------------------------//
void print_condition(struct conditionNode* cond) {
	if (cond->right_operand != NULL) {
		printf("%s ", reserved[cond->relop]);
		print_primary(cond->left_operand);
		print_primary(cond->right_operand);	
		printf("\n");
	} else {
		// Rule 37 Applies (cond -> ID)
		// Print ID
		printf("%s\n", cond->left_operand->id);
	}
	
	return;
}

//----------------------------------//
// Semantic Type-Checking Functions //
//----------------------------------//

//----------------------------------------------//
// print_symbol_table Method			//
//						//
// Displays the Symbol Table for Debugging	//
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
					return symbol_table[i].typeNum;
				case TYPE_ID:
					// Type Declaration ID List Search
					// ID found while Error checking
					// Create Semantic Error 0
					semantic_error(0);
					exit(0);
				case VAR_ID:
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
					}

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
					}

					return symbol_table[i].typeNum;
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
				default:
					// Should Never Reach Here
					// Only will occur if incorrect code given
					printf("ERROR UNKNOWN CODE GIVEN");
					break;
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

/*--------------------------------------------------------------------
  CREATING PARSE TREE NODE
---------------------------------------------------------------------*/
struct programNode* make_programNode() {
	return (struct programNode*) malloc(sizeof (struct programNode));
}

struct declNode* make_declNode() {
	return (struct declNode*) malloc(sizeof (struct declNode));
}

struct type_decl_sectionNode* make_type_decl_sectionNode() {
	return (struct type_decl_sectionNode*) malloc(sizeof (struct type_decl_sectionNode));
}

struct var_decl_sectionNode* make_var_decl_sectionNode() {
	return (struct var_decl_sectionNode*) malloc(sizeof (struct var_decl_sectionNode));
}

struct var_declNode* make_var_declNode() {
	return (struct var_declNode*) malloc(sizeof (struct var_declNode));
}

struct type_declNode* make_type_declNode() {
	return (struct type_declNode*) malloc(sizeof (struct type_declNode));
}

struct type_decl_listNode* make_type_decl_listNode() {
	return (struct type_decl_listNode*) malloc(sizeof (struct type_decl_listNode));
}

struct var_decl_listNode* make_var_decl_listNode() {
	return (struct var_decl_listNode*) malloc(sizeof (struct var_decl_listNode));
}

struct id_listNode* make_id_listNode() {
	return (struct id_listNode*) malloc(sizeof (struct id_listNode));
}

struct type_nameNode* make_type_nameNode() {
	return (struct type_nameNode*) malloc(sizeof (struct type_nameNode));
}

struct bodyNode* make_bodyNode() {
	return (struct bodyNode*) malloc(sizeof (struct bodyNode));
}

struct stmt_listNode* make_stmt_listNode() {
	return (struct stmt_listNode*) malloc(sizeof (struct stmt_listNode));
}

struct stmtNode* make_stmtNode() {
	return (struct stmtNode*) malloc(sizeof (struct stmtNode));
}

struct while_stmtNode* make_while_stmtNode() {
	return (struct while_stmtNode*) malloc(sizeof (struct while_stmtNode));
}

struct assign_stmtNode* make_assign_stmtNode() {
	return (struct assign_stmtNode*) malloc(sizeof (struct assign_stmtNode));
}

struct exprNode* make_exprNode() {
	return (struct exprNode*) malloc(sizeof (struct exprNode));
}

struct primaryNode* make_primaryNode() {
	return (struct primaryNode*) malloc(sizeof (struct primaryNode));
}

struct conditionNode* make_conditionNode() {
	return (struct conditionNode*) malloc(sizeof (struct conditionNode));
}

/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------
  PARSING AND BUILDING PARSE TREE
---------------------------------------------------------------------*/
struct primaryNode* primary() {
	struct primaryNode* primary;

	ttype = getToken();

	// Test for Rules 39 - 41
	if (ttype == ID) {
		// Rule 39 Found (Assign primary ID)
		primary = make_primaryNode();
		primary->tag = ttype;
		primary->id = (char *) malloc((tokenLength + 1) * sizeof (char));
		strcpy(primary->id, token);
		return primary;
	} else if (ttype == NUM) {
		// Rule 40 Found (Assign primary NUM)
		primary = make_primaryNode();
		primary->tag = ttype;
		primary->ival = atoi(token);
		return primary;
	} else if (ttype == REALNUM) {
		// Rule 41 Found (Assign primary REALNUM)
		primary = make_primaryNode();
		primary->tag = ttype;
		primary->fval = atof(token);
		return primary;
	} else {
		syntax_error("primary. ID, NUM, or REALNUM expected", line_no);
		exit(0);
	}
}

struct conditionNode* condition() {
	struct conditionNode* cond;

	ttype = getToken();

	if ((ttype == ID) | (ttype == NUM) | (ttype == REALNUM)) {
		switch (ttype) {
			case ID:
				// Rule 37 or 39 must apply
				ungetToken();
				cond = make_conditionNode();
				cond->left_operand = primary();

				ttype = getToken();

				// Test for Rule 37 or Rule 38
				if (ttype == LBRACE) {
					ungetToken();
					cond->right_operand = NULL;
					lookup_cond(cond);
					return cond;
				} else if ((ttype == GREATER) | (ttype == GTEQ) | (ttype == LESS) | (ttype == NOTEQUAL) | (ttype == LTEQ)) {
					cond->relop = ttype;
					cond->right_operand = primary();
					lookup_cond(cond);
					return cond;
				} else {
					syntax_error("condition. GREATER, GTEQ, LESS, NOTEQUAL, LTEQ or LBRACE expected", line_no);
					exit(0);
				}
				break;
			case NUM:
			case REALNUM:
				// Rule 38 must apply
				ungetToken();
				cond = make_conditionNode();
				cond->left_operand = primary();

				ttype = getToken();

				// Check for Rule 42 - 46 (relop)
				if ((ttype == GREATER) | (ttype == GTEQ) | (ttype == LESS) | (ttype == NOTEQUAL) | (ttype == LTEQ)) {
					cond->relop = ttype;
					cond->right_operand = primary();
					lookup_cond(cond);
					return cond;
				} else {
					syntax_error("condition. GREATER, GTEQ, LESS, NOTEQUAL, LTEQ expected", line_no);
					exit(0);
				}
				break;
		}
	} else {
		syntax_error("condition. ID, NUM, or REALNUM expected", line_no);
		exit(0);
	}
}

struct exprNode* factor() {
	struct exprNode* facto;

	ttype = getToken();

	if (ttype == LPAREN) {
		facto = expr();
		ttype = getToken();

		if (ttype == RPAREN) {
			return facto;
		} else {
			syntax_error("factor. RPAREN expected", line_no);
			exit(0);
		}
	} else if (ttype == NUM) {
		facto = make_exprNode();
		facto->primary = make_primaryNode();
		facto->tag = PRIMARY;
		facto->optr = NOOP;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->primary->tag = NUM;
		facto->primary->ival = atoi(token);
		return facto;
	} else if (ttype == REALNUM) {
		facto = make_exprNode();
		facto->primary = make_primaryNode();
		facto->tag = PRIMARY;
		facto->optr = NOOP;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->primary->tag = REALNUM;
		facto->primary->fval = atof(token);
		return facto;
	} else if (ttype == ID) {
		facto = make_exprNode();
		facto->primary = make_primaryNode();
		facto->tag = PRIMARY;
		facto->optr = NOOP;
		facto->leftOperand = NULL;
		facto->rightOperand = NULL;
		facto->primary->tag = ID;
		facto->primary->id = (char *) malloc((tokenLength + 1) * sizeof (char));
		strcpy(facto->primary->id, token);
		return facto;
	} else {
		syntax_error("factor. NUM, REALNUM, or ID, expected", line_no);
		exit(0);
	}
}

struct exprNode* term() {
	struct exprNode* ter;
	struct exprNode* f;

	ttype = getToken();

	if ((ttype == ID) | (ttype == LPAREN) | (ttype == NUM) | (ttype == REALNUM)) {
		ungetToken();
		f = factor();
		ttype = getToken();

		if ((ttype == MULT) | (ttype == DIV)) {
			ter = make_exprNode();
			ter->optr = ttype;
			ter->leftOperand = f;
			ter->rightOperand = term();
			ter->tag = EXPR;
			return ter;
		} else if ((ttype == SEMICOLON) | (ttype == PLUS) | (ttype == MINUS) | (ttype == RPAREN)) {
			ungetToken();
			return f;
		} else {
			syntax_error("term. MULT or DIV expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("term. ID, LPAREN, NUM, or REALNUM expected", line_no);
		exit(0);
	}
}

struct exprNode* expr() {
	struct exprNode* exp;
	struct exprNode* t;

	ttype = getToken();

	if ((ttype == ID) | (ttype == LPAREN) | (ttype == NUM) | (ttype == REALNUM)) {
		ungetToken();
		t = term();
		ttype = getToken();

		if ((ttype == PLUS) | (ttype == MINUS)) {
			exp = make_exprNode();
			exp->optr = ttype;
			exp->leftOperand = t;
			exp->rightOperand = expr();
			exp->tag = EXPR;
			return exp;
		} else if ((ttype == SEMICOLON) | (ttype == MULT) | (ttype == DIV) | (ttype == RPAREN)) {
			ungetToken();
			return t;
		} else {
			syntax_error("expr. PLUS, MINUS, or SEMICOLON expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("expr. ID, LPAREN, NUM, or REALNUM expected", line_no);
		exit(0);
	}
}

struct assign_stmtNode* assign_stmt() {
	struct assign_stmtNode* assignStmt;

	ttype = getToken();

	if (ttype == ID) {
		assignStmt = make_assign_stmtNode();
		assignStmt->id = (char *) malloc((tokenLength + 1) * sizeof (char));
		strcpy(assignStmt->id, token);
		ttype = getToken();

		if (ttype == EQUAL) {
			assignStmt->expr = expr();
			lookup_assign(assignStmt); // Check for Semantic Errors
			return assignStmt;
		} else {
			syntax_error("assign_stmt. EQUAL expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("assign_stmt. ID expected", line_no);
		exit(0);
	}
}

struct while_stmtNode* while_stmt() {
	struct while_stmtNode* whileStmt;

	ttype = getToken();

	// Test for Rule 25
	if (ttype == WHILE) {
		whileStmt = make_while_stmtNode();
		whileStmt->condition = condition();
		whileStmt->body = body();
		return whileStmt;
	} else {
		syntax_error("while_stmt, WHILE expected", line_no);
		exit(0);
	}
}

struct stmtNode* stmt() {
	struct stmtNode* stm;

	ttype = getToken();
	stm = make_stmtNode();

	if (ttype == ID) {
		// assign_stmt
		ungetToken();
		stm->assign_stmt = assign_stmt();
		stm->stmtType = ASSIGN;
		ttype = getToken();

		if (ttype == SEMICOLON) {
			return stm;
		} else {
			syntax_error("stmt. SEMICOLON expected", line_no);
			exit(0);
		}
	} else if (ttype == WHILE) {
		// while_stmt
		ungetToken();
		stm->while_stmt = while_stmt();
		stm->stmtType = WHILE;
		return stm;
	} else {
		// syntax error 
		syntax_error("stmt. ID or WHILE expected", line_no);
		exit(0);
	}
}

struct stmt_listNode* stmt_list() {
	struct stmt_listNode* stmtList;

	ttype = getToken();

	if ((ttype == ID) | (ttype == WHILE)) {
		ungetToken();
		stmtList = make_stmt_listNode();
		stmtList->stmt = stmt();
		ttype = getToken();

		if ((ttype == ID) | (ttype == WHILE)) {
			ungetToken();
			stmtList->stmt_list = stmt_list();
			return stmtList;
		} else {
			// If the next token is not in FOLLOW(stmt_list), 
			// let the caller handle it.
			ungetToken();
			return stmtList;
		}
	} else {
		syntax_error("stmt_list. ID or WHILE expected", line_no);
		exit(0);
	}
}

struct bodyNode* body() {
	struct bodyNode* bod;

	ttype = getToken();

	if (ttype == LBRACE) {
		bod = make_bodyNode();
		bod->stmt_list = stmt_list();

		ttype = getToken();

		if (ttype == RBRACE) {
			return bod;
		} else {
			syntax_error("body. RBRACE expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("body. LBRACE expected", line_no);
		exit(0);
	}
}

struct type_nameNode* type_name() {
	struct type_nameNode* tName;
	tName = make_type_nameNode();

	ttype = getToken();

	if ((ttype == ID) | (ttype == INT) | (ttype == REAL) | (ttype == STRING) | (ttype == BOOLEAN)) {
		tName->type = ttype;

		if (ttype == ID) {
			tName->id = (char *) malloc(tokenLength + 1);
			strcpy(tName->id, token);
		}

		return tName;
	} else {
		syntax_error("type_name. type name expected", line_no);
		exit(0);
	}
}

struct id_listNode* id_list() {
	struct id_listNode* idList;
	idList = make_id_listNode();
	ttype = getToken();

	if (ttype == ID) {
		idList->id = (char*) malloc(tokenLength + 1);
		strcpy(idList->id, token);
		ttype = getToken();

		if (ttype == COMMA) {
			idList->id_list = id_list();
			return idList;
		} else if (ttype == COLON) {
			ungetToken();
			idList->id_list = NULL;
			return idList;
		} else {
			syntax_error("id_list. COMMA or COLON expected", line_no);
			exit(0);
		}
	} else {
		syntax_error("id_list. ID expected", line_no);
		exit(0);
	}

}

struct type_declNode* type_decl() {
	struct type_declNode* typeDecl;
	typeDecl = make_type_declNode();
	ttype = getToken();

	if (ttype == ID) {
		ungetToken();
		typeDecl->id_list = id_list();
		ttype = getToken();

		if (ttype == COLON) {
			typeDecl->type_name = type_name();
			ttype = getToken();

			if (ttype == SEMICOLON) {
				lookup_type(typeDecl);
				return typeDecl;
			} else {
				syntax_error("type_decl. SEMICOLON expected", line_no);
				exit(0);
			}
		}
	} else {
		syntax_error("type_decl. ID expected", line_no);
	}
}

struct var_declNode* var_decl() {
	struct var_declNode* varDecl;
	varDecl = make_var_declNode();
	ttype = getToken();

	if (ttype == ID) {
		ungetToken();
		varDecl->id_list = id_list();
		ttype = getToken();

		if (ttype == COLON) {
			varDecl->type_name = type_name();
			ttype = getToken();

			if (ttype == SEMICOLON) {
				lookup_var(varDecl);
				return varDecl;
			} else {
				syntax_error("var_decl. SEMICOLON expected", line_no);
				exit(0);
			}
		}
	}
}

struct var_decl_listNode* var_decl_list() {
	struct var_decl_listNode* varDeclList;
	varDeclList = make_var_decl_listNode();

	ttype = getToken();

	if (ttype == ID) {
		ungetToken();
		varDeclList->var_decl = var_decl();
		ttype = getToken();

		if (ttype == ID) {
			ungetToken();
			varDeclList->var_decl_list = var_decl_list();
			return varDeclList;
		} else {
			ungetToken();
			return varDeclList;
		}
	} else {
		syntax_error("var_decl_list. ID expected", line_no);
		exit(0);
	}
}

struct type_decl_listNode* type_decl_list() {
	struct type_decl_listNode* typeDeclList;
	typeDeclList = make_type_decl_listNode();

	ttype = getToken();

	if (ttype == ID) {
		ungetToken();
		typeDeclList->type_decl = type_decl();
		ttype = getToken();

		if (ttype == ID) {
			ungetToken();
			typeDeclList->type_decl_list = type_decl_list();
			return typeDeclList;
		} else {
			ungetToken();
			return typeDeclList;
		}
	} else {
		syntax_error("type_decl_list. ID expected", line_no);
		exit(0);
	}
}

struct var_decl_sectionNode* var_decl_section() {
	struct var_decl_sectionNode *varDeclSection;
	varDeclSection = make_var_decl_sectionNode();

	ttype = getToken();

	if (ttype == VAR) {
		// no need to ungetToken() 
		varDeclSection->var_decl_list = var_decl_list();
		return varDeclSection;
	} else {
		syntax_error("var_decl_section. VAR expected", line_no);
		exit(0);
	}
}

struct type_decl_sectionNode* type_decl_section() {
	struct type_decl_sectionNode *typeDeclSection;
	typeDeclSection = make_type_decl_sectionNode();

	ttype = getToken();

	if (ttype == TYPE) {
		typeDeclSection->type_decl_list = type_decl_list();
		return typeDeclSection;
	} else {
		syntax_error("type_decl_section. TYPE expected", line_no);
		exit(0);
	}
}

struct declNode* decl() {
	struct declNode* dec;
	dec = make_declNode();

	ttype = getToken();

	if (ttype == TYPE) {
		ungetToken();
		dec->type_decl_section = type_decl_section();
		ttype = getToken();

		if (ttype == VAR) {
			// type_decl_list is epsilon
			// or type_decl already parsed and the 
			// next token is checked
			ungetToken();
			dec->var_decl_section = var_decl_section();
		} else {
			ungetToken();
			dec->var_decl_section = NULL;
		}
		return dec;
	} else {
		dec->type_decl_section = NULL;

		if (ttype == VAR) {
			// type_decl_list is epsilon
			// or type_decl already parsed and the 
			// next token is checked
			ungetToken();
			dec->var_decl_section = var_decl_section();
			return dec;
		} else {
			if (ttype == LBRACE) {
				ungetToken();
				dec->var_decl_section = NULL;
				return dec;
			} else {
				syntax_error("decl. LBRACE expected", line_no);
				exit(0); // stop after first syntax error
			}
		}
	}
}

struct programNode* program() {
	struct programNode* prog;

	prog = make_programNode();
	ttype = getToken();

	if ((ttype == TYPE) | (ttype == VAR) | (ttype == LBRACE)) {
		ungetToken();
		prog->decl = decl();
		prog->body = body();
		return prog;
	} else {
		syntax_error("program. TYPE or VAR or LBRACE expected", line_no);
		exit(0); // stop after first syntax error
	}
}

// COMMON mistakes:
//    *     = instead of ==
//    *     not allocating space before strcpy
main() {
	struct programNode* parseTree;
	insert_explicit_types();
	parseTree = program();
	if (DEBUG) print_parse_tree(parseTree);
	if (DEBUG) printf("\nSUCCESSFULLY PARSED INPUT!\n\n");
	if (DEBUG) print_symbol_table();
	print_types();
}
