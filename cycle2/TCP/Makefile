# Define the compiler
CC = gcc

# Define the targets
SERVER = server
CLIENT = client

# Define the source files
SERVER_SRC = server.c
CLIENT_SRC = client.c

# Build server
$(SERVER): $(SERVER_SRC)
	$(CC) $(SERVER_SRC) -o $(SERVER)

# Build client
$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CLIENT_SRC) -o $(CLIENT)

# Run server
run-server: $(SERVER)
	./$(SERVER)

# Run client
run-client: $(CLIENT)
	./$(CLIENT)

# Clean the built files
clean:
	rm -f $(SERVER) $(CLIENT)

# Default target to build both server and client
all: $(SERVER) $(CLIENT)

