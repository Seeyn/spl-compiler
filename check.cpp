#include "node.h"
#include "check.h"
using namespace std;



int global_scope;
multimap<string,Variable*> variable_table;
//name,pair<fields,scope>
multimap<string,pair<vector<Variable*>,int>> struct_table;

void show_table(){
    printf("########### variable table ############\n");
    for(auto it= variable_table.begin();it != variable_table.end();it++){
        printf("%s :%s  line:%d scope:%d TYPE:%d\n",it->first.c_str(),it->second->type,it->second->line,it->second->scope,it->second->class_type);
    }
    printf("########### structure table ############\n");
    for(auto it= struct_table.begin();it != struct_table.end();it++){
        printf("%s\n",it->first.c_str());
    }
}
void push_scope(){
    global_scope++;
    return;
}
void pop_scope(){
    //pop 元素
    for(auto it = struct_table.begin(); it != struct_table.end(); it++){
        if(it->second.second == global_scope){
            struct_table.erase(it);
            it = struct_table.begin();
            it--;
        }
    }

    for(auto it = variable_table.begin(); it != variable_table.end(); it++){
        if(it->second->scope == global_scope){
            variable_table.erase(it);
            it = variable_table.begin();
            it--;
        }
    }
    global_scope--;
    return;
}

void checkProgram(node *root){
    global_scope = 0;
    struct_table.clear();
    variable_table.clear();
    // printf("start!\n");
    checkExtDefList(root->children[0]);
    return;
}

void checkExtDefList(node *Node){
    /*
    ExtDefList: ExtDef ExtDefList {
        $$ = new_node((char*)"ExtDefList", 2, $1, $2);
    }
    | %empty {
        $$ = new_node(NULL, 0);
    }
    */
//    printf("checkExtDefList\n");
   if(Node->num>0){
       checkExtDef(Node->children[0]);
       checkExtDefList(Node->children[1]);
   }
   return;
}

void checkExtDef(node* Node){
    /*
    ExtDef: Specifier ExtDecList SEMI {
        $$ = new_node((char*)"ExtDef", 3, $1, $2, $3);
    }
    | Specifier SEMI {
        $$ = new_node((char*)"ExtDef", 2, $1, $2);
    }
    | Specifier FunDec CompSt {
        $$ = new_node((char*)"ExtDef", 3, $1, $2, $3);
    }
    */
//    printf("checkExtDef\n");
   char* type = checkSpecifier(Node->children[0]);
   //这里可以确认是不是struct 但是array? primitive? 不知道
   //果然还是得把type传下去
   if(!strcmp(Node->children[1]->name,(char*)"ExtDecList")){
       checkExtDecList(Node->children[1],type);
   }else if(!strcmp(Node->children[1]->name,(char*)"FunDec")){
       checkFunDec(Node->children[1],type);
       checkCompSt(Node->children[2],type);
   }
}
void checkCompSt(node* Node,char* type){
    /*
    CompSt: LC DefList StmtList RC {
        $$ = new_node((char*)"CompSt", 4, $1, $2, $3, $4);
    }
    */
    push_scope();
    checkDefList(Node->children[1],false);
    // printf("target : %s\n",type);
    checkStmtList(Node->children[2],type);
    pop_scope();
}

void checkStmtList(node *Node,char* type){
    /*
    StmtList: Stmt StmtList {
        $$ = new_node((char*)"StmtList", 2, $1, $2);
    } 
    | %empty {
        $$ = new_node(NULL, 0);
    }
    */
   if(Node->line>1){
       checkStmt(Node->children[0],type);
       checkStmtList(Node->children[1],type);
   }
}

