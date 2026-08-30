# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -g

# Executable program, the final output
TARGET = microc

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj

# Sources and objects
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# Default rule
all: $(TARGET)

# Final Link TARGET depends of each OBJS file
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compile each ".c" to object code
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object dir
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean