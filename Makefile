CC=gcc
CFLAGS=-Wall -pthread
TARGET=udp_to_tcp

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c

clean:
	rm -f $(TARGET)
