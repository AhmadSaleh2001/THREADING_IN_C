# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Source files
SRC = rw_lock.c bridge.c test_custom.c

# Object files
OBJ = $(SRC:.c=.o)

# Output executable
TARGET = test_custom.exe

# Rule to build the target executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets to avoid conflict with files named clean
.PHONY: clean
