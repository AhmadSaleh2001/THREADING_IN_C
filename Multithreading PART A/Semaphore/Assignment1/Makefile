CC = gcc
CFLAGS = -pthread
TARGET = main.exe

all: $(TARGET)

$(TARGET): main.c
	$(CC) main.c -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)
