#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include <stdlib.h>

#include<iostream>
#include<vector>
#include<iterator>
#include<map>
#include<algorithm>
#include<set>
#define KEYWORDS  5
#define RESERVED  26

using namespace::std;


char current_token[MAX_TOKEN_LENGTH];
int token_length;
token_type t_type;
int line = 1;


void copyFirstsets(char *source ,char *destination);
static int active_token = 0;
static char *reserved[] =
    {   "",
        "IF",
        "WHILE",
        "DO",
        "THEN",
        "PRINT",
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
        "NUM",
        "ID",
        "ERROR",
        "END_OF_TOKEN",
        "END_OF_PROD"
    };


static void skip_space()
{
    char c;

    c = getchar();
    line += (c == '\n');
    while (!feof(stdin) && isspace(c))
    {
        c = getchar();
        line += (c == '\n');
    }
    // return character to input buffer if eof is not reached
    if (!feof(stdin))
    {
        ungetc(c,stdin);
    }
}

static int is_keyword(char *s)
{
    int i;

    for (i = 1; i <= KEYWORDS; i++)
    {
        if (strcmp(reserved[i],s) == 0)
        {
            return i;
        }
    }
    return 0;
}

static token_type scan_number()
{
    char c;

    c = getchar();
    if (isdigit(c))
    {
        if (c == '0')
        {
            current_token[token_length] = c;
            token_length++;
        }
        else
        {
            while (isdigit(c) && token_length < MAX_TOKEN_LENGTH)
            {
                current_token[token_length] = c;
                token_length++;
                c = getchar();
            }
            ungetc(c, stdin);
        }
        current_token[token_length] = '\0';
        return NUM;
    }
    else
    {
        return ERROR;
    }
}

