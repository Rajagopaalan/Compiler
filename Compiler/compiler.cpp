#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>


#include <iostream>
#include <map>
#include <algorithm>
#include <string>
#include <string.h>
#include <vector>
extern "C"{
#include "compiler.h"
}
using namespace ::std ;

 
 
#define DEBUG 1     // 1 => Turn ON debugging, 0 => Turn OFF debugging
 
void debug(const char * format, ...)
{
    va_list args;
    if (DEBUG)
    {
        va_start (args, format);
        vfprintf (stdout, format, args);
        va_end (args);
    }
}
 
//---------------------------------------------------------
// Lexer
#define ALLOC(t) (t*) calloc(1,sizeof(t))
 
char token[MAX_TOKEN_LENGTH];      // token string
TokenType  t_type;                 // token type
bool  activeToken = false;
unsigned int  token_length;
unsigned int  line_no = 1;
vector <StatementNode*> nestedif;
int ifcount;
 
const char *reserved[] =
{
    "",
    "VAR",
    "IF",
    "WHILE",
    "SWITCH",
    "CASE",
    "DEFAULT",
    "print",
    "GOTO",
    "FOR",
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
    "{",
    "}",
    "<>",
    ">",
    "<",
    "ID",
    "NUM",
    "ERROR"
};
 
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
 
    // return character to input buffer if eof is not reached
    if (!feof(stdin))
    {
        ungetc(c, stdin);
    }
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
 
TokenType scan_number()
{
    char c;
 
    c = getchar();
    if (isdigit(c))
    {
        if (c == '0')
        {
            token[token_length] = c;
            token_length++;
        }
        else
        {
            while (isdigit(c))
            {
                token[token_length] = c;
                token_length++;;
                c = getchar();
            }
            ungetc(c, stdin);
        }
        token[token_length] = '\0';
        return NUM;
    }
    else
    {
        return ERROR;
    }
}
 
 
TokenType scan_id_or_keyword()
{
    TokenType t_type;
    char c;
    int k;
 
    c = getchar();
    if (isalpha(c))
    {
        while (isalnum(c) && token_length < MAX_TOKEN_LENGTH)
        {
            token[token_length] = c;
            token_length++;
            c = getchar();
        }
        if (!feof(stdin))
        {
            ungetc(c, stdin);
        }
 
        token[token_length] = '\0';
        k = isKeyword(token);
        if (k == 0)
        {
            t_type = ID;
        }
        else
        {
            t_type = (TokenType) k;
        }
        return t_type;
    }
    else
    {
        return ERROR;
    }
}
 
void ungetToken()
{
    activeToken = true;
}
 
TokenType getToken()
{
    char c;
 
    if (activeToken)
    {
        activeToken = false;
        return t_type;
    }
 
    skipSpace();
    token_length = 0;
    c = getchar();
 
    switch (c)
    {
        case '+':   t_type = PLUS;      break;
        case '-':   t_type = MINUS;     break;
        case '/':   t_type = DIV;       break;
        case '*':   t_type = MULT;      break;
        case '=':   t_type = EQUAL;     break;
        case ':':   t_type = COLON;     break;
        case ',':   t_type = COMMA;     break;
        case ';':   t_type = SEMICOLON; break;
        case '[':   t_type = LBRAC;     break;
        case ']':   t_type = RBRAC;     break;
        case '(':   t_type = LPAREN;    break;
        case ')':   t_type = RPAREN;    break;
        case '{':   t_type = LBRACE;    break;
        case '}':   t_type = RBRACE;    break;
        case '>':   t_type = GREATER;   break;
        case '<':
            c = getchar();
            if (c == '>')
            {
                t_type = NOTEQUAL;
            }
            else
            {
                ungetc(c, stdin);
                t_type = LESS;
            }
            break;
        default :
            if (isdigit(c))
            {
                ungetc(c, stdin);
                t_type = scan_number();
            }
            else if (isalpha(c))
            {
                // token is either a keyword or ID
                ungetc(c, stdin);
                t_type = scan_id_or_keyword();
            }
            else if (c == EOF)
            {
                t_type = END_OF_FILE;
            }
            else
            {
                t_type = ERROR;
            }
            break;
    } // End Switch
    return t_type;
}
 
