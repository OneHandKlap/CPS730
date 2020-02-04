#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <time.h>


void generateReply(int argCommand){

    if(argCommand==1){
        //STUFF FOR POST
        //retrieve POST function status code
        //retrieve HEADERS
        //FORMAT
        //PRINt/PUT/WRITE
    }
    if(argCommand==2){
        //STUFF FOR GET
        //retrieve GET function status code
        //retrieve HEADERS
        //retrieve file lines
        //FORMAT
        //PRINt/PUT/WRITE (from file)
        
    }
    if(argCommand==3){
        //STUFF FOR HEAD
        //retrieve GET function status code
        //retrieve HEADERS
        //retrieve file lines
        //FORMAT
    }
    else{
        return 1;
    }
}

int post(char file){

}




int main(int argc, char ** argv){
    clock_t begin = clock();
    int socket_desc, client_sock, c ,read_size;
    struct sockaddr_in server, client;
    char client_message[2000];
    char* reply_message;
    int port;

    if (argc>1){
        if (strcmp(argv[1],"-p")==0){
            port=atoi(argv[2]);
        }
    }

    //create socket

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1){
        printf("Could not create socket");
    }
    puts("Socket created");

    //prepare sockaddr_in structure

    server.sin_family = AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    // inet_pton(AF_INET, "192.0.2.33", &(server.sin_addr.s_addr));
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

    if (argc>1){
        if(strcmp(argv[1],"-d")==0){

            char ip_address[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(server.sin_addr.s_addr), ip_address, INET_ADDRSTRLEN);
            clock_t end= clock();
            if(port!=0){
            printf("\nDEBUGGING INFORMATION\n--------------------\nPORT:%d\nIP:%s\nRUNNING TIME:%lf\n",port,ip_address,(double)(end-begin)/CLOCKS_PER_SEC);
            clock_t begin = clock();
            }
            else
            {
            printf("\nDEBUGGING INFORMATION\n--------------------\nPORT:%d\nIP:%s\nRUNNING TIME:%lf\n",8888,ip_address,(double)(end-begin)/CLOCKS_PER_SEC);
            }
            
        }
    }

    client_sock=accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
    if (client_sock < 0){
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    while((read_size = recv(client_sock, client_message, sizeof(client_message)-1,0 ))>0){

        regex_t reg;
        regmatch_t postmatch[2];

        int postMatch = regcomp(&reg,"[(^GET)|(^POST)|(HEAD)]",0);
        
        if (regexec(&reg, client_message,postMatch, postmatch,0)==0){

            char *token=strtok((client_message),"/");
            token = strtok(NULL,"/");
            puts(token);

            FILE *fp;
            if((fp=fopen(token,"r"))==NULL){
                puts("Status Code: 404 / File Not Found");
            }
            char buff[255];
            
            while(fgets(buff, 255, fp)!= NULL){
                puts(buff);
            }
        }
        else{

        }
        // write(client_sock , client_message , strlen(client_message));
        // puts(client_message);
        
    }
    if(read_size==0){
        puts("Client disconnected");

    }
    else if(read_size==-1){
        perror("receive failed");
    }

    close(socket_desc);
    return 0;
}