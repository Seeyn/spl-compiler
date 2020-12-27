#ifndef tran_H
#define tran_H

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


void tranProgram(node *root);
void tranExtDefList(node *Node);
void tranExtDef(node* Node);
void tranExtDecList(node* Node,char* type);
Variable* tranVarDec(node* Node,char* type,bool is_struct);
Variable* tranFunDec(node* Node,char* type);
char* tranSpecifier(node *Node);
char* tranStructSpecifier(node* Node);
vector<Variable*> tranDefList(node* Node,bool is_struct);
vector<Variable*> tranDef(node* Node,bool is_struct);
vector<Variable*> tranVarList(node* Node);
Variable* tranParamDec(node* func);
Variable* tranDec(node* Node,char* type,bool is_struct);
vector<Variable*> tranDecList(node* Node,char* type,bool is_struct);
string tranExp(node* Node,int place);
void tranStmt(node* Node,char* type);
void tranStmtList(node *Node,char* type);
void tranCompSt(node* Node,char* type);
void tranArgs(node* Node,char* func_name,vector<int> &args);
void tranCond(node* Node,int label_true,int label_false);
#endif