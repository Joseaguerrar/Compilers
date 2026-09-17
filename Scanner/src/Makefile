CC = gcc
FLEX = flex

CFLAGS = -Wall -Wextra -g -I../include

BUILD_DIR = ../build
BIN_DIR = ../bin
PRESENTATION_DIR = ../presentation
TEST_DIR = ../tests


# ============================================================
# Main target
# ============================================================

all: directories preprocessor test_scanner test_beamer


# ============================================================
# Directories
# ============================================================

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)


# ============================================================
# Preprocessor
# ============================================================

$(BUILD_DIR)/preprocessor.lex.c: preprocessor.l | directories
	$(FLEX) -o $@ $<

$(BUILD_DIR)/preprocessor.o: $(BUILD_DIR)/preprocessor.lex.c
	$(CC) $(CFLAGS) -c $< -o $@

preprocessor: $(BUILD_DIR)/preprocessor.o
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/preprocessor


# ============================================================
# Scanner
# ============================================================

$(BUILD_DIR)/scanner.lex.c: scanner.l | directories
	$(FLEX) -o $@ $<

$(BUILD_DIR)/scanner.lex.o: $(BUILD_DIR)/scanner.lex.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/scanner.o: scanner.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_scanner.o: test_scanner.c
	$(CC) $(CFLAGS) -c $< -o $@

test_scanner: \
	$(BUILD_DIR)/scanner.lex.o \
	$(BUILD_DIR)/scanner.o \
	$(BUILD_DIR)/test_scanner.o
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/test_scanner


# ============================================================
# Beamer generator
# ============================================================

$(BUILD_DIR)/beamer_generator.o: beamer_generator.c ../include/beamer_generator.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_beamer.o: $(TEST_DIR)/test_beamer.c
	$(CC) $(CFLAGS) -c $< -o $@

test_beamer: \
	$(BUILD_DIR)/beamer_generator.o \
	$(BUILD_DIR)/test_beamer.o
	$(CC) $(CFLAGS) $^ -o $(BIN_DIR)/test_beamer


# ============================================================
# Generate and compile Beamer presentation
# ============================================================

presentation: test_beamer
	$(BIN_DIR)/test_beamer
	cd $(PRESENTATION_DIR) && pdflatex -interaction=nonstopmode presentation.tex
	cd $(PRESENTATION_DIR) && pdflatex -interaction=nonstopmode presentation.tex


# ============================================================
# Run individual components
# ============================================================

run_preprocessor: preprocessor
	@if [ -z "$(INPUT)" ]; then \
		echo "Uso: make run_preprocessor INPUT=archivo.c"; \
		exit 1; \
	fi
	$(BIN_DIR)/preprocessor $(INPUT)


run_scanner: test_scanner
	$(BIN_DIR)/test_scanner


run_beamer: presentation
	@echo "Presentación generada en:"
	@echo "$(PRESENTATION_DIR)/presentation.pdf"


# ============================================================
# Clean
# ============================================================

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)

	rm -f ../test_beamer
	rm -f ./preprocessor
	rm -f ./test_scanner
	rm -f ./*.lex.c
	rm -f ./*.o
	rm -f ./lex.yy.c

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


.PHONY: \
	all \
	directories \
	preprocessor \
	test_scanner \
	test_beamer \
	presentation \
	run_preprocessor \
	run_scanner \
	run_beamer \
	clean \
	rebuild