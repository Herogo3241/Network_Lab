#include "config.h"
#include <string.h>

void *serverComm(void *sock_id)
{
    int client_socket = *(int *)sock_id;
    while (1)
    {
        char data[1024];
        bzero(data, 1024);
        int data_size = recv(client_socket, data, 1024, 0);
        data[data_size] = '\0';
        printf("%s", data);
    }
}

int main(void)
{
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket == -1)
        perror("Socket Error");
    else
        printf("Socket Established\n");

    struct sockaddr_in client_addr;
    client_addr.sin_port = htons(port);
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(ip);

    int connection_status = connect(client_socket, (struct sockaddr *)&client_addr, sizeof(client_addr));
    if (connection_status == -1)
        perror("Connection Error");
    else
        printf("Connection Established\n");

    pthread_t thread;
    char name[10];
    printf("Enter username: ");
    if (fgets(name, sizeof(name), stdin) != NULL)
    {
        size_t len = strlen(name);
        if (len > 0 && name[len - 1] == '\n')
        {
            name[len - 1] = '\0'; // Remove trailing newline
        }
        printf("Hello, %s!\n", name);
    }
    else
    {
        printf("Error reading input.\n");
        return 1;
    }

    pthread_create(&thread, NULL, serverComm, (void *)client_socket);
    while (1)
    {
        char *buffer = NULL;
        size_t len = 0;
        ssize_t message_size = getline(&buffer, &len, stdin);

        if (message_size > 0)
        {
            if (strcmp(buffer, "exit\n") == 0)
            {
                free(buffer); // Free memory allocated by getline
                break;
            }
            else
            {
                char message[1024];
                memset(message, 0, sizeof(message)); // Clear the message buffer
                snprintf(message, 1024, "[%s]: %s", name, buffer);
                send(client_socket, message, strlen(message), 0);
            }
        }
        free(buffer); // Free buffer after use
    }

    return 0;
}