//---------------------------------------------------------
// Execute

int start=0;
std::map<string,struct ValueNode* > vnode ;

struct ValueNode* assignvalue(){
ValueNode *value_node=ALLOC(struct ValueNode);
if(t_type==ID){

value_node->name=strdup(token);
value_node->value=0;

}

else if(t_type==NUM){

value_node->value=atoi(token);
value_node->name=NULL;

}
else {value_node=NULL;}
return value_node;
}




struct  AssignmentStatement* assignstmt(){
t_type=getToken();

AssignmentStatement *assign_stmt=ALLOC(struct AssignmentStatement);
if(t_type==ID){
if(vnode[token]==NULL){
assign_stmt->left_hand_side=assignvalue();
        vnode[assign_stmt->left_hand_side->name]=assign_stmt->left_hand_side;
}
else
{
assign_stmt->left_hand_side=vnode[token];
}
t_type=getToken();
if(t_type==EQUAL){
t_type=getToken();
if(t_type==NUM || t_type==ID){
if(t_type==NUM){
assign_stmt->operand1=assignvalue();
}
if(t_type==ID){
if(vnode[token]==NULL){
assign_stmt->operand1=assignvalue();
}
else{
assign_stmt->operand1=vnode[token];}
}
t_type=getToken();
if(t_type==SEMICOLON){
assign_stmt->operand2=NULL;
assign_stmt->op=OP_NOOP;
}
else{
if(t_type==PLUS || t_type==MINUS || t_type==DIV || t_type==MULT){
if(t_type==PLUS){
assign_stmt->op=OP_PLUS;
}
if(t_type==MINUS){
assign_stmt->op=OP_MINUS;
}
if(t_type==DIV){
assign_stmt->op=OP_DIV;
}
if(t_type==MULT){
assign_stmt->op=OP_MULT;
}
}
t_type=getToken();
if(t_type==NUM || t_type==ID){
if(t_type==NUM){
assign_stmt->operand2=assignvalue();
}
if(t_type==ID){
if(vnode[token]!=NULL){
assign_stmt->operand2=vnode[token];}
else{
assign_stmt->operand2=assignvalue();
}
}

t_type=getToken();
if(t_type==SEMICOLON){

return  assign_stmt;
}}
else{return NULL;}
}
}
else{return NULL;}
}
else{return NULL;}
}
else{return NULL;}

return assign_stmt;
}


struct IfStatement* ifstmt(){
IfStatement* ifstatement = ALLOC(struct IfStatement);

   ifcount=ifcount+1;

t_type = getToken();

if(t_type==IF){

t_type=getToken();
 if (t_type==NUM || (t_type==ID && vnode[token]==NULL)) {

        ungetToken();

        ifstatement->condition_operand1 = assignvalue();

    }

else if(t_type==ID ){

ifstatement->condition_operand1=vnode[token];
t_type=getToken();


if(t_type==GREATER || t_type==LESS || t_type==NOTEQUAL){
if(t_type==GREATER){

ifstatement->condition_op=CONDITION_GREATER;}
if(t_type==LESS){

ifstatement->condition_op=CONDITION_LESS;
}
if(t_type==NOTEQUAL){
ifstatement->condition_op=CONDITION_NOTEQUAL;
}
t_type=getToken();

 if (t_type==NUM || (t_type==ID && vnode[token]==NULL)) {

        ungetToken();

        ifstatement->condition_operand2 = assignvalue();

    }

if(t_type==ID ){


ifstatement->condition_operand2=vnode[token];
}
getToken();
   if (t_type!=LBRACE) {
        return NULL;
    }


StatementNode* Noopstmt = ALLOC(struct StatementNode);
Noopstmt->type= NOOP_STMT;


ifstatement->true_branch= parse_generate_intermediate_representation();
ifstatement->false_branch= Noopstmt;
StatementNode* tnode = ifstatement->true_branch;
while(tnode->next!=NULL){
tnode=tnode->next;
}
//Noopstmt=ifstatement->true_branch->next;


tnode->next=Noopstmt;
   if (ifcount>0) {
        nestedif.push_back(Noopstmt);
    }
    else{
       Noopstmt->next=parse_generate_intermediate_representation();
    }
    ifcount =ifcount-1;





  //  return ifstatement;


}


}



}
return ifstatement;
}



