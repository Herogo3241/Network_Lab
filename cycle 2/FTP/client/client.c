#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 21005
#define BUFFER_SIZE 1024
#define COMMAND_SIZE 32
#define FILENAME_SIZE 256

void upload_file(int socket, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(socket, buffer, bytes_read, 0);
    }

    fclose(file);
    printf("File sent successfully\n");
}

void download_file(int socket, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Error creating file: %s\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    printf("File received successfully\n");
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <command> <filename> <server_ip>\n", argv[0]);
        printf("Commands: upload, download\n");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, argv[3], &server_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Send command length and command
    uint32_t command_len = strlen(argv[1]) + 1;  // +1 for null terminator
    send(sock, &command_len, sizeof(command_len), 0);
    send(sock, argv[1], command_len, 0);

    // Send filename length and filename
    uint32_t filename_len = strlen(argv[2]) + 1;  // +1 for null terminator
    send(sock, &filename_len, sizeof(filename_len), 0);
    send(sock, argv[2], filename_len, 0);

    if (strcmp(argv[1], "upload") == 0) {
        upload_file(sock, argv[2]);
    } else if (strcmp(argv[1], "download") == 0) {
        download_file(sock, argv[2]);
    } else {
        printf("Invalid command\n");
    }

    close(sock);
    return 0;
}
