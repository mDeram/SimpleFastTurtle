DEPS = mwc/forge.h error.h lexer.h list.h parser.h token.h tree_printer.h warning.h
OBJ = error.o lexer.o list.o parser.o token.o tree_printer.o warning.o main.o

default: sft

%.o: %.c $(DEPS)
	gcc -c -o $@ $<

sft: $(OBJ)
	gcc -o $@ $^

debug: $(OBJ)
	gcc -Q -v -da -o $@ $^
	gdb debug

clean:
	rm *.o