static token_type scan_id_keyword()
{
    char c;
    int k;
    token_type the_type;

    c = getchar();
    if (isalpha(c))
    {
        while (isalnum(c) && token_length < MAX_TOKEN_LENGTH)
        {
            current_token[token_length] = c;
            token_length++;
            c = getchar();
        }
        current_token[token_length] = '\0';
        ungetc(c, stdin);
        k = is_keyword(current_token);
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

    if (active_token)
    {
        active_token = 0;
        return t_type;
    }
    skip_space();
    token_length = 0;
    current_token[0] = '\0';
    c = getchar();
    switch (c)
    {
        case '.': t_type = DOT;        return t_type;
        case '+': t_type = PLUS;       return t_type;
        case '-': t_type = MINUS;      return t_type;
        case '/': t_type = DIV;        return t_type;
        case '*': t_type = MULT;       return t_type;
        case '=': t_type = EQUAL;      return t_type;
        case ':': t_type = COLON;      return t_type;
        case ',': t_type = COMMA;      return t_type;
        case ';': t_type = SEMICOLON;  return t_type;
        case '[': t_type = LBRAC;      return t_type;
        case ']': t_type = RBRAC;      return t_type;
        case '(': t_type = LPAREN;     return t_type;
        case ')': t_type = RPAREN;     return t_type;
        case '<':
            c = getchar();
            if (c == '=')
            {
                t_type = LTEQ;
            }
            else if (c == '>')
            {
                t_type = NOTEQUAL;
            }
            else
            {
                ungetc(c, stdin);
                t_type = LESS;
            }
            return t_type;
       case '>':
            c = getchar();
            if (c == '=')
            {
                t_type = GTEQ;
            }
            else
            {
                ungetc(c, stdin);
                t_type = GREATER;
            }
            return t_type;
       default:
            if (isdigit(c))
            {
                ungetc(c, stdin);
                t_type = scan_number();
            }
            else if (isalpha(c)) // token is either keyword or ID
            {
                ungetc(c, stdin);
                t_type = scan_id_keyword();
            }
            else if (c == EOF)
            {
                t_type = END_OF_FILE;
            }
            else if(c=='#')
             {
               t_type = END_OF_TOKEN;
}
 else if(c=='##'){t_type=END_OF_PROD;}
            else
            {
                t_type = ERROR;
            }
            return t_type;
    }
}

void ungetToken()
{
    active_token = 1;
}

  vector<string> Nonterminals;

map<string,vector<vector<string> > >productions;
map<string,vector<string> >firstsets;
map<string,vector<string> >firstsetsold;
map<string,vector<string> >followsets;
string startsymbol;

map<string,string> epsilon;
void storeproductions(){
while(!feof(stdin))
{
getToken();
if(t_type==27)
{
break;
}
int isEpsilon=1;
string Nontermleft=current_token;
vector<string>rules;
while(!feof(stdin)){
getToken();

if(t_type==27)
{
break;
}
if((strcmp(current_token,"")!=0)){
rules.push_back(current_token);
 isEpsilon=0;}
       
 
}
productions[Nontermleft].push_back(rules);
if(isEpsilon==1){

 epsilon[Nontermleft]="#";
firstsets[Nontermleft].push_back("#");
}

  }
}
void printepsi(){

for (map<string,vector<string> >::iterator proditer=firstsets.begin(); proditer!=firstsets.end(); ++proditer){
 cout<< proditer->first;

        }
    
}



void printfirstsets(){

      for (vector<string>::iterator it =Nonterminals.begin() ; it != Nonterminals.end(); ++it){
    for (map<string,vector<string> >::iterator proditer=firstsets.begin(); proditer!=firstsets.end(); ++proditer){
     
string temp=*it;
string temp1=proditer->first;
if(temp==temp1){
   cout<< "FIRST("<<proditer->first <<") = {"<<" ";
          
              int isfirst=1;
            for (vector<string>::iterator tokennext=proditer->second.begin(); tokennext!=proditer->second.end(); ++tokennext) {
                 string toktemp=*tokennext;
                     
            
           if(isfirst==0){cout<<","<<" ";}
  cout<< *tokennext;
if(toktemp!=""){
    isfirst=0;}
            }

           cout<<" "<<"}"<<endl;
     
        }
}
    
}
}



void printfollowsets(){

      for (vector<string>::iterator it =Nonterminals.begin() ; it != Nonterminals.end(); ++it){
    for (map<string,vector<string> >::iterator proditer=followsets.begin(); proditer!=followsets.end(); ++proditer){
     
string temp=*it;
string temp1=proditer->first;
if(temp==temp1){
   cout<< "FOLLOW("<<proditer->first <<") = {"<<" ";
          
              int isfirst=1;
            for (vector<string>::iterator tokennext=proditer->second.begin(); tokennext!=proditer->second.end(); ++tokennext) {
                 string toktemp=*tokennext;
                     
            
           if(isfirst==0){cout<<","<<" ";}
  cout<< *tokennext;
if(toktemp!=""){
    isfirst=0;}
            }

           cout<<" "<<"}"<<endl;
     
        }
}
    
}
}

bool isEpsilon(string str){
//bool result=find(epsilon.begin(), epsilon.end(),*it)==epsilon.end();
map<string,string>::const_iterator it = epsilon.find(str);


bool result=it!=epsilon.end();
//bool result=
if(result==1){
//cout<< str; contains epsilon
}
return result;


}


bool isEpsilonfirstsets(string str){
map<string,vector<string> >::const_iterator it = firstsets.find(str);
bool res =find(it->second.begin(), it->second.end(), "#")!=it->second.end();
if(res==1){
//contains epsilon
}
return res;
}




bool isNonterminal(string str){
bool res =find(Nonterminals.begin(), Nonterminals.end(), str)!=Nonterminals.end();
if(res==1){//cout<< str; Nonterminal
//cout<< str; 
}
return  res;
}
template<typename T>
void eliminateDuplicates(std::vector<T>& vector){
set<std::string>const uniques(vector.begin(),vector.end());
vector.assign(uniques.begin(),uniques.end());
}
void calculateFirst(){
int start=1;
int x=0;

while(start){

      for (map<string,vector<vector<string> > >::iterator proditer=productions.begin(); proditer!=productions.end(); ++proditer){
        for (vector<vector<string> >::iterator ruleIter=proditer->second.begin(); ruleIter!=proditer->second.end(); ++ruleIter) {    
            for (vector<string>::iterator tokennext=(*ruleIter).begin(); tokennext!=(*ruleIter).end(); ++tokennext) {
                                if(!isNonterminal(*tokennext)){
                           firstsets[proditer->first].push_back(*tokennext);
 eliminateDuplicates(firstsets[proditer->first]);
if(firstsetsold==firstsets){
}
               firstsetsold=firstsets;
x++;
if(x==1000){start=0;}
                             break;
                        }
                       else{
                            string Nonterm=*tokennext;
                  
                           firstsets[proditer->first].insert(firstsets[proditer->first].end(),firstsets[Nonterm].begin(),firstsets[Nonterm].end());
                eliminateDuplicates(firstsets[proditer->first]);
if(firstsetsold==firstsets){
}
  firstsetsold=firstsets;
x++;
if(x==1000){start=0;}
                             if(!isEpsilonfirstsets(Nonterm)){break;}               
                               }                    

            }
         
        }
    }
}}
void calculateFollow(){
int isfound=0;
//int isstart=1;
int x=0;
int start=1;
while(start){
    for (map<string,vector<string>  >::iterator proditer=followsets.begin(); proditer!=followsets.end(); ++proditer){
string Nonterm=proditer->first;
//cout<<Nonterm;
    for (map<string,vector<vector<string> > >::iterator firstiter=productions.begin(); firstiter!=productions.end(); ++firstiter){

   
        for (vector<vector<string> >::iterator ruleIter=firstiter->second.begin(); ruleIter!=firstiter->second.end(); ++ruleIter) {    
            for (vector<string>::iterator tokennext=(*ruleIter).begin(); tokennext!=(*ruleIter).end(); ++tokennext) {  
string follow=*tokennext;
if(startsymbol==Nonterm)
{followsets[proditer->first].push_back("$");
    eliminateDuplicates(followsets[proditer->first]);}

if(Nonterm==follow){
isfound=1;
int itr=1;
while(itr){
if((tokennext+1)!=(*ruleIter).end()){
string Nterm=*(tokennext+1);
if(isNonterminal(Nterm)){
if(!isEpsilonfirstsets(Nterm)){

 followsets[proditer->first].insert(followsets[proditer->first].end(),firstsets[Nterm].begin(),firstsets[Nterm].end());
                eliminateDuplicates(followsets[proditer->first]);
itr=0;
}
else{
if((tokennext+1)!=(*ruleIter).end()){
tokennext=tokennext+1;
}
else{
followsets[proditer->first].push_back(Nterm);
    eliminateDuplicates(followsets[proditer->first]);
itr=0;
}
}}
else{followsets[proditer->first].push_back(Nterm);
    eliminateDuplicates(followsets[proditer->first]);
itr=0;
}
}
else{followsets[proditer->first].insert(followsets[proditer->first].end(),followsets[firstiter->first].begin(),followsets[firstiter->first].end());
    eliminateDuplicates(followsets[proditer->first]);
itr=0;}
}
}
}
            

}

}
//if(isfound==0){followsets[proditer->first].push_back("$");
  //  eliminateDuplicates(followsets[proditer->first]);}
isfound=0;
//isstart=0;

}
x++;
if(x==100){start=0;}
}
}

main(){

int start=1;

while(!feof(stdin))
{
getToken();


if(t_type==27)
{
break;
}
if(strcmp(current_token,"#")!=0){
Nonterminals.push_back(current_token);
if(start==1){
startsymbol=current_token;}
start=0;
firstsets[current_token].push_back("");
followsets[current_token].push_back("");
}

}
storeproductions();
//printepsi();
//printrules();
calculateFirst();
calculateFollow();
//printfirstsets();
//cout<<endl;
printfollowsets();
}
