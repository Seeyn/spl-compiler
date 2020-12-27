
#include "node.h"
#include <iostream>
using namespace std;
node *new_node(char *name,   int num ,...){
   
    node *Node = (node*)malloc(sizeof(node));
    // printf("%s \n",name);
    Node->name = name;
    
    Node->num = num;
    Node->children = NULL;
    if(num>0){
        node **childs = (node**)calloc(num,sizeof(node)); 
        va_list valist;
        va_start(valist, num);
        node *tmp = va_arg(valist, node*);
        Node->line = tmp->line;
        childs[Node->num-num] = tmp;
        num--;
       while(num>0){
           node *tmp = va_arg(valist, node*);
           childs[Node->num-num] = tmp;
           num--;
       }
       Node->children = childs;
    }
    
    return Node;
}
node *newLeaf(char *name, char *value, int line){
    
    node *Node = (node*)malloc(sizeof(node));
    // printf("%d",line);
    Node->name = name;
    Node->value = value;
    Node->line = line;
    // printf("%s",name);
    return Node;
}
void node_traverse(node *root,int space){
    // printf("1");
    if(root->name != NULL){
        for(int i =0;i<space;i++){
            printf("  ");
        }
        if(root->line != -1) {
            printf ("%s", root->name);
        }
        if(!strcmp(root->name, "TYPE") ||
                !strcmp(root->name, "ID") ||
                !strcmp(root->name, "INT") ||
                !strcmp(root->name, "FLOAT") ||
                !strcmp(root->name, "CHAR")) {
            printf(": %s", root->value);
        }
        if(!strcmp(root->name, "Program") ||
                !strcmp(root->name, "ExtDefList") ||
                !strcmp(root->name, "ExtDef") ||
                !strcmp(root->name, "ExtDecList") ||
                !strcmp(root->name, "Specifier") ||
                !strcmp(root->name, "StructSpecifier") ||
                !strcmp(root->name, "VarDec") ||
                !strcmp(root->name, "FunDec") ||
                !strcmp(root->name, "VarList") ||
                !strcmp(root->name, "ParamDec") ||
                !strcmp(root->name, "CompSt") ||
                !strcmp(root->name, "StmtList") ||
                !strcmp(root->name, "Stmt") ||
                !strcmp(root->name, "DefList") ||
                !strcmp(root->name, "Def") ||
                !strcmp(root->name, "DecList") ||
                !strcmp(root->name, "Dec") ||
                !strcmp(root->name, "Exp") ||
                !strcmp(root->name, "Args")) {
                     printf(" (%d)", root->line);   
                    }
       
        printf("\n");
        if(root->children==NULL)
            return;
        for(int i =0;i<root->num;i++){
            // printf("1\n");
            node_traverse(root->children[i],space+1);        
        }
    }
}

// int main(void){
//     node *tmp1 = newLeaf("root","tmp1",1);
//     node *tmp2 = newLeaf("root","tmp2",1);
//     node *tmp3 = newLeaf("root","tmp3",1);
//     node *root = new_node("root","name",3,tmp1,tmp2,tmp3);
//     node_traverse(root);
//     return 0;
// }