CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -pthread

SRC_DIR = src
BIN_DIR = bin

SERVER_SRC = $(SRC_DIR)/server.c $(SRC_DIR)/network.c
CLIENT_SRC = $(SRC_DIR)/client.c $(SRC_DIR)/network.c

SERVER_BIN = $(BIN_DIR)/server
CLIENT_BIN = $(BIN_DIR)/client

.PHONY: all clean

all: $(SERVER_BIN) $(CLIENT_BIN)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(SERVER_BIN): $(SERVER_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $@ $(LDFLAGS)

$(CLIENT_BIN): $(CLIENT_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $@ $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR)
