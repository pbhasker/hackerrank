CC=gcc
CFLAGS=-Wall -g --std=c99

all:
	$(CC) $(CFLAGS) direct_connections.c -o connections
clean:
	rm *.exe
