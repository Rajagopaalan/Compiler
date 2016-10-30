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
extern "C" {
         #include "compiler.h"
     }

using namespace ::std ;



#define ALLOC(t) (t*) calloc(1,sizeof(t))
vector < StatementNode* > nestedif;
int ifcount;


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




struct StatementNode* ifsstmt(){
        ValueNode* casevaluel=ALLOC(struct ValueNode);

        ValueNode* casevaluer=ALLOC(struct ValueNode);
  int isfirst=1;
    StatementNode* sstmt=ALLOC(struct StatementNode);
    sstmt->type=NOOP_STMT;


    StatementNode* tstmt=ALLOC(struct StatementNode);
    StatementNode* tnode=ALLOC(struct StatementNode);
tstmt=NULL;
  
    t_type = getToken();
    if (t_type==ID) {
        casevaluel = vnode[token];
    }
    else {
        return NULL;
    }
    
    t_type=getToken();
    if (t_type!=LBRACE) {
        return NULL;
    }
    t_type=getToken();
    if (t_type!=CASE) {
        return NULL;
    }
    

  
    while (t_type==CASE || t_type==DEFAULT){
       

    if(t_type==CASE){
        t_type=getToken();
        if (t_type!=NUM) {
            return NULL;
        }
        ungetToken();
     
        casevaluer=assignvalue();
    }
    else{
        casevaluer=casevaluel;
    }
    IfStatement* ifstmt = ALLOC(struct IfStatement);

    ifstmt->condition_operand1=casevaluel;
       ifstmt->condition_operand2=casevaluer;
 ifstmt->condition_op=CONDITION_NOTEQUAL;
    t_type=getToken();
    if (t_type!=COLON) {
        return NULL;
    }
    t_type=getToken();
    if(t_type!=LBRACE){
        return NULL;
    }
    
    StatementNode*fstmt=ALLOC(struct StatementNode);
   fstmt->type=NOOP_STMT;
   fstmt->next=parse_generate_intermediate_representation();
    ifstmt->false_branch=fstmt;


        if (isfirst==1) {
            isfirst=0;
            StatementNode* caseone = ALLOC(StatementNode);
            caseone->type=IF_STMT;
            caseone->if_stmt=ifstmt;

            sstmt=caseone;
        }
        if (tstmt!=NULL) {
           tstmt->type=IF_STMT;
           tstmt->if_stmt=ifstmt;
        }
       tstmt=ALLOC(struct StatementNode);
        tnode = ifstmt->false_branch;
        while (tnode->next!=NULL) {
            tnode=tnode->next;
        }
        ifstmt->true_branch=tstmt;
        tnode->next=tstmt;
        t_type=getToken();
    }
    if (tnode!=NULL) {
       nestedif.push_back(tnode);
       nestedif.push_back(tstmt);
    }
    ungetToken();
    return sstmt;
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

stmtnode=ifsstmt();


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
