CC = gcc
CFLAGS = -Wall -Wextra -g
FLEX = flex

all: preprocessor test_scanner

preprocessor: preprocessor.l
	$(FLEX) -o preprocessor.lex.c preprocessor.l
	$(CC) $(CFLAGS) preprocessor.lex.c -o preprocessor

test_scanner: scanner.lex.c scanner.c test_scanner.c
	$(CC) $(CFLAGS) scanner.lex.c scanner.c test_scanner.c -o test_scanner

scanner.lex.c: scanner.l
	$(FLEX) -o scanner.lex.c scanner.l

clean:
	rm -f preprocessor test_scanner *.lex.c *.o

.PHONY: all clean