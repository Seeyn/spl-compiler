#include "node.h"
#include "translate.h"
using namespace std;

map<string,int> var_table;
// map<string,int> tmp_table;
int label_num;
int tmp_num;
vector<string> codes;

string converttmp(int tmp){
    return "t"+to_string(tmp);
}

string convertvar(int var){
    return "v"+to_string(var);
}

string assignOp(string left,string right){
    return left + " := " + right + " ";
}
string labelOp(int index){
    return "LABEL label"+to_string(index)+" : ";
}
string gotoOp(int index){
    return "GOTO label"+to_string(index)+" ";
}
void show(){
    int count = codes.size();
    for (int i = 0; i < count;i++)
    {
        fprintf(FILE_OUT,"%s\n",codes[i].c_str());
    }
}
void tranProgram(node *root){

    var_table.clear();
    // printf("start! ");
    // read_num = 0;
    label_num = 0;
    tranExtDefList(root->children[0]);
    show();
    return;
}

void tranExtDefList(node *Node){
    /*
    ExtDefList: ExtDef ExtDefList {
        $$ = new_node((char*)"ExtDefList", 2, $1, $2);
    }
    | %empty {
        $$ = new_node(NULL, 0);
    }
    */
//    printf("tranExtDefList ");
   if(Node->num>0){
       tranExtDef(Node->children[0]);
       tranExtDefList(Node->children[1]);
   }
   return;
}

void tranExtDef(node* Node){
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
//    printf("tranExtDef ");
   char* type = tranSpecifier(Node->children[0]);
   //这里可以确认是不是struct 但是array? primitive? 不知道
   //果然还是得把type传下去
   if(!strcmp(Node->children[1]->name,(char*)"ExtDecList")){
       tranExtDecList(Node->children[1],type);
   }else if(!strcmp(Node->children[1]->name,(char*)"FunDec")){
       tranFunDec(Node->children[1],type);
       tranCompSt(Node->children[2],type);
   }
}
void tranCompSt(node* Node,char* type){
    /*
    CompSt: LC DefList StmtList RC {
        $$ = new_node((char*)"CompSt", 4, $1, $2, $3, $4);
    }
    */
    tranDefList(Node->children[1],false);
    // printf("target : %s ",type);
    tranStmtList(Node->children[2],type);
}

void tranStmtList(node *Node,char* type){
    /*
    StmtList: Stmt StmtList {
        $$ = new_node((char*)"StmtList", 2, $1, $2);
    }
    | %empty {
        $$ = new_node(NULL, 0);
    }
    */
   if(Node->line>1){
       tranStmt(Node->children[0],type);
       tranStmtList(Node->children[1],type);
   }
}

void tranStmt(node* Node,char* type){
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
       tmp_num += 1;
       int t1 = tmp_num;
       tranExp(Node->children[0],t1);
   }else if(Node->num==1){
       tranCompSt(Node->children[0],type);
   }else if(Node->num==3){
       tmp_num += 1;
       int t1 = tmp_num;
       string code1 = tranExp(Node->children[1],t1);
       codes.push_back("RETURN t"+to_string(t1)+" ");

   }else if(!strcmp(Node->children[0]->name,"WHILE")){
       label_num+=3;
       int label1 = label_num-2;
       int label2 = label_num-1;
       int label3 = label_num;
       codes.push_back(labelOp(label1));
       tranCond(Node->children[2],label2,label3);
       codes.push_back(labelOp(label2));
       tranStmt(Node->children[4],type);
       codes.push_back(gotoOp(label1));
       codes.push_back(labelOp(label3));
   }else if(Node->num==7 ){
    //    printf("%s: ",Node->children[6]->name);
       label_num+=3;
       int label1 = label_num-2;
       int label2 = label_num-1;
       int label3 = label_num;
       tranCond(Node->children[2],label1,label2);
       codes.push_back(labelOp(label1));
       tranStmt(Node->children[4],type);
       codes.push_back(gotoOp(label3));
       codes.push_back(labelOp(label2));
       tranStmt(Node->children[6],type);
       codes.push_back(labelOp(label3));
   }else{
       label_num+=2;
       int label1 = label_num-1;
       int label2 = label_num;
       tranCond(Node->children[2],label1,label2);
       codes.push_back(labelOp(label1));
       tranStmt(Node->children[4],type);
       codes.push_back(labelOp(label2));
   }
   return;
}

