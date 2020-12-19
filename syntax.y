%{  
    #include "node.h"
    #include "check.h"
    #ifndef LEX_YY_C
    #define LEX_YY_C
    #include "lex.yy.c"
    #endif
    extern FILE *yyin;
    FILE* FILE_OUT;
    void yyerror(const char*);
    int has_error = 0;
    int has_lexical_error = 0;
    
%}
%union {
    struct node *node;
}
%nonassoc LOWER_ELSE
%nonassoc <node> TYPE STRUCT IF ELSE WHILE RETURN
%token <node> INT FLOAT CHAR ID
%token <node> LC RC
%right <node> ASSIGN
%left <node> AND 
%left <node> OR 
%left <node> LT LE GT GE NE EQ
%left <node> PLUS MINUS
%left <node> MUL DIV
%right <node> NOT
%left <node> LP RP LB RB DOT
%token <node> SEMI COMMA

%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args
%%
Program: ExtDefList {
        $$ = new_node((char*)"Program", 1, $1);
        if (!has_error)
            // node_traverse($$,0);
            checkProgram($$);
            // show_table();
    }
    ;
ExtDefList: ExtDef ExtDefList {
        $$ = new_node((char*)"ExtDefList", 2, $1, $2);
    }
    | %empty {
        $$ = new_node(NULL, 0);
    }
    ;
ExtDef: Specifier ExtDecList SEMI {
        $$ = new_node((char*)"ExtDef", 3, $1, $2, $3);
    }
    | Specifier SEMI {
        $$ = new_node((char*)"ExtDef", 2, $1, $2);
    }
    | Specifier FunDec CompSt {
        $$ = new_node((char*)"ExtDef", 3, $1, $2, $3);
    }
    ;
ExtDecList: VarDec {
        $$ = new_node((char*)"ExtDecList", 1, $1);
    }
    | VarDec COMMA ExtDecList {
        $$ = new_node((char*)"ExtDecList", 3, $1, $2, $3);
    }
    ;

Specifier: TYPE {
         $$ = new_node((char*)"Specifier", 1, $1);
    } 
    | StructSpecifier {
         $$ = new_node((char*)"Specifier", 1, $1);
    }
    ;

StructSpecifier: STRUCT ID LC DefList RC {
        $$ = new_node((char*)"StructSpecifier", 5, $1, $2, $3, $4, $5);
    }
    |
    STRUCT ID {
        $$ = new_node((char*)"StructSpecifier", 2, $1, $2);
    }
    ;

VarDec: ID {
        $$ = new_node((char*)"VarDec", 1, $1);
    }
    | VarDec LB INT RB {
        $$ = new_node((char*)"VarDec", 4, $1, $2, $3, $4);
    }
    ;

FunDec: ID LP VarList RP {
        $$ = new_node((char*)"FunDec", 4, $1, $2, $3, $4);
    }
    | ID LP RP {
        $$ = new_node((char*)"FunDec", 3, $1, $2, $3);
    }
    ;
VarList: ParamDec COMMA VarList {
       $$ = new_node((char*)"VarList", 3, $1, $2, $3);
    }
    | ParamDec {
       $$ = new_node((char*)"VarList", 1, $1);
    }
    ;
ParamDec: Specifier VarDec {
        $$ = new_node((char*)"ParamDec", 2, $1, $2);
    }
    ;

CompSt: LC DefList StmtList RC {
        $$ = new_node((char*)"CompSt", 4, $1, $2, $3, $4);
    }
    ;
StmtList: Stmt StmtList {
        $$ = new_node((char*)"StmtList", 2, $1, $2);
    } 
    | %empty {
        $$ = new_node(NULL, 0);
    }
    ;
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
    ;

DefList: Def DefList {
       $$ = new_node((char*)"DefList", 2, $1, $2);
    }
    | %empty {
       $$ = new_node(NULL, 0);
    }
    ;

Def: Specifier DecList SEMI {
        $$ = new_node((char*)"Def", 3, $1, $2, $3);
    }
    ;

DecList: Dec {
        $$ = new_node((char*)"DecList", 1, $1);
    }
    | Dec COMMA DecList {
        $$ = new_node((char*)"DecList", 3, $1, $2, $3);
    }
    ;

Dec: VarDec {
        $$ = new_node((char*)"Dec", 1, $1);
    }
    | VarDec ASSIGN Exp {
        $$ = new_node((char*)"Dec", 3, $1, $2, $3);
    }
    ;

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
    | MINUS Exp {$$ = new_node((char*)"Exp", 2, $1, $2);}
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
    ;

Args: Exp COMMA Args {$$ = new_node((char*)"Args", 3, $1, $2, $3);}
    | Exp {$$ = new_node((char*)"Args", 1, $1);}
    ;


%%


void yyerror(const char *s){
    has_error = 1;
    
    if (!has_lexical_error) {
        fprintf(stderr, "Error type B at line %d: %s\n", yylineno, s);
    }
}

int main(int argc, char **argv){
    yyin = fopen(argv[1], "r");
    int file_len = strlen(argv[1]);
    char* output_file_name = (char*)malloc(sizeof(char) * (file_len + 3));
    strcpy(output_file_name, argv[1]);
    output_file_name[file_len-3] = 'o';
    output_file_name[file_len-2] = 'u';
    output_file_name[file_len-1] = 't';
    output_file_name[file_len] = '\0';
    FILE_OUT = fopen(output_file_name, "w");
    yyparse();
    fclose(yyin);
}