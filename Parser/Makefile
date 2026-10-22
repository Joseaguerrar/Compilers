.SUFFIXES:

CC = gcc
BISON = bison
FLEX = flex

CFLAGS = -Wall -Wextra -g -Iinclude

BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/parser

OBJS = \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/parser.o \
	$(BUILD_DIR)/preprocessor.o \
	$(BUILD_DIR)/scanner.o \
	$(BUILD_DIR)/scanner_api.o

all: directories $(TARGET)

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

# Flex: Preprocesador
$(BUILD_DIR)/preprocessor.lex.c: src/preprocessor.l | directories
	$(FLEX) -o $@ $<

$(BUILD_DIR)/preprocessor.o: $(BUILD_DIR)/preprocessor.lex.c include/preprocessor.h
	$(CC) $(CFLAGS) -c $< -o $@

# Flex: Scanner
$(BUILD_DIR)/scanner.lex.c: src/scanner.l | directories
	$(FLEX) -o $@ $<

$(BUILD_DIR)/scanner.o: $(BUILD_DIR)/scanner.lex.c include/token.h
	$(CC) $(CFLAGS) -c $< -o $@

# Scanner API
$(BUILD_DIR)/scanner_api.o: src/scanner_api.c include/token.h | directories
	$(CC) $(CFLAGS) -c $< -o $@

# Bison: Parser
$(BUILD_DIR)/parser.bison.c: src/parser.y | directories
	$(BISON) -o $@ $<

$(BUILD_DIR)/parser.o: $(BUILD_DIR)/parser.bison.c include/token.h
	$(CC) $(CFLAGS) -c $< -o $@

# Main
$(BUILD_DIR)/main.o: src/main.c include/preprocessor.h | directories
	$(CC) $(CFLAGS) -c $< -o $@

# Ejecutable final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)

rebuild: clean all

.PHONY: all directories clean rebuild