void tranExtDecList(node* Node,char* type){
    /*
    ExtDecList: VarDec {
        $$ = new_node((char*)"ExtDecList", 1, $1);
    }
    | VarDec COMMA ExtDecList {
        $$ = new_node((char*)"ExtDecList", 3, $1, $2, $3);
    }
    */
    // fprintf(FILE_OUT,"tranExtDecList ");
    if(Node->num >1){
        tranVarDec(Node->children[0],type,false);
        tranExtDecList(Node->children[2],type);
    }else{
        tranVarDec(Node->children[0],type,false);
    }
    return;
}

Variable* tranVarDec(node* Node,char* type,bool is_struct=false){
    /*
    VarDec: ID {
        $$ = new_node((char*)"VarDec", 1, $1);
    }
    | VarDec LB INT RB {
        $$ = new_node((char*)"VarDec", 4, $1, $2, $3, $4);
    }
    */
//    printf("tranVarDec :%d ",Node->line);
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

        Array* array = new Array(type,current->line,0,CLASS_ARRAY,size);

        return array;
    }else{
        //定义变量
        // char* name = current->children[0]->value;
        char* name = current->children[0]->value;
        var_table[name] = var_table.size() + 1;
        Variable* var = new Variable(type,current->line,0,CLASS_PRIMITIVE);
        return var;
    }
   }else{
       //定义结构体
        // char* name;
        // Struct* var;
        // if(Node->num>1){
        //     vector<int> size;
        //     while(current->num>1){
        //         size.push_back(atoi(current->children[2]->value));
        //         current=current->children[0];
        //     }
        //     name = current->children[0]->value;

        //     //var = new Struct(type,current->line,0,CLASS_ARRAY,members);
        // }else{
        //     name = current->children[0]->value;

        //     //var = new Struct(type,current->line,0,CLASS_STRUCT,members);
        // }


        // return  (char*)"float";
   }

}

Variable* tranFunDec(node* Node,char* type){
    /*
    FunDec: ID LP VarList RP {
        $$ = new_node((char*)"FunDec", 4, $1, $2, $3, $4);
    }
    | ID LP RP {
        $$ = new_node((char*)"FunDec", 3, $1, $2, $3);
    }
    */
   //ID 的 value应该就是名字
    string name = Node->children[0]->value;

    //在这里初始化func 然后再在varlist里面更新东西
    vector<Variable*> args{};
    int size = var_table.size();
    if(Node->num>3){
        args = tranVarList(Node->children[2]);
    }
    int delta = var_table.size()-size;
    Function* func = new Function(type,Node->line,0,CLASS_FUNCTION,args);
    codes.push_back("FUNCTION " + name + " :");
    if(Node->num>3){
        for(int i =delta;i>0;i--){
            codes.push_back("PARAM v"+to_string(size+i));
        }
    }
    // var_table[name] = -1;
    return func;

}
char* tranSpecifier(node *Node){
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
        return tranStructSpecifier(Node->children[0]);
    }

}

char* tranStructSpecifier(node* Node){
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

   }else{
       //这里不是插入结构体,而是确认结构体的存在,但加入变量符号表还是在Vardec


   }
   //返回结构体的名字
   return name;
}

vector<Variable*> tranDefList(node* Node,bool is_struct){
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
       vector<Variable*> declist = tranDef(Node->children[0],is_struct);
       deflist.insert(deflist.end(),declist.begin(),declist.end());
       Node = Node->children[1];
   }
   return deflist;
}
vector<Variable*> tranDef(node* Node,bool is_struct){
    /*
    Def: Specifier DecList SEMI {
        $$ = new_node((char*)"Def", 3, $1, $2, $3);
    }
    */
   char* type = tranSpecifier(Node->children[0]);
   vector<Variable*> declist =  tranDecList(Node->children[1],type,is_struct);
   return declist;
}

vector<Variable*> tranVarList(node* Node){
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
       variable_list.push_back(tranParamDec(current->children[0]));
       current = current->children[2];
   }
   variable_list.push_back(tranParamDec(current->children[0]));
   return variable_list;
}

Variable* tranParamDec(node* func){
    /*
    ParamDec: Specifier VarDec {
        $$ = new_node((char*)"ParamDec", 2, $1, $2);
    }
    */
   //ParamDec 可能是var,array,struct..是个问题
   //把func的节点传进来,根据Specifier判断是不是struct
   //数组与普通变量都是Variable
    char* type = tranSpecifier(func->children[0]);
    // printf("tranParamDec ");
    return tranVarDec(func->children[1],type,false);
}

