
#ifndef NODE_H
#define NODE_H




#include <stdarg.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <iostream>
using namespace std;


typedef struct node{
    int line;
    char *name;
    char *value;

    int num;
    struct node **children;
} node;

node *new_node(char *name,  int num ,...);
void node_traverse(node *root,int space);
node *newLeaf(char *name, char *value, int line);

#endif