struct IfStatement* ifwstmt(StatementNode* target){
StatementNode* targ=target;

ifcount=ifcount+1;

IfStatement* ifstatement = ALLOC(struct IfStatement);
//ungetToken();
t_type = getToken();

if(t_type!=WHILE){

 if (t_type==NUM || (t_type==ID && vnode[token]==NULL)) {

      ungetToken();

        ifstatement->condition_operand1 = assignvalue();

    }

else if(t_type==ID ){


ifstatement->condition_operand1=vnode[token];
t_type=getToken();


if(t_type==GREATER || t_type==LESS || t_type==NOTEQUAL){
if(t_type==GREATER){

ifstatement->condition_op=CONDITION_GREATER;}
if(t_type==LESS){

ifstatement->condition_op=CONDITION_LESS;
}
if(t_type==NOTEQUAL){
ifstatement->condition_op=CONDITION_NOTEQUAL;
}
t_type=getToken();

 if (t_type==NUM || (t_type==ID && vnode[token]==NULL)) {

        ungetToken();

        ifstatement->condition_operand2 = assignvalue();

    }

if(t_type==ID ){


ifstatement->condition_operand2=vnode[token];
}
getToken();
getToken();

   if (t_type!=LBRACE) {
        return NULL;
    }
GotoStatement* gotostmt = ALLOC(struct GotoStatement);
//gotostmt->type= GOTO_STMT;
gotostmt->target=targ;
StatementNode* gotstmt = ALLOC(struct StatementNode);
gotstmt->type= GOTO_STMT;
gotstmt->goto_stmt=gotostmt;


StatementNode* Noopstmt = ALLOC(struct StatementNode);
Noopstmt->type= NOOP_STMT;


ifstatement->true_branch= parse_generate_intermediate_representation();
ifstatement->false_branch= Noopstmt;
StatementNode* tnode = ifstatement->true_branch;
while(tnode->next!=NULL){
tnode=tnode->next;
}
//Noopstmt=ifstatement->true_branch->next;


tnode->next=Noopstmt;
   if (ifcount>0) {
        nestedif.push_back(Noopstmt);
    }
    else{
       Noopstmt->next=parse_generate_intermediate_representation();
    }
    ifcount =ifcount-1;
tnode->next=gotstmt;

}


}



}
return ifstatement;
}





struct IfStatement* ifsstmt(){

IfStatement* ifstatement = ALLOC(struct IfStatement);
//ungetToken();
t_type = getToken();

if(t_type==SWITCH){
t_type = getToken();
if(t_type==ID ){


ifstatement->condition_operand1=vnode[token];
t_type=getToken();

if(t_type!=LBRACE){
return NULL;

}
t_type=getToken();

if(t_type==CASE){
t_type=getToken();
if(t_type!=NUM){
return NULL;

}
ifstatement->condition_operand2=assignvalue();

ifstatement->condition_op=CONDITION_NOTEQUAL ;
t_type=getToken();
if(t_type!=COLON){
return NULL;

}
if(t_type!=LBRACE){
return NULL;

}

StatementNode* Noopstmt = ALLOC(struct StatementNode);
Noopstmt->type= NOOP_STMT;
ifstatement->false_branch= parse_generate_intermediate_representation();
StatementNode* tnode = ifstatement->false_branch;
while(tnode->next!=NULL){
tnode=tnode->next;
}
//Noopstmt=ifstatement->true_branch->next;

ifstatement->true_branch= Noopstmt;



Noopstmt->next=parse_generate_intermediate_representation();
tnode->next=Noopstmt;


//getToken();


}
}



}
return ifstatement;
}






