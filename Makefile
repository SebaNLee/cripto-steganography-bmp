CC ?= cc
NAME := stegobmp

SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN_DIR := bin
BIN := $(BIN_DIR)/$(NAME)

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CFLAGS ?= -O2 -g
CFLAGS += -std=c99 -Wall -Wextra -I$(INC_DIR) -MMD -MP
LDLIBS := -lcrypto

.PHONY: all debug clean

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS  += -O0 -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: clean $(BIN)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

-include $(DEPS)