void checkStmt(node* Node,char* type){
    /*
    Stmt: Exp SEMI {
        $$ = new_node((char*)"Stmt", 2, $1, $2);
    }
    | CompSt {
        $$ = new_node((char*)"Stmt", 1, $1);
    }
    | RETURN Exp SEMI {
        $$ = new_node((char*)"Stmt", 3, $1, $2, $3);
    }
    | IF LP Exp RP Stmt %prec LOWER_ELSE {
        $$ = new_node((char*)"Stmt", 5, $1, $2, $3, $4, $5);
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        $$ = new_node((char*)"Stmt", 7, $1, $2, $3, $4, $5, $6, $7);
    }
    | WHILE LP Exp RP Stmt {
        $$ = new_node((char*)"Stmt", 5, $1, $2, $3, $4, $5);
    }
    */
   if(Node->num==2){
       checkExp(Node->children[0],false,false);
   }else if(Node->num==1){
       checkCompSt(Node->children[0],type);
   }else if(Node->num==3){
    //    printf("%s\n",type);
       char* return_type = checkExp(Node->children[1],false,false);
        if(strcmp(return_type,type)&&strcmp(return_type,"False")){
            fprintf(FILE_OUT,"Error type 8 at Line %d: incompatiable return type\n",Node->line);
        }
   }else if(Node->num==7){
       checkExp(Node->children[2],false,false);
       checkStmt(Node->children[4],type);
       checkStmt(Node->children[6],type);
   }else{
       checkExp(Node->children[2],false,false);
       checkStmt(Node->children[4],type);
   }
   return;
}

void checkExtDecList(node* Node,char* type){
    /*
    ExtDecList: VarDec {
        $$ = new_node((char*)"ExtDecList", 1, $1);
    }
    | VarDec COMMA ExtDecList {
        $$ = new_node((char*)"ExtDecList", 3, $1, $2, $3);
    }
    */
    // fprintf(FILE_OUT,"checkExtDecList\n");
    if(Node->num >1){
        checkVarDec(Node->children[0],type,false);
        checkExtDecList(Node->children[2],type);
    }else{
        checkVarDec(Node->children[0],type,false);
    }
    return;
}

Variable* checkVarDec(node* Node,char* type,bool is_struct=false){
    /*
    VarDec: ID {
        $$ = new_node((char*)"VarDec", 1, $1);
    }
    | VarDec LB INT RB {
        $$ = new_node((char*)"VarDec", 4, $1, $2, $3, $4);
    }
    */
//    printf("checkVarDec :%d\n",Node->line);
   node* current = Node;
   
   if(!strcmp(type,"int")||!strcmp(type,"float")||!strcmp(type,"char")){
       //在这里定义普通变量与数组
    if(Node->num>1){
        vector<int> size;
        while(current->num>1){
            size.push_back(atoi(current->children[2]->value));
            current=current->children[0];
        }
        //定义数组
        char* name = current->children[0]->value;
        if(variable_table.find(name)!=variable_table.end()&&variable_table.find(name)->second->scope==global_scope&&!is_struct)
            fprintf(FILE_OUT,"Error type 3 at Line %d: redefine variable: %s\n",Node->line,name);
        Array* array = new Array(type,current->line,global_scope,CLASS_ARRAY,size);
        variable_table.insert(make_pair(name,array));
        return array;
    }else{
        //定义变量
        // char* name = current->children[0]->value;
        char* name = current->children[0]->value;
        if(variable_table.find(name)!=variable_table.end()&&variable_table.find(name)->second->scope==global_scope&&!is_struct){
            fprintf(FILE_OUT,"Error type 3 at Line %d: redefine variable: %s\n",Node->line,name);
        }
        Variable* var = new Variable(type,current->line,global_scope,CLASS_PRIMITIVE);
        variable_table.insert(make_pair(name,var));
        return var;
    }
   }else{
       //定义结构体
        char* name; 
        Struct* var; 
        if(Node->num>1){
            vector<int> size;
            while(current->num>1){
                size.push_back(atoi(current->children[2]->value));
                current=current->children[0];
            }
            name = current->children[0]->value;
            vector<Variable*> members = struct_table.find(name)->second.first;
            var = new Struct(type,current->line,global_scope,CLASS_ARRAY,members);
        }else{
            name = current->children[0]->value;
            vector<Variable*> members = struct_table.find(name)->second.first;
            var = new Struct(type,current->line,global_scope,CLASS_STRUCT,members);
        }
        
        variable_table.insert(make_pair(name,var));
        return var;
   }
   
}

