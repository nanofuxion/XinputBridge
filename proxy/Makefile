CC=gcc
CFLAGS=-Wall -Wextra -pthread
LDFLAGS=-pthread
TARGET=udp_proxy

all: $(TARGET)

$(TARGET): proxy.c
	$(CC) $(CFLAGS) -o $(TARGET) proxy.c $(LDFLAGS)

clean:
	rm -f $(TARGET)