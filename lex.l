%{  
    #include <string.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include "syntax.tab.h"
    #include "node.h"
    #include "check.h"
    int yycolumn = 1;
    extern int has_error;
    extern int has_lexical_error;

  
%}

%option yylineno

HEX_INT [-+]?0[xX]([0-9a-fA-F]{0,8})
OCT_INT [-+]?0([0-3]?[0-7]{0,10})
DEC_INT [-+]?0|([1-9][0-9]*)
HEX_INT_WRONG [-+]?0[xX][0-9a-zA-Z]*
OCT_INT_WRONG [-+]?0[a-wy-z][0-9a-zA-Z]*
INT {HEX_INT}|{OCT_INT}|{DEC_INT}
INT_WRONG {HEX_INT_WRONG}|{OCT_INT_WRONG}
HEX_FLOAT {HEX_INT}\.[0-9a-f]*
OCT_FLOAT {OCT_INT}\.[0-7]*
DEC_FLOAT [-+]?(0|[1-9][0-9]*)\.[0-9]*
FLOAT {HEX_FLOAT}|{OCT_FLOAT}|{DEC_FLOAT}
CHAR ('[a-zA-Z0-9]')|('\\x[0-9a-fA-F]{2}')
CHAR_WRONG ('\\x[0-9a-zA-Z]*')
ID ([a-zA-Z_][0-9a-zA-Z_]*)
ID_WRONG [0-9][0-9a-zA-Z_]*

DOT \.
SEMI ;
COMMA ,
ASSIGN =
LT <
LE <=
GT >
GE >=
NE !=
EQ ==
PLUS \+
MINUS -
MUL \*
DIV \/
AND &&
OR \|\|
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}

%%

{FLOAT} { 
    yylval.node = newLeaf((char*)"FLOAT", strdup(yytext), yylineno);
    return FLOAT;
}
{INT} {
    yylval.node = newLeaf((char*)"INT", strdup(yytext), yylineno);
    return INT;
}
 
{CHAR} {
    
    yylval.node = newLeaf((char*)"CHAR",strdup(yytext), yylineno);
    return CHAR;    
}
 
{ID} {
    if (strcmp(yytext, "int") == 0 ||
            strcmp(yytext, "float") == 0 ||
            strcmp(yytext, "char") == 0) {
        yylval.node = newLeaf((char*)"TYPE", strdup(yytext), yylineno);
        return TYPE;
    }    
    else if (strcmp(yytext, "struct") == 0) {
        yylval.node = newLeaf((char*)"STRUCT", NULL, yylineno);
        return STRUCT;
    }
    else if (strcmp(yytext, "if") == 0) {
        yylval.node = newLeaf((char*)"IF", NULL, yylineno);
        return IF;
    }
    else if (strcmp(yytext, "else") == 0) {
        yylval.node = newLeaf((char*)"ELSE", NULL, yylineno);
        return ELSE;
    }
    else if (strcmp(yytext, "while") == 0) {
        yylval.node = newLeaf((char*)"WHILE", NULL, yylineno);
        return WHILE;
    }
    else if (strcmp(yytext, "return") == 0) {
        yylval.node = newLeaf((char*)"RETURN", NULL, yylineno);
        return RETURN;
    }
    else {
        yylval.node = newLeaf((char*)"ID", strdup(yytext), yylineno);
        return ID;
    }
}
 
{DOT} {
    yylval.node = newLeaf((char*)"DOT", NULL, yylineno);
    return DOT;
}
{SEMI} { 
    yylval.node = newLeaf((char*)"SEMI", NULL, yylineno);
    return SEMI;    
}
{COMMA} { 
    yylval.node = newLeaf((char*)"COMMA", NULL, yylineno);
    return COMMA;
}
{ASSIGN} { 
    yylval.node = newLeaf((char*)"ASSIGN", NULL, yylineno);
    return ASSIGN;
}
{LT} { 
    yylval.node = newLeaf((char*)"LT", (char*)"<", yylineno);
    return LT;
}
{LE} {
    yylval.node = newLeaf((char*)"LE", (char*)"<=", yylineno);
    return LE;
}
{GT} { 
    yylval.node = newLeaf((char*)"GT", (char*)">", yylineno);
    return GT;    
}
{GE} { 
    yylval.node = newLeaf((char*)"GE", (char*)">=", yylineno);
    return GE;
}
{NE} { 
    yylval.node = newLeaf((char*)"NE", (char*)"!=", yylineno);
    return NE;
}
{EQ} { 
    yylval.node = newLeaf((char*)"EQ", (char*)"==", yylineno);
    return EQ;
}
{PLUS} { 
    yylval.node = newLeaf((char*)"PLUS", NULL, yylineno);
    return PLUS;
}
{MINUS} { 
    yylval.node = newLeaf((char*)"MINUS", NULL, yylineno);
    return MINUS;    
}
{MUL} { 
    yylval.node = newLeaf((char*)"MUL", NULL, yylineno);
    return MUL;    
}
{DIV} { 
    yylval.node = newLeaf((char*)"DIV", NULL, yylineno);
    return DIV;
}
{AND} { 
    yylval.node = newLeaf((char*)"AND", NULL, yylineno);
    return AND;
}
{OR} { 
    yylval.node = newLeaf((char*)"OR", NULL, yylineno);
    return OR;
}
{NOT} { 
    yylval.node = newLeaf((char*)"NOT", NULL, yylineno);
    return NOT;
}
{LP} { 
    yylval.node = newLeaf((char*)"LP", NULL, yylineno);
    return LP;
}
{RP} { 
    yylval.node = newLeaf((char*)"RP", NULL, yylineno);
    return RP;
}
{LB} { 
    yylval.node = newLeaf((char*)"LB", NULL, yylineno);
    return LB;
}
{RB} { 
    yylval.node = newLeaf((char*)"RB", NULL, yylineno);
    return RB;
}
{LC} { 
    yylval.node = newLeaf((char*)"LC", NULL, yylineno);
    return LC;
}
{RC} { 
    yylval.node = newLeaf((char*)"RC", NULL, yylineno);
    return RC;
}
{INT_WRONG} {
    printf("Error type A at Line %d: Wrong Int Form \'%s\'\n", 
    yylineno, yytext);
    has_lexical_error = 1;
    has_error = 1;
}
{CHAR_WRONG} {
    printf("Error type A at Line %d: Wrong Char Form \'%s\'\n", 
    yylineno, yytext);
    has_lexical_error = 1;
    has_error = 1;
}
{ID_WRONG} {
    printf("Error type A at Line %d: Wrong Form \"%s\"\n", 
    yylineno, yytext);
    has_lexical_error = 1;
    has_error = 1;
}
[\n] {has_lexical_error = 0;}
[ \r\t] {}
. {
    printf("Error type A at Line %d: unknown lexeme \'%s\'\n", 
    yylineno, yytext);
    has_error = 1;
    has_lexical_error = 1;
}

%%