Variable* checkFunDec(node* Node,char* type){
    /*
    FunDec: ID LP VarList RP {
        $$ = new_node((char*)"FunDec", 4, $1, $2, $3, $4);
    }
    | ID LP RP {
        $$ = new_node((char*)"FunDec", 3, $1, $2, $3);
    }
    */
   //ID 的 value应该就是名字
    char* name = Node->children[0]->value;
    if(variable_table.find(name)!=variable_table.end())
        fprintf(FILE_OUT,"Error type 4 at Line %d: redefine function: %s\n",Node->line,name);
    //在这里初始化func 然后再在varlist里面更新东西
    vector<Variable*> args{};
    if(Node->num>3){
        args = checkVarList(Node->children[2]);
    }
    Function* func = new Function(type,Node->line,global_scope,CLASS_FUNCTION,args); 
    variable_table.insert(make_pair(name,func));
    return func;
    
}
char* checkSpecifier(node *Node){
    /*
    Specifier: TYPE {
         $$ = new_node((char*)"Specifier", 1, $1);
    } 
    | StructSpecifier {
         $$ = new_node((char*)"Specifier", 1, $1);
    }
    */
    if(!strcmp(Node->children[0]->name,"TYPE")){
        return Node->children[0]->value;
    }else{
        return checkStructSpecifier(Node->children[0]);
    }
    
}

char* checkStructSpecifier(node* Node){
    /*
    StructSpecifier: STRUCT ID LC DefList RC {
        $$ = new_node((char*)"StructSpecifier", 5, $1, $2, $3, $4, $5);
    }
    |
    STRUCT ID {
        $$ = new_node((char*)"StructSpecifier", 2, $1, $2);
    }
    */
   char* name = Node->children[1]->value;
   if(Node->num>2){
       //定义结构体
       if(struct_table.find(name)!=struct_table.end()){
           fprintf(FILE_OUT,"Error type 15 at Line %d: redefine struct: %s\n",Node->line,name);
       }else{
           vector<Variable*> deflist = checkDefList(Node->children[3],true);
           struct_table.insert(make_pair(name,make_pair(deflist,global_scope)));
       }
   }else{
       //这里不是插入结构体,而是确认结构体的存在,但加入变量符号表还是在Vardec
       if(struct_table.find(name)!=struct_table.end()){
           if(struct_table.find(name)->second.second <= global_scope){
               return name;
           }
       }else{
           //error 找不到struct定义
       } 
   }
   //返回结构体的名字
   return name;
}

vector<Variable*> checkDefList(node* Node,bool is_struct){
    /*
    DefList: Def DefList {
    $$ = new_node((char*)"DefList", 2, $1, $2);
    }
    | %empty {
    $$ = new_node(NULL, 0);
    }
    */
   vector<Variable*> deflist;
   while(Node->num>0){
       vector<Variable*> declist = checkDef(Node->children[0],is_struct);
       deflist.insert(deflist.end(),declist.begin(),declist.end()); 
       Node = Node->children[1];
   }
   return deflist;
}
vector<Variable*> checkDef(node* Node,bool is_struct){
    /*
    Def: Specifier DecList SEMI {
        $$ = new_node((char*)"Def", 3, $1, $2, $3);
    }
    */
   char* type = checkSpecifier(Node->children[0]);
   vector<Variable*> declist =  checkDecList(Node->children[1],type,is_struct);
   return declist;
}

vector<Variable*> checkVarList(node* Node){
    /*
    VarList: ParamDec COMMA VarList {
       $$ = new_node((char*)"VarList", 3, $1, $2, $3);
    }
    | ParamDec {
       $$ = new_node((char*)"VarList", 1, $1);
    }
    */
   node* current = Node;
   vector<Variable*> variable_list;
   while(current->num>1){
       variable_list.push_back(checkParamDec(current->children[0]));
       current = current->children[2];
   }
   variable_list.push_back(checkParamDec(current->children[0]));
   return variable_list;
}

Variable* checkParamDec(node* func){
    /*
    ParamDec: Specifier VarDec {
        $$ = new_node((char*)"ParamDec", 2, $1, $2);
    }
    */
   //ParamDec 可能是var,array,struct..是个问题
   //把func的节点传进来,根据Specifier判断是不是struct
   //数组与普通变量都是Variable
    char* type = checkSpecifier(func->children[0]);
    // printf("checkParamDec\n");
    return checkVarDec(func->children[1],type,false);
}

vector<Variable*> checkDecList(node* Node,char* type,bool is_struct){
    /*
    DecList: Dec {
        $$ = new_node((char*)"DecList", 1, $1);
    }
    | Dec COMMA DecList {
        $$ = new_node((char*)"DecList", 3, $1, $2, $3);
    }
    */
    // printf("checkDecList\n");
    node* current = Node;
    vector<Variable*> variable_list;
    while(current->num>1){
        // printf("nop\n");
        variable_list.push_back(checkDec(current->children[0],type,is_struct));
        current = current->children[2];
    } 
    // printf("nop\n");
    variable_list.push_back(checkDec(current->children[0],type,is_struct));
    return variable_list;
}

