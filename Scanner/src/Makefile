.SUFFIXES:

CC = gcc
FLEX = flex

CFLAGS = -Wall -Wextra -g -I../include

BUILD_DIR = ../build
BIN_DIR = ../bin
PRESENTATION_DIR = ../presentation

TARGET = $(BIN_DIR)/analyzer


# ============================================================
# Main target
# ============================================================

all: directories $(TARGET)


# ============================================================
# Directories
# ============================================================

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(PRESENTATION_DIR)


# ============================================================
# Flex: Preprocessor
# ============================================================

$(BUILD_DIR)/preprocessor.lex.c: preprocessor.l | directories
	$(FLEX) -o $@ $<

$(BUILD_DIR)/preprocessor.o: $(BUILD_DIR)/preprocessor.lex.c
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Flex: Scanner
# ============================================================

$(BUILD_DIR)/scanner.lex.c: scanner.l | directories
	$(FLEX) -o $@ $<

$(BUILD_DIR)/scanner.lex.o: $(BUILD_DIR)/scanner.lex.c
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Scanner API
# ============================================================

$(BUILD_DIR)/scanner_api.o: scanner_api.c ../include/token.h
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Beamer
# ============================================================

$(BUILD_DIR)/beamer_generator.o: \
	beamer_generator.c \
	../include/beamer_generator.h \
	../include/token.h
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Main
# ============================================================

$(BUILD_DIR)/main.o: \
	main.c \
	../include/preprocessor.h \
	../include/token.h \
	../include/beamer_generator.h
	$(CC) $(CFLAGS) -c $< -o $@


# ============================================================
# Final executable
# ============================================================

$(TARGET): \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/preprocessor.o \
	$(BUILD_DIR)/scanner.lex.o \
	$(BUILD_DIR)/scanner_api.o \
	$(BUILD_DIR)/beamer_generator.o
	$(CC) $(CFLAGS) $^ -o $@


# ============================================================
# Run complete pipeline
# ============================================================

run: all
	@if [ -z "$(INPUT)" ]; then \
		echo "Uso: make run INPUT=archivo.c"; \
		exit 1; \
	fi
	cd .. && ./bin/analyzer $(INPUT)


# ============================================================
# Clean
# ============================================================

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)

	rm -f $(PRESENTATION_DIR)/presentation.aux
	rm -f $(PRESENTATION_DIR)/presentation.log
	rm -f $(PRESENTATION_DIR)/presentation.nav
	rm -f $(PRESENTATION_DIR)/presentation.out
	rm -f $(PRESENTATION_DIR)/presentation.snm
	rm -f $(PRESENTATION_DIR)/presentation.toc
	rm -f $(PRESENTATION_DIR)/presentation.vrb
	rm -f $(PRESENTATION_DIR)/presentation.pdf
	rm -f $(PRESENTATION_DIR)/presentation.tex


# ============================================================
# Complete rebuild
# ============================================================

rebuild: clean all


.PHONY: all directories run clean rebuild