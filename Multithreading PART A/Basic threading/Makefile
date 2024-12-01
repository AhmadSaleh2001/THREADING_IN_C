# Name of the executable
EXEC = thread_example

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -pthread

# Source files
SRC = main.c

# Default rule to compile the program
all: $(EXEC)

# Rule to compile the program
$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

# Rule to clean up the compiled files
clean:
	rm -f $(EXEC)

.PHONY: all clean
