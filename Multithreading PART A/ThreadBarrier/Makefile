# Define the compiler
CC = gcc

# Define the source files
SOURCES = main.c threadbarrier.c

# Define the output executable name
OUTPUT = main.exe

# Define the compilation flags (optional)
CFLAGS = -Wall -pthread

# Rule to build the executable
$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(SOURCES)

# Rule to clean up generated files
clean:
	rm -f $(OUTPUT)
