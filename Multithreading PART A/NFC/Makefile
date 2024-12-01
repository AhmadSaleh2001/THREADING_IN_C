# Makefile for compiling gl_thread.c, main.c, notf_chain.c, and notf_element.c into main.exe

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11

# Output executable name
TARGET = main.exe

# Source files
SRC = rt_entry.c rt_table.c notf_element.c main.c notf_chain.c


# Default target: Compile all .c files into main.exe
all: $(TARGET)

# Rule to compile source files into main.exe
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean up build files (only removes the executable)
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
