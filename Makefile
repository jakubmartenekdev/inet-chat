#-----------------------------------
# Use conditional variable to change between release/debug
# eg. make DEBUG=0
#-----------------------------------
DEBUG ?= 1

ifeq ($(DEBUG), 1)
	CFLAGS=-Wall -g -DDEBUG
	TARGET_DIR=$(BUILD_DIR)/debug/bin
	OBJ_DIR=$(BUILD_DIR)/debug/obj
else
	CFLAGS=-Wall -s -O3
	TARGET_DIR=$(BUILD_DIR)/release/bin
	OBJ_DIR=$(BUILD_DIR)/release/obj
endif


CC=gcc
SRCS=$(wildcard *.c)
EXCLUDE = ll_queue.c
FILTERED_SRCS = $(filter-out $(EXCLUDE), $(SRCS))
BUILD_DIR=build
# Creates program for each filtered C file
TARGETS=$(patsubst %.c,$(TARGET_DIR)/%,$(FILTERED_SRCS)) 
OBJS=$(patsubst %.c, $(OBJ_DIR)/%.o,$(SRCS))
#-----------------------------------
# Testable sources
#-----------------------------------
MODULES_TO_TEST = $(filter $(OBJ_DIR)/ll_queue.o, $(OBJS))

#-----------------------------------
# Unit test resources
#-----------------------------------
UTEST_DIR=tests/unit
UTEST_SRCS=$(wildcard $(UTEST_DIR)/*.c)
UTEST_SRCS_STRIPPED=$(notdir $(UTEST_SRCS))
UTEST_OBJ_DIR = $(UTEST_DIR)/obj
UTEST_OBJS=$(patsubst %.c, $(UTEST_OBJ_DIR)/%.o, $(UTEST_SRCS_STRIPPED))
UTEST_LIB=-lcriterion
UTEST_BIN_DIR = $(UTEST_DIR)/bin
UTEST_BIN = $(UTEST_BIN_DIR)/tests

.PHONY: all clean tests run install
# Make removes created files that are not targets with names
# %.o etc. are not names but patters
# to signal make not to delete my tmp files use .PRECIOUS
# .PRECIOUS: $(OBJ_DIR)/%.o

#-----------------------------------
# Main program
#-----------------------------------
all: $(TARGETS) $(OBJS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_DIR)/server: $(OBJ_DIR)/server.o
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -pthread -o $@ $<

$(TARGET_DIR)/%: $(OBJ_DIR)/%.o
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

#-----------------------------------
# Testing framework
#-----------------------------------
tests: $(UTEST_BIN) $(UTEST_OBJS)

# Compile to obj
$(UTEST_OBJ_DIR)/%.o: $(UTEST_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Link together
$(UTEST_BIN): $(UTEST_OBJS) $(MODULES_TO_TEST)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(UTEST_LIB) -o $@ $^

run: $(UTEST_BIN)
	./$(UTEST_BIN)

#-----------------------------------
# Local installation
#-----------------------------------
install:
	@echo "Installing binaries to /usr/local/bin"
	install -m 755 $(TARGET_DIR)/server /usr/local/bin/
	install -m 755  $(TARGET_DIR)/client /usr/local/bin/
	@echo "Successfully installed."

uninstall:
	@echo "Uninstalling programs..."
	rm -f /usr/local/bin/server /usr/local/bin/client
	@echo "Successfully uninstalled."

#-----------------------------------
# Remove temps
#-----------------------------------
clean:
	rm -rf $(BUILD_DIR) $(UTEST_BIN_DIR) $(UTEST_OBJ_DIR)
