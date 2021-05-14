DEPS = mwc/forge.h error.h lexer.h list.h parser.h token.h tree_printer.h warning.h
OBJ = error.o lexer.o list.o parser.o token.o tree_printer.o warning.o main.o

%.o: %.c $(DEPS)
	gcc -c -o $@ $<

sft: $(OBJ)
	gcc -o $@ $^

clean:
	rm *.o
