CC=gcc
TARGETS=client server sync stack queue

.PHONY: all clean

all: $(TARGETS)

client: client.c
	$(CC) -o $@ $^

server: server.c
	$(CC) -o $@ $^

sync: sync.c
	$(CC) -pthread -o $@ $^

stack: stack.c
	$(CC) -o $@ $^

queue: queue.c
	$(CC) -o $@ $^

test: test.c
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGETS)

