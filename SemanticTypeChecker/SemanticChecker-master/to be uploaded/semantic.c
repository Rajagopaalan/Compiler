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


#define KEYWORDS  11




int tc = 0;
int iserror=0;
struct tokens {
	char*  tokenname;	        
   	int dt; 
	char* ttype; 
};

struct tokens tl[200];

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


#define MAX_TOKEN_LENGTH 100
char token[MAX_TOKEN_LENGTH]; 
token_type t_type; 
bool activeToken = false;
int tokenLength;
int line_no = 1;
int ndt=8;

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

void err(int error,char* token) {
        iserror=1;
	printf("ERROR CODE %d %s\n", error,token);
}
void err_three(int error,int line) {
       iserror=1;
	printf("ERROR CODE %d %d\n", error,line);
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
if(prim->tag==ID){

printf("%s",prim->id);}
else if(prim->tag==NUM){
printf("%d",prim->ival);
}
else{
printf("%f",prim->fval);
}

}


int checktokens(char* tn, char* ttype,int dt, char* turn) {
	int i=0;
	int islocate = 0;	


	for (i = 0; i < tc; i++) {
		if (strcmp(tn, tl[i].tokenname) == 0) {			
			islocate= 1;


if(turn=="TYPE_NAME" && tl[i].ttype == "var_decl" ){
					err(4 ,tn);
                         exit(0);	}				
                                     
  

     else   if((turn=="VAR_ID" ||turn=="COND_ID"  ||turn=="COND_PRI"  ||turn=="STMT") && tl[i].ttype == "type_decl")
	{
						err(1,tn);
						exit(0);
					}		
	

 else if(turn=="TYPE_ID"){           err(0,tn);
					exit(0);}


  else if (turn=="VAR_ID") {if( tl[i].ttype == "var_decl") {
					err(2,tn);
exit(0);
					} }
                     
				
		
				else if (turn=="COND_ID"){

if(tl[i].dt==INT || tl[i].dt == REAL || tl[i].dt == LONG || tl[i].dt==STRING){                           
							err_three(3,line_no);
							exit(0);
}	}			
                                            
                                              
					
                              
      if (turn=="TYPE_NAME" || turn=="COND_PRI" || turn=="STMT" ||  (turn=="COND_ID" && tl[i].dt!=BOOLEAN)) {
return tl[i].dt;}

}
}
	
	if (islocate!=1) {
				struct tokens newtokens;
		newtokens.tokenname = tn;
		newtokens.ttype = ttype;
		newtokens.dt = dt;		
		tl[tc] = newtokens;		
		if ((turn == "TYPE_NAME") || (turn== "COND_PRI") || (turn=="STMT")) {
			ndt++;
		}
		tc++;		
		return tl[i].dt;
	}
return 0;

}

int checktypes(int q, int w) {
	if (q== w) {
		return w;
	}
 if((q==REAL || q==INT || q==BOOLEAN || q==STRING)  &&(w==REAL || w==INT || w==BOOLEAN || w==STRING) ){
	
	err_three(3,line_no);
		exit(0);
	} else {int i=0;

            while(i<tc){	
			if (tl[i].dt == w) {				
				tl[i].dt = q;}
i++;
}}
	return q;
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

struct caseNode *caseNode;
caseNode=ALLOC(struct caseNode);
t_type=getToken();
if(t_type==CASE){
t_type=getToken();
if(t_type==NUM){
t_type=getToken();
if(t_type==COLON){
t_type=getToken();
if(t_type==LBRACE){
ungetToken();
caseNode->body=body();
}
else{syntax_error("case LBRACE expected");}


}
else{syntax_error("case COLON  expected");}
}
else{syntax_error("case NUM expected");}
}
else{syntax_error("case CASE expected");}

    return caseNode;
}



struct case_listNode* case_list()
{
    // TODO: implement this for EC
struct case_listNode *case_list_Node;
case_list_Node=ALLOC(struct case_listNode);
t_type=getToken();
if(t_type==CASE){
ungetToken();
case_list_Node->cas=cas();
t_type=getToken();
if(t_type==CASE){
ungetToken();
case_list_Node->case_list=case_list();
}
ungetToken();
}
else{syntax_error("case_list CASE expected");}




return case_list_Node;
}



