#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#define PORT 21005
#define BUFFER_SIZE 1024
#define COMMAND_SIZE 32
#define FILENAME_SIZE 256

volatile sig_atomic_t keep_running = 1;

void handle_signal(int signum) {
    if (signum == SIGINT) {
        printf("\nReceived shutdown signal. Cleaning up...\n");
        keep_running = 0;
    }
}

void handle_upload(int client_socket, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Error creating file: %s\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    printf("File received successfully: %s\n", filename);
}

void handle_download(int client_socket, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
    printf("File sent successfully: %s\n", filename);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Set up signal handler
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR option
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);
    printf("Press Ctrl+C to shutdown the server\n");

    // Set timeout for accept
    struct timeval tv;
    tv.tv_sec = 1;  // 1 second timeout
    tv.tv_usec = 0;
    setsockopt(server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    while (keep_running) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Timeout occurred, check if we should keep running
                continue;
            }
            perror("Accept failed");
            continue;
        }

        printf("Client connected\n");

        char command[COMMAND_SIZE] = {0};
        char filename[FILENAME_SIZE] = {0};
        
        // Receive command length and command
        uint32_t command_len;
        recv(client_socket, &command_len, sizeof(command_len), 0);
        recv(client_socket, command, command_len, 0);

        // Receive filename length and filename
        uint32_t filename_len;
        recv(client_socket, &filename_len, sizeof(filename_len), 0);
        recv(client_socket, filename, filename_len, 0);

        printf("Received command: %s, filename: %s\n", command, filename);

        if (strcmp(command, "upload") == 0) {
            handle_upload(client_socket, filename);
        } else if (strcmp(command, "download") == 0) {
            handle_download(client_socket, filename);
        }

        close(client_socket);
    }

    printf("Closing server socket...\n");
    close(server_socket);
    printf("Server shutdown complete\n");
    return 0;
}
