#include "config.h"
#include <string.h>

int main(void){
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1)
        perror("Socket Error");
    else
        printf("Socket Established\n");


    struct sockaddr_in server_addr;
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    
    int bind_status = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (bind_status == -1)
        perror("Bind Error");


    listen(server_socket, 1);
    printf("Listening to port: %d\n", (int)port);

    struct sockaddr_in client_addr;
    socklen_t addr;
    int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr);
    if(client_socket == -1)
        perror("Error Accepting Client");
    else
        printf("Accepted Client\n");


    char message[1024];
    memset(message, 0, 1024);
    strcpy(message, "CONNECTED");
    send(client_socket, message, strlen(message), 0);

    while(1){
        bzero(message, 1024);
        recv(client_socket, message, 1024, 0);
        printf("%s\n", message);
    }    


    close(server_socket);
    return 0;

}
