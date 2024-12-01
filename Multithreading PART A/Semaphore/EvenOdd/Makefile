CC = gcc
CFLAGS = -pthread
TARGET = main.exe
SOURCES = main.c sema.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)
