#include "config.h"
#include <string.h>


int main(void){
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket == -1)
        perror("Socket Error");
    else
        printf("Socket Established\n");
    

    struct sockaddr_in client_addr;
    client_addr.sin_port = htons(port);
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(ip);

    int connection_status = connect(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if (connection_status == -1)
        perror("Connection Error");
    else
        printf("Connection Established\n");


    char buffer[1024];
    memset(buffer, '\0', sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    printf("[SERVER]: %s\n", buffer);

 
    // bzero(buffer, 1024);
    
    char* line = NULL;
    size_t lineSize = 0;
    printf("Enter the message...(Type exit to exit)");
    while(1){
        ssize_t readLineSize = getline(&line, &lineSize, stdin);
        if (readLineSize > 0){
            if (strcmp(line, "exit\n") == 0)
                break;
            else
                send(client_socket, line, readLineSize, 0);
        }
    }



    return 0;



}