struct PrintStatement* printstmt(){
    PrintStatement* print_stmt = ALLOC(struct PrintStatement);
    t_type = getToken();
    if (t_type==NUM || (t_type==ID && vnode[token]==NULL)) {
           print_stmt->id = assignvalue();
    }
    else if (t_type==ID) {
      print_stmt->id = vnode[token];

    }
    else {
        return NULL;
    }
      t_type = getToken();
    if (t_type!=SEMICOLON) {
        return NULL;
    }
    return print_stmt;
}

struct StatementNode*  parse_generate_intermediate_representation(){

struct StatementNode* stmtnode=ALLOC(struct  StatementNode);
 while (nestedif.empty()==false) {
        StatementNode* stmtifs=nestedif.back();
        stmtifs->next=stmtnode;
        nestedif.pop_back();
    }


t_type=getToken();
if(start==0){

ifcount=-1;
while(t_type!=LBRACE){
t_type=getToken();

}
t_type=getToken();
start=1;
}

switch (t_type) {

case ID :
ungetToken();
stmtnode->type=ASSIGN_STMT;
stmtnode->assign_stmt=assignstmt();


break;

case PRINT :


stmtnode->type=PRINT_STMT;
stmtnode->print_stmt=printstmt();


break;
case IF:
ungetToken();
stmtnode->type=IF_STMT;
stmtnode->if_stmt=ifstmt();

break;

case SWITCH:
//stmtnode->type=IF_STMT;
//stmtnode->if_stmt=ifsstmt();


break;

case WHILE:

StatementNode* targ=ALLOC(struct StatementNode);
targ=stmtnode;
//printf("%d",stmtnode->type);
stmtnode->type=IF_STMT;
stmtnode->if_stmt=ifwstmt(targ);


GotoStatement* gotostmt = ALLOC(struct GotoStatement);
//gotostmt->type= GOTO_STMT;
gotostmt->target=targ;

StatementNode* gotstmt = ALLOC(struct StatementNode);
gotstmt->type= GOTO_STMT;
gotstmt->goto_stmt=gotostmt;
break;







}


//printf("%d",stmtnode->type);
/*if(t_type!=RBRACE){



stmtnode->next= parse_generate_intermediate_representation();
}*/

t_type=getToken();
if (t_type==RBRACE) {
        int nexttok=0;
        while (t_type==RBRACE) {
            //        return stmt_node;
            t_type = getToken();
          nexttok=1;
        }
        if (nexttok==1) {
            ungetToken();
        }
        return stmtnode;
    }
    if (t_type!=END_OF_FILE) {
        ungetToken();
        stmtnode->next=parse_generate_intermediate_representation();
    }

return stmtnode;
}

 
void execute_program(struct StatementNode* program)
{
    struct StatementNode* pc = program;
    int op1, op2, result;
 
    while (pc != NULL)
    {
        switch (pc->type)
        {
            case NOOP_STMT:
                pc = pc->next;
                break;
 
            case PRINT_STMT:
                if (pc->print_stmt == NULL)
                {
                    debug("Error: pc points to a print statement but pc->print_stmt is null.\n");
                    exit(1);
                }
                if (pc->print_stmt->id == NULL)
                {
                    debug("Error: print_stmt->id is null.\n");
                    exit(1);
                }
                printf("%d\n", pc->print_stmt->id->value);
                pc = pc->next;
                break;
 
            case ASSIGN_STMT:
                if (pc->assign_stmt == NULL)
                {
                    debug("Error: pc points to an assignment statement but pc->assign_stmt is null.\n");
                    exit(1);
                }
                if (pc->assign_stmt->operand1 == NULL)
                {
                    debug("Error: assign_stmt->operand1 is null.\n");
                    exit(1);
                }
                if (pc->assign_stmt->op != OP_NOOP)
                {
                    if (pc->assign_stmt->operand2 == NULL)
                    {
                        debug("Error: assign_stmt->op requires two operands but assign_stmt->operand2 is null.\n");
                        exit(1);
                    }
                }
                if (pc->assign_stmt->left_hand_side == NULL)
                {
                    debug("Error: assign_stmt->left_hand_side is null.\n");
                    exit(1);
                }
                switch (pc->assign_stmt->op)
                {
                    case OP_PLUS:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 + op2;
                    
                        break;
                    case OP_MINUS:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 - op2;
                        break;
                    case OP_MULT:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 * op2;
                        break;
                    case OP_DIV:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 / op2;
                        break;
                    case OP_NOOP:
                        op1 = pc->assign_stmt->operand1->value;
                        result = op1;
                        break;
                    default:
                        debug("Error: invalid value for assign_stmt->op (%d).\n", pc->assign_stmt->op);
                        exit(1);
                        break;
                }
                pc->assign_stmt->left_hand_side->value = result;
               
                pc = pc->next;
                break;
 
            case IF_STMT:
                if (pc->if_stmt == NULL)
                {
                    debug("Error: pc points to an if statement but pc->if_stmt is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->true_branch == NULL)
                {
                    debug("Error: if_stmt->true_branch is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->false_branch == NULL)
                {
                    debug("Error: if_stmt->false_branch is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->condition_operand1 == NULL)
                {
                    debug("Error: if_stmt->condition_operand1 is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->condition_operand2 == NULL)
                {
                    debug("Error: if_stmt->condition_operand2 is null.\n");
                    exit(1);
                }
                op1 = pc->if_stmt->condition_operand1->value;
                op2 = pc->if_stmt->condition_operand2->value;
                switch (pc->if_stmt->condition_op)
                {
                    case CONDITION_GREATER:
                        if (op1 > op2)
                        {
                            pc = pc->if_stmt->true_branch;
                        }
                        else
                        {
                            pc = pc->if_stmt->false_branch;
                        }
                        break;
                    case CONDITION_LESS:
                        if (op1 < op2)
                        {
                            pc = pc->if_stmt->true_branch;
                        }
                        else
                        {
                            pc = pc->if_stmt->false_branch;
                        }
                        break;
                    case CONDITION_NOTEQUAL:
                        if (op1 != op2)
                        {
                            pc = pc->if_stmt->true_branch;
                        }
                        else
                        {
                            pc = pc->if_stmt->false_branch;
                        }
                        break;
                    default:
                        debug("Error: invalid value for if_stmt->condition_op (%d).\n", pc->if_stmt->condition_op);
                        exit(1);
                        break;
                }
                break;
 
            case GOTO_STMT:
                if (pc->goto_stmt == NULL)
                {
                    debug("Error: pc points to a goto statement but pc->goto_stmt is null.\n");
                    exit(1);
                }
                if (pc->goto_stmt->target == NULL)
                {
                    debug("Error: goto_stmt->target is null.\n");
                    exit(1);
                }
                pc = pc->goto_stmt->target;
                break;
 
            default:
                debug("Error: invalid value for pc->type (%d).\n", pc->type);
                exit(1);
                break;
        }
    }
}
 
int main(int argc, const char * argv[])
{
    // insert code here...
    {
        if (argc > 1)
        {
            FILE * fp = freopen(argv[1], "r", stdin);
            if (fp == NULL)
            {
                perror(argv[1]);
                exit(1);
            }
        }
    }
    struct StatementNode * program;
    program = parse_generate_intermediate_representation();
    execute_program(program);
    return 0;
}
