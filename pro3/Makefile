
FLEX = flex
CC=g++
BISON=bison

.lex: lex.l
	$(FLEX) lex.l
.yacc: syntax.y
	$(BISON) -d syntax.y

splc: .lex .yacc clean 
	@mkdir bin
	touch bin/splc
	@chmod +x bin/splc
	${CC} syntax.tab.c  translate.cpp node.cpp -lfl  -o bin/splc 
clean:
	@rm -rf bin/
.PHONY: splc