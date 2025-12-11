CC=gcc
TARGETS=main server sync stack queue

.PHONY: all clean

all: $(TARGETS)

main: main.c
	$(CC) -o $@ $^

server: server.c
	$(CC) -o $@ $^

sync: sync.c
	$(CC) -pthread -o $@ $^

stack: stack.c
	$(CC) -o $@ $^

queue: queue.c
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGETS)

