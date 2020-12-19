
FLEX = flex
CC=g++
BISON=bison

.lex: lex.l
	$(FLEX) lex.l
.yacc: syntax.y
	$(BISON) -d syntax.y

splc: .lex .yacc 
	${CC} syntax.tab.c   check.cpp node.cpp -lfl  -o bin/splc 
clean:
	@rm -rf bin/
.PHONY: splc