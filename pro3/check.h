#ifndef CHECK_H
#define CHECK_H

#include <map>
#include <vector>
using namespace std;
extern FILE* FILE_OUT;

enum TYPE{
    CLASS_PRIMITIVE,
    CLASS_ARRAY,
    CLASS_FUNCTION,
    CLASS_STRUCT
};

class Variable{
public:
    char* type;
    int line;
    int scope;
    TYPE class_type;   
    Variable(char* type,int line,int scope,TYPE class_type)
    : type(type),line(line),scope(scope),class_type(class_type){}
    virtual bool operator==(const Variable& other) const{
        return this->class_type == other.class_type;
    }
};

class Array:public Variable{
public:
    vector<int> size;
    Array(char* type,int line,int scope,TYPE class_type,vector<int> size)
    : Variable::Variable(type,line,scope,class_type),size(size){}
    // bool operator==(const Type& other){
    //     return this->type_id == other.type_id 
    //     && this->name == dynamic_cast<const CStructure&>(other).name;
    // }
    bool operator==(const Variable& other){
        if(other.class_type == this->class_type ){
            bool array_check = true;
            vector<int> other_arr = dynamic_cast<const Array&>(other).size;
            array_check = (other_arr.size()==size.size());
            int i = 0;
            if(array_check){
                while(i<size.size()){
                    if(!other_arr[i]==size[i])
                        return false;
                    i++;
                }
            }
            return true;
        }
        return false;
    }
};

class Function:public Variable{
public:
    vector<Variable*> args;
    Function(char* type,int line,int scope,TYPE class_type,vector<Variable*> args)
    : Variable::Variable(type,line,scope,class_type),args(args){}
};

class Struct:public Variable{
public:
    vector<Variable*> members;
    Struct(char* type,int line,int scope,TYPE class_type,vector<Variable*> members)
    : Variable::Variable(type,line,scope,class_type),members(members){}
}; 

void show_table();
void push_scope();
void pop_scope();

void checkProgram(node *root);
void checkExtDefList(node *Node);
void checkExtDef(node* Node);
void checkExtDecList(node* Node,char* type);
Variable* checkVarDec(node* Node,char* type,bool is_struct);
Variable* checkFunDec(node* Node,char* type);
char* checkSpecifier(node *Node);
char* checkStructSpecifier(node* Node);
vector<Variable*> checkDefList(node* Node,bool is_struct);
vector<Variable*> checkDef(node* Node,bool is_struct);
vector<Variable*> checkVarList(node* Node);
Variable* checkParamDec(node* func);
Variable* checkDec(node* Node,char* type,bool is_struct);
vector<Variable*> checkDecList(node* Node,char* type,bool is_struct);
char* checkExp(node* Node,bool is_array,bool is_left);
void checkStmt(node* Node,char* type);
void checkStmtList(node *Node,char* type);
void checkCompSt(node* Node,char* type);
void checkArgs(node* Node,char* func_name,vector<Variable *> args);
#endif