all: a.out

a.out: main.cc lexer.cc
	g++ main.cc lexer.cc

run: a.out
	./a.out

gdb: main.cc lexer.cc
	g++ -g main.cc lexer.cc
	gdb a.out