Variable* checkDec(node* Node,char* type,bool is_struct){
    /*
    Dec: VarDec {
        $$ = new_node((char*)"Dec", 1, $1);
    }
    | VarDec ASSIGN Exp {
        $$ = new_node((char*)"Dec", 3, $1, $2, $3);
    }
    */
    // char* type = checkSpecifier(Node->children[0]);
    // return checkVarDec(func->children[1],type);
    // printf("checkDec\n");
    Variable* var = checkVarDec(Node->children[0],type,is_struct);
    if(Node->num>1){
        char* type = checkExp(Node->children[2],false,false);
        //这里要判断一下左右两边的类型
        if(strcmp(type,var->type)&&strcmp(type,"False")){
            fprintf(FILE_OUT,"Error type 5 at Line %d: unmatching type on both sides of assignment\n",Node->line);
        }
    }
    return var;
}

char* checkExp(node* Node,bool is_array=false,bool is_left=false){
    /*
    Exp: Exp ASSIGN Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp AND Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp OR Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp LT Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp LE Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp GT Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp GE Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp NE Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp EQ Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp PLUS Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp MINUS Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp MUL Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp DIV Exp {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | LP Exp RP {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | MINUS Exp {$$ = new_node((char*)"Exp", 2, $1);}
    | NOT Exp {$$ = new_node((char*)"Exp", 2, $1, $2);}
    | ID LP Args RP {$$ = new_node((char*)"Exp", 4, $1, $2, $3, $4);}
    | ID LP RP {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | Exp LB Exp RB {$$ = new_node((char*)"Exp", 4, $1, $2, $3, $4);}
    | Exp DOT ID {$$ = new_node((char*)"Exp", 3, $1, $2, $3);}
    | ID {$$ = new_node((char*)"Exp", 1, $1);}
    | INT {$$ = new_node((char*)"Exp", 1, $1);}
    | FLOAT {$$ = new_node((char*)"Exp", 1, $1);}
    | CHAR {$$ = new_node((char*)"Exp", 1, $1);}
    | error {}
    */
   //Args 就是输入进函数的参数,要确定类型和注册的是对的上的.
//    printf("checkExp\n");
    
    char* first_token = Node->children[0]->name;
    // printf("%s : %d\n",first_token,Node->line);
    // printf("%s: %d\n",first_token,Node->children[0]->line);
    if(!strcmp("LP",first_token)||!strcmp("MINUS",first_token)||!strcmp("NOT",first_token)){
        // printf("%s\n",Node->children[1]->name);
        if(!strcmp("MINUS",first_token)||!strcmp("NOT",first_token)){
            if(is_left)
                fprintf(FILE_OUT,"Error type 6 at Line %d: left side in assignment is rvalue\n",Node->line);
        }
        return checkExp(Node->children[1]);
    }
    if(Node->num == 1){
        if(!strcmp("ID",first_token)){
            //先查下有没有被定义

            char* name = Node->children[0]->value;
            if(variable_table.find(name)==variable_table.end()){
                // show_table();
                fprintf(FILE_OUT,"Error type 1 at Line %d: undefined variable: %s\n",Node->children[0]->line,name);
                return (char*)"False";
            }else{
                if(variable_table.find(name)->second->class_type != 1&& is_array)
                    fprintf(FILE_OUT,"Error type 10 at Line %d: indexing on non-array variable\n",Node->line);
                return variable_table.find(name)->second->type;
            }
        }else{            
            if(is_array)
                fprintf(FILE_OUT,"Error type 10 at Line %d: indexing on non-array variable\n",Node->line);
            if(is_left)
                fprintf(FILE_OUT,"Error type 6 at Line %d: left side in assignment is rvalue\n",Node->line);
            if(!strcmp("INT",first_token)){
                return (char*)"int";
            }
                
            if(!strcmp("CHAR",first_token))
                return (char*)"char";
            return (char*)"float";
        }
    }
    if(!strcmp("ID",first_token)){
        char* name = Node->children[0]->value;
        if(variable_table.find(name)==variable_table.end()){
            fprintf(FILE_OUT,"Error type 2 at Line %d: undefined function: %s\n",Node->children[0]->line,name);
            return (char*)"False";
        }
        if(variable_table.find(name)->second->class_type!=2){
            fprintf(FILE_OUT,"Error type 11 at Line %d: invoking non-function variable: %s\n",Node->children[0]->line,name);
            return (char*)"False";
        }
        if(Node->num>3){
            //说明有参数
            checkArgs(Node->children[2],name,dynamic_cast<Function*>(variable_table.find(name)->second)->args);
        }else{
            //说明没有参数
            //这里还要处理一下没有参数的情况,没有的话可以自己写个测试
            if(dynamic_cast<Function*>(variable_table.find(name)->second)->args.size()>0){
                fprintf(FILE_OUT,"Error type 9 at Line %d: expect argument",Node->line);
            }
        }
        return variable_table.find(name)->second->type;
    }
    if(Node->num == 3){
        if(strcmp(Node->children[1]->name,"DOT")){
            char* leftType;
            if(is_left)
                fprintf(FILE_OUT,"Error type 6 at Line %d: left side in assignment is rvalue\n",Node->line);
            if(!strcmp(Node->children[1]->name,"ASSIGN")){
                leftType = checkExp(Node->children[0],false,true);
            }else{
                leftType = checkExp(Node->children[0]);
            }
            char* rightType = checkExp(Node->children[2]);
            if(!strcmp(leftType,rightType)){
                return leftType;
            }else if(strcmp(leftType,"False")&&strcmp(rightType,"False")){
                //左右类型不符合
                fprintf(FILE_OUT,"Error type 5 at Line %d: unmatching type on both sides of assignment\n",Node->line);
            }
            if(strcmp(rightType,"char") && strcmp(rightType,"int") && strcmp(rightType,"float")&&strcmp(rightType,"False")){
                fprintf(FILE_OUT,"Error type 7 at Line %d: binary operation on non-number variables\n",Node->line);
            }
            if(strcmp(leftType,"char") && strcmp(leftType,"int") && strcmp(leftType,"float")&&strcmp(leftType,"False")){
                fprintf(FILE_OUT,"Error type 7 at Line %d: binary operation on non-number variables\n",Node->line);
            }
        }else{
            //处理dot
            char* type = checkExp(Node->children[0]);
            //判断是不是struct
            if(!strcmp(type,"int")||!strcmp(type,"float")||!strcmp(type,"char"))
                fprintf(FILE_OUT,"Error type 13 at Line %d: accessing with non-struct variable\n",Node->line);
            char* name = Node->children[2]->value;
            if(variable_table.find(name)==variable_table.end()){
                fprintf(FILE_OUT,"Error type 14 at Line %d: no such member: %s\n",Node->line,name);
                return (char*)"False";
            }
            type = variable_table.find(name)->second->type;
            if(is_array&&variable_table.find(name)->second->class_type!=1){
                fprintf(FILE_OUT,"Error type 10 at Line %d: indexing on non-array variable\n",Node->line);
            }
            return type;
        }
    }
    if(Node->num == 4){
        char* type = checkExp(Node->children[0],true);
        if(strcmp(checkExp(Node->children[2],false),"int"))
            fprintf(FILE_OUT,"Error type 12 at Line %d: indexing by non-integer\n",Node->line);
        return type;
    }
    return (char*)"False";
}

void checkArgs(node* Node,char* func_name,vector<Variable *> args){
    /*
    Args: Exp COMMA Args {$$ = new_node((char*)"Args", 3, $1, $2, $3);}
    | Exp {$$ = new_node((char*)"Args", 1, $1);}
    */
    node* current = Node;
    //没有确认类型,只确认数量
    int count = 1;
    while(current->num > 1){
        char* type = checkExp(current->children[0]);
        if(strcmp(type,args[count-1]->type))
            fprintf(FILE_OUT,"Error type 9 at Line %d: the %d_th parameter has invalid argument type for %s  , expect %s, got %s\n",Node->line,count,func_name,args[count-1]->type,type);
        current = current->children[2];
        count += 1;
    }
    if(count!=args.size()){
        fprintf(FILE_OUT,"Error type 9 at Line %d: invalid argument number for %s, expect %ld, got %d\n",Node->line,func_name,args.size(),count);
    }
}