vector<Variable*> tranDecList(node* Node,char* type,bool is_struct){
    /*
    DecList: Dec {
        $$ = new_node((char*)"DecList", 1, $1);
    }
    | Dec COMMA DecList {
        $$ = new_node((char*)"DecList", 3, $1, $2, $3);
    }
    */
    // printf("tranDecList ");
    node* current = Node;
    vector<Variable*> variable_list;
    while(current->num>1){
        // printf("nop ");
        variable_list.push_back(tranDec(current->children[0],type,is_struct));
        current = current->children[2];
    }
    // printf("nop ");
    variable_list.push_back(tranDec(current->children[0],type,is_struct));
    return variable_list;
}

Variable* tranDec(node* Node,char* type,bool is_struct){
    /*
    Dec: VarDec {
        $$ = new_node((char*)"Dec", 1, $1);
    }
    | VarDec ASSIGN Exp {
        $$ = new_node((char*)"Dec", 3, $1, $2, $3);
    }
    */
    // char* type = tranSpecifier(Node->children[0]);
    // return tranVarDec(func->children[1],type);
    // printf("tranDec ");
    Variable* var = tranVarDec(Node->children[0],type,is_struct);
    if(Node->num>1){
        tmp_num+=1;
        int t1 = tmp_num;
        tranExp(Node->children[2],t1);
        string name = Node->children[0]->children[0]->value;
        int variable = var_table[name];
        codes.push_back(assignOp(convertvar(variable),converttmp(t1)));
    }
    
    
    return var;
}

string tranExp(node* Node,int place){
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
//    printf("tranExp ");

    char* first_token = Node->children[0]->name;
    // show();
    // printf("\n");
    // printf("%s : %d \n",first_token,Node->line);
    // printf("%s : %d ",first_token,Node->line);
    // printf("%s: %d ",first_token,Node->children[0]->line);
    if(!strcmp("LP",first_token)||!strcmp("MINUS",first_token)||!strcmp("NOT",first_token)){
        // printf("%s ",Node->children[1]->name);
        if(!strcmp("MINUS",first_token)){
            tmp_num += 1;
            int t1 = tmp_num;
            string code1 = tranExp(Node->children[1],t1);
            string code2 = assignOp(converttmp(place),"#0 - "+converttmp(t1));
            codes.push_back(code2);
            return code1 + code2;
        }
        if(!strcmp("NOT",first_token)){
            label_num += 2;
            int label1 = label_num-1;
            int label2 = label_num;
            string code0 = assignOp(converttmp(place),"#0");
            codes.push_back(code0);
            tranCond(Node,label1,label2);
            codes.push_back(labelOp(label1));
            codes.push_back(assignOp(converttmp(place),"#1"));
            return "RIGHT";
        }
        return tranExp(Node->children[1],place);
    }
    if(Node->num == 1){
        if(!strcmp("ID",first_token)){
            //先查下有没有被定义

            char* name = Node->children[0]->value;
            string assign = assignOp(converttmp(place),convertvar(var_table[name]));
            codes.push_back(assign);
            return assign;
        }else{
            //处理下数字
            string value= Node->children[0]->value;
            string assign = assignOp(converttmp(place),"#" + value);
            codes.push_back(assign);
            return assign;
        }
    }
    if(!strcmp("ID",first_token)){
        char* name = Node->children[0]->value;

        if(Node->num>3){
            //说明有参数
            // fprintf(FILE_OUT,"%s ",name);
            vector<int> args;
            tranArgs(Node->children[2],name,args);

            if(!strcmp(name,"write")){
                // printf("%s ",Node->children[2]->name);
                // printf("WRITE %s ",tranExp(Node->children[2]->children[0]).c_str());
                codes.push_back("WRITE "+converttmp(args[0]));

            }else{
                for(int i =0;i<args.size();i++){
                    codes.push_back("ARG "+converttmp(args[i]));
                }
                codes.push_back(converttmp(place)+" := CALL "+Node->children[0]->value);
            }
        }else{
            //说明没有参数
            //这里还要处理一下没有参数的情况,没有的话可以自己写个测试
            // fprintf(FILE_OUT,"%s ",name);

            if(!strcmp(name,"read")){
                // read_num++;
                // printf("READ t%d ",read_num + (int)tmp_table.size());
                codes.push_back("READ "+converttmp(place));
            }else{
                codes.push_back(converttmp(place)+" := CALL "+Node->children[0]->value);
            }

        }
        return (char*)"float";
    }
    if(Node->num == 3){
        if(strcmp(Node->children[1]->name,"DOT")){
            if(!strcmp(Node->children[1]->name,"ASSIGN")){
                tmp_num += 1;
                int t1 = tmp_num;
                int var= (int)var_table[Node->children[0]->children[0]->value];
                tranExp(Node->children[2],t1);
                codes.push_back(assignOp(convertvar(var),converttmp(t1)));
                //codes.push_back(assignOp(converttmp(place),convertvar(var)));
                return "ASSIGN";

            }else if(!strcmp(Node->children[1]->name,"LT") ||
                    !strcmp(Node->children[1]->name,"GT") ||
                    !strcmp(Node->children[1]->name,"LE") ||
                    !strcmp(Node->children[1]->name,"GE") ||
                    !strcmp(Node->children[1]->name,"NE") ||
                    !strcmp(Node->children[1]->name,"EQ") ||
                    !strcmp(Node->children[1]->name,"AND") ||
                    !strcmp(Node->children[1]->name,"OR") ){

                        // tranCond();
                        // printf("%s ",Node->children[0]->name);
                        // tranCond(Node,)
                    label_num += 2;
                    int label1 = label_num-1;
                    int label2 = label_num;
                    string code0 = assignOp(converttmp(place),"#0");
                    codes.push_back(code0);
                    tranCond(Node,label1,label2);
                    codes.push_back(labelOp(label1));
                    codes.push_back(assignOp(converttmp(place),"#1"));
                    return "RIGHT";
                        // printf("IF ");
                // char* name = tranExp(Node->children[0],false,true);
                // char* name = tranExp(Node->children[2],false,true);

            }else{

                tmp_num += 2;
                int t1 = tmp_num-1;
                int t2 = tmp_num;
                string code1 = tranExp(Node->children[0],t1);
                string code2 = tranExp(Node->children[2],t2);

                string code3 = assignOp(converttmp(place),converttmp(t1) +" " +Node->children[1]->value + " " +converttmp(t2));
                codes.push_back(assignOp(converttmp(place),converttmp(t1) +" " +Node->children[1]->value + " " +converttmp(t2)));
                return code1+code2+code3;
            }

        }else{
            //处理dot

        }
    }
    if(Node->num == 4){

    }
    return (char*)"False";
}

