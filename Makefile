CC=gcc
CFLAGS=-Wall
SRCS=$(wildcard *.c)
EXCLUDE = ll_queue.c
FILTERED_SRCS = $(filter-out $(EXCLUDE), $(SRCS))
TARGET_DIR=bin
# Make programs for filtered C files
TARGETS=$(patsubst %.c,$(TARGET_DIR)/%,$(FILTERED_SRCS)) 

.PHONY: all clean

all: $(TARGETS)

$(TARGET_DIR)/sync: sync.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -pthread -o $@ $<

$(TARGET_DIR)/%: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(TARGET_DIR)

