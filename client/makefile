CC := emcc
SRC_DIR := src
INC_DIR := inc
OBJ_DIR := obj
BIN_DIR := bin
OUT := $(BIN_DIR)/out.html
SHARED_DIR := ../shared

RAYLIB_DIR := /Users/anton/Development/raylib/src
RAYLIB_WEB_DIR := $(RAYLIB_DIR)/web
RAYLIB_WEB_LIB := $(RAYLIB_WEB_DIR)/libraylib.a

EM_INC_DIR := /Users/anton/emsdk/upstream/emscripten/cache/sysroot/include

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
CFLAGS := -I$(INC_DIR) -I$(SHARED_DIR) -I$(RAYLIB_DIR) -I$(EM_INC_DIR) -Wall -Wextra
LDFLAGS := $(RAYLIB_WEB_LIB) --shell-file $(RAYLIB_DIR)/minshell.html -s USE_GLFW=3 -s ASYNCIFY -lwebsocket.js --preload-file resources -s USE_SDL=1

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

