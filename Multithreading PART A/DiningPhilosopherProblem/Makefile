# Define compiler and output file
CC = gcc
TARGET = assignment_din_ph.exe

# Define source files
SRC = assignment_din_ph.c

# Default target to build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC)

# Clean up by removing the executable
clean:
	rm -f $(TARGET)
