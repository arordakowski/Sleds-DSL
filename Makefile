default:
	flex -l lexico.l
	bison -dv sintatico.y
	gcc -o comp sintatico.tab.c	lex.yy.c -lfl

	./comp < entradaMaxMin.txt 