void tranCond(node* Node,int label_true,int label_false){
    if(!strcmp(Node->children[0]->name,"NOT")){
        tranCond(Node->children[1],label_false,label_true);
    }else if(!strcmp(Node->children[1]->name,"LT") ||
                    !strcmp(Node->children[1]->name,"GT") ||
                    !strcmp(Node->children[1]->name,"LE") ||
                    !strcmp(Node->children[1]->name,"GE") ||
                    !strcmp(Node->children[1]->name,"NE") ||
                    !strcmp(Node->children[1]->name,"EQ")
                    )
    {
        tmp_num += 2;
        int t1 = tmp_num -1;
        int t2 = tmp_num;
        tranExp(Node->children[0],t1);
        tranExp(Node->children[2],t2);
        string code3 = "IF "+converttmp(t1) + " " + Node->children[1]->value +" " +converttmp(t2)+" "+gotoOp(label_true);
        codes.push_back(code3);
        codes.push_back(gotoOp(label_false));
    }else if (!strcmp(Node->children[1]->name,"AND")){
        label_num +=1;
        int label1 = label_num;
        tranCond(Node->children[0],label1,label_false);
        codes.push_back(labelOp(label1));
        tranCond(Node->children[2],label_true,label_false);
    }else if (!strcmp(Node->children[1]->name,"OR")){
        label_num +=1;
        int label1 = label_num;
        tranCond(Node->children[0],label_true,label1);
        codes.push_back(labelOp(label1));
        tranCond(Node->children[2],label_true,label_false);
    }else{
        printf("hah ");
    }

}

void tranArgs(node* Node,char* func_name,vector<int> &args){
    /*
    Args: Exp COMMA Args {$$ = new_node((char*)"Args", 3, $1, $2, $3);}
    | Exp {$$ = new_node((char*)"Args", 1, $1);}
    */

    if(Node->num > 1){

        tmp_num += 1;
        int t1 = tmp_num;
        tranExp(Node->children[0],t1);
        args.push_back(t1);
        tranArgs(Node->children[2],func_name,args);
    }else{
        tmp_num += 1;
        int t1 = tmp_num;
        tranExp(Node->children[0],t1);
        args.push_back(t1);
    }
}



