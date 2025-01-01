#include "config.h"



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
    bzero(buffer, 1024);
    recv(client_socket, buffer, 4096, 0);

    printf("[SERVER]: %s", buffer);

    


    return 0;



}
