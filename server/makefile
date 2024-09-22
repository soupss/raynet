CC := gcc
SRC_DIR := src
INC_DIR := inc
OBJ_DIR := obj
BIN_DIR := bin
OUT := $(BIN_DIR)/out
SHARED_DIR := ../shared

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
CFLAGS := -I$(INC_DIR) -I$(SHARED_DIR) $(shell pkg-config --cflags libwebsockets) $(shell pkg-config --cflags openssl) -Wall -Wextra
LDFLAGS := $(shell pkg-config --libs libwebsockets)

.PHONY: all clean

all: $(OUT)

$(OUT): $(OBJECTS) | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/*
