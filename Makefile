# Compiler and flags
CC      = gcc																									# Compiler to use [C] -> [x86]  
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude -g 								# -std=c11: Enforces the C11 lang

# Executable program, the final output
TARGET  = microc

# Directories to use [src/ , include/, obj/]
SRCDIR  = src
INCDIR  = include
OBJDIR  = obj

# Sources and automatic objects
SRCS    = $(wildcard $(SRCDIR)/*.c) 													# Detects any ".c" file on "src/"
OBJS    = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))		# Uses "pattern substitution" (patsubst) to traslate each ".c" -> ".o"

# Default rule
all: $(TARGET)

# Final Link TARGET depends of each OBJS file
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Compile each ".c" to object code
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Craete object dir
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean

# Use
# 
# make 														(Compile the project)
# ./microc tests/prueba.micro 		(run the compiler with a test file)
# make clean											(Clean build files)

# If the makefile don't exist:
# gcc -Wall -Wextra -std=c11 -Iinclude -g src/*.c -o microc # (All of the ".c" into microc)