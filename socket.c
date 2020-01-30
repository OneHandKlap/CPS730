#include <stdio.h>
#include <string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

int main(int argc, char * argv[]){
    int socket_desc, client_sock, c ,read_size;
    struct sockaddr_in server, client;
    char client_message[2000];

    //create socket

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1){
        printf("Could not create socket");
    }
    puts("Socket created");

    //prepare sockaddr_in structure

    server.sin_family = AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port = htons(8888);

    //bind socket

    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
        perror("bind failed");
        return 1;
    }
    puts ("Bind successful");

    //Listen
    listen(socket_desc,3);
    
    //Accept incoming
    puts("Waiting for incoming connections ... .. . ..");
    c=sizeof(struct sockaddr_in);

    client_sock=accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
    if (client_sock < 0){
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    while((read_size = recv(client_sock, client_message, 2000,0 ))>0){
        //Parrot the message received

        write(client_sock, client_message, strlen(client_message));

    }
    if(read_size==0){
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size==-1){
        perror("receive failed");
    }
    return 0;
}