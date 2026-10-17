.SUFFIXES:

CC = gcc
BISON = bison

CFLAGS = -Wall -Wextra -g -I../Scanner/include

BUILD_DIR = build
BIN_DIR = bin

TARGET = $(BIN_DIR)/parser

SCANNER_DIR = ../Scanner
SCANNER_BUILD = $(SCANNER_DIR)/build
SCANNER_OBJS = \
	$(SCANNER_BUILD)/preprocessor.o \
	$(SCANNER_BUILD)/scanner.lex.o \
	$(SCANNER_BUILD)/scanner_api.o

all: directories $(TARGET)

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)

$(BUILD_DIR)/parser.bison.c: src/parser.y | directories
	$(BISON) -o $@ $<

$(BUILD_DIR)/parser.o: $(BUILD_DIR)/parser.bison.c
	$(CC) $(CFLAGS) -c $(BUILD_DIR)/parser.bison.c -o $@

$(BUILD_DIR)/main.o: src/main.c ../Scanner/include/preprocessor.h | directories
	$(CC) $(CFLAGS) -c $< -o $@

$(SCANNER_OBJS): FORCE
	$(MAKE) -C $(SCANNER_DIR)/src $(patsubst $(SCANNER_BUILD)/%,../build/%,$@)

$(TARGET): $(BUILD_DIR)/main.o $(BUILD_DIR)/parser.o $(SCANNER_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)

rebuild: clean all

.PHONY: all directories clean rebuild FORCE