struct switch_stmtNode* switch_stmt()
{
//switch_stmt → SWITCH ID LBRACE case_list RBRACE
int temps;
struct switch_stmtNode *switch_Node;
switch_Node=ALLOC(struct switch_stmtNode);
t_type=getToken();

if(t_type==SWITCH){
t_type=getToken();
if(t_type==ID){
switch_Node->id=strdup(token);
temps=checktokens(switch_Node->id, "var_decl", INT,"COND_PRI");
checktypes(INT,temps);
if(t_type==LBRACE){


switch_Node->case_list=case_list();
t_type=getToken();
if(t_type==RBRACE){
//ungetToken();

}
else{syntax_error("switch stmt RBRACE expected");}
}
else{syntax_error("switch_stmt LBRACE expected");}
}
else{syntax_error("switch stmt ID expected");}
}
else{syntax_error("switch stmt SWITCH expected");}

 return switch_Node;
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
  prim->fval= atoi(token);
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
int temptoken1,temptoken2;
if(condNode->right_operand!=NULL){
if(condNode->left_operand->tag==ID){

temptoken1=checktokens(condNode->left_operand->id,"var_decl",ndt,"COND_PRI");

}
else if(condNode->left_operand->tag==NUM){temptoken1=INT;}
else if(condNode->left_operand->tag==REALNUM){temptoken1=REAL;}
if(condNode->right_operand->tag==ID)
{temptoken2=checktokens(condNode->right_operand->id,"var_decl",ndt,"COND_PRI");}

else if(condNode->right_operand->tag==NUM){temptoken2=INT;}
else if(condNode->right_operand->tag==REALNUM){temptoken2=REAL;}
checktypes(temptoken2,temptoken1);}
else{temptoken1=checktokens(condNode->left_operand->id,"var_decl",BOOLEAN,"COND_ID");
checktypes(BOOLEAN,temptoken1);

}
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
{    struct assign_stmtNode* assignStmt;
        t_type = getToken();

    if (t_type == ID)
    {        assignStmt = ALLOC(struct assign_stmtNode);
        assignStmt->id = strdup(token);
        t_type = getToken();
        if (t_type == EQUAL)
        {
            assignStmt->expr = expr();

int a, s;

	
a=checktokens(assignStmt->id, "var_decl", ndt,"STMT");




int wdt,edt;
int exprvalue1=0;

  struct exprNode *ro= assignStmt->expr;
           struct exprNode *lo=assignStmt->expr;
         if(assignStmt->expr->tag!=PRIMARY){
         
       while(ro->tag!=PRIMARY){
         ro=ro->rightOperand;
}
     
       while(lo->tag!=PRIMARY){
         lo=lo->rightOperand;
}
 if(lo->primary->tag==ID){


exprvalue1=checktokens(lo->primary->id,"var_decl",ndt,"STMT");
}
else if(lo->primary->tag==NUM){exprvalue1=INT;}
else{exprvalue1=REAL;}  



 if(ro->primary->tag==ID){


exprvalue1=checktokens(ro->primary->id,"var_decl",ndt,"STMT");
}
else if(ro->primary->tag==NUM){exprvalue1=INT;}
else{exprvalue1=REAL;} }
else{
if(lo->primary->tag==ID){

exprvalue1=checktokens(lo->primary->id,"var_decl",ndt,"STMT");
}
else if(lo->primary->tag==NUM){exprvalue1=INT;}
else{exprvalue1=REAL;}  



 if(ro->primary->tag==ID){


exprvalue1=checktokens(ro->primary->id,"var_decl",ndt,"STMT");
}
else if(ro->primary->tag==NUM){exprvalue1=INT;}
else{exprvalue1=REAL;} 
}
 
checktypes(exprvalue1,a);



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
        idList->id= strdup(token);
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
          int dt;
	struct id_listNode* id_listNode = typeDecl->id_list;	
	if (typeDecl->type_name->type == ID) {		
		dt = checktokens(typeDecl->type_name->id, "type_decl",ndt,"TYPE_NAME");
	} else {		
		dt = typeDecl->type_name->type;
	}
	
	while (typeDecl->id_list!= NULL) {
		checktokens(typeDecl->id_list->id, "type_decl", dt,"TYPE_ID");	
		typeDecl->id_list = typeDecl->id_list->id_list;	
	}


           
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
                
	int dt;
	
	if (varDecl->type_name->type == ID) {
		
		dt = checktokens(varDecl->type_name->id, "type_decl",  ndt,"TYPE_NAME");
	} else {
		
		dt = varDecl->type_name->type;
	}

	
	while (varDecl->id_list != NULL) {
		checktokens(varDecl->id_list->id, "var_decl", dt,"VAR_ID");	
		varDecl->id_list= varDecl->id_list->id_list;	
	}
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

 
if(iserror==0){printf("All systems go!\n");}

    return 0;
}
