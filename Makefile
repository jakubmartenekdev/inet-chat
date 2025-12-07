CC=gcc
TARGETS=main server sync

.PHONY: all clean

all: $(TARGETS)

main: main.c
	$(CC) -o $@ $^

server: server.c
	$(CC) -o $@ $^

sync: sync.c
	$(CC) -pthread -o $@ $^

clean:
	rm -rf $(TARGETS)

