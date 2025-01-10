#include "config.h"
#include <string.h>


typedef struct Client {
    int index;
    int sock_id;
    struct sockaddr_in client_addr;
    int len;
} Client;

int client_count = 0;

Client client[1024];
pthread_t threads[1024];

void *clientComm(void* client_details){
    Client* cl_details = (Client*) client_details;
    int index = cl_details->index;
    int client_sock = cl_details->sock_id;
    printf("[Client %d Connected]", index);
    while(1){
        char data[1024];
        bzero(data, 1024);
        ssize_t message_size = recv(client_sock, data, 1024, 0);
        if(message_size > 0){
           for(int i = 0 ;i < client_count; i++){
                if (i != index)
                    send(client[i].sock_id, data, 1024, 0);
           } 
        }
    }
    
}

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

   

    while(1){
        client[client_count].sock_id = accept(server_socket, (struct sockaddr*) &client[client_count].client_addr, &client[client_count].len);
        pthread_create(&threads[client_count], NULL, clientComm, (void *)&client[client_count]);
        client_count++;
    }    

    for (int i = 0; i < client_count; i++){
        pthread_join(threads[i], NULL);
    }


    close(server_socket);
    return 0;

}
