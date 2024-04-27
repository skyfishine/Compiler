SRCS = main.cc lexer.cc parser.cc symbol.cc
TARGET = a.out

all: $(TARGET)

$(TARGET): $(SRCS)
	g++ -Wall -Werror $(SRCS)

run: $(TARGET)
	./$(TARGET)

gdb: $(SRCS)
	g++ -g $(SRCS)
	gdb $(TARGET)

clean:
	rm -f test.dyd test.err test.pro test.var a.out