# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -g

# Executable program, the final output
TARGET = microc

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj

# Files generated while compiling/running a Micro program
GENERATED_ASM = program_mostro.s
GENERATED_BIN = program_mostro_binary

# Sources and objects
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Default rule
all: $(TARGET)

# Final link: TARGET depends on every object file
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compile each .c file into object code
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean generated build files
clean:
	rm -rf $(OBJDIR) $(TARGET) $(GENERATED_ASM) $(GENERATED_BIN)

.PHONY: all clean