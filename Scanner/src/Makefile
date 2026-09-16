all:
	flex preprocessor.l
	gcc lex.yy.c -o preprocessor
	./preprocessor

# FUTURE MAKE FILE
# CC = gcc
# CFLAGS = -Wall -Wextra -g
# FLEX = flex

# TARGET = test_scanner

# LEX_SRC = scanner.l
# LEX_C = scanner.lex.c
# C_SRCS = scanner.c test_scanner.c $(LEX_C)
# OBJS = scanner.o test_scanner.o scanner.lex.o

# all: $(TARGET)

# $(TARGET): $(OBJS)
# 	$(CC) $(OBJS) -o $(TARGET)

# $(LEX_C): $(LEX_SRC)
# 	$(FLEX) -o $@ $<

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# clean:
# 	rm -f $(TARGET) $(OBJS) $(LEX_C) lex.yy.c preprocessor

# .PHONY: all clean