# Compiler and flags
CC = clang
CFLAGS = -Wall -Wextra -pedantic -std=c11 -Iinclude -g

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin

# Files
CLI_SRC = $(SRC_DIR)/cli.c
SERVER_SRC = $(SRC_DIR)/server.c
CLI_OBJ = $(CLI_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
TARGET = $(BIN_DIR)/crest-cli

# Default rule (build CLI)
all: $(TARGET)

$(TARGET): $(CLI_OBJ) $(SERVER_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(CLI_OBJ) $(SERVER_OBJ)

# Compile object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

# Run the CLI
run: all
	./$(TARGET)

# Clean compiled files
clean:
	rm -rf $(SRC_DIR)/*.o $(BIN_DIR)

# Debugging mode
debug: CFLAGS += -DDEBUG -g
debug: all
