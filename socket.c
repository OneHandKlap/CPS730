#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <time.h>


int post(char file[], char info[]){

    FILE *fp=fopen(file,"w");
    if(fp!=NULL){
        char str[strlen(info)];
        strcpy(str,info);
        fwrite(str,1,strlen(str),fp);
        fclose(fp);

    }
    else{
        return 1;
    }
    return 0;

}
void slice_str(const char * str, char * buffer, int start, int end)
{
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}
void generateReply(int argCommand, char file[], char info[],int client_sock){

    if(argCommand==1){
        //STUFF FOR POST
        //retrieve POST function status code
        //retrieve HEADERS
        //FORMAT
        //PRINt/PUT/WRITE
        printf("writing: %s; to file %s",info,file);
        if(post(file,info)==0){
            send(client_sock,"Status Code: 200",17,0);
        }
        else{
            printf("Error");
        }

    }
    else{
        printf("error invalid command selection generateReply");
    }
}





int main(int argc, char ** argv){
    clock_t begin = clock();
    int socket_desc, client_sock, c ,read_size;
    struct sockaddr_in server, client;
    char client_message[2000];
    char client_message2[2000];
    char client_message3[2000];
    char* reply_message;
    int port=0;

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
    server.sin_addr.s_addr;
    inet_pton(AF_INET, "10.17.175.206", &(server.sin_addr.s_addr));
    if(port>=8000){
        server.sin_port=htons(port);
    }
    else{
    server.sin_port = htons(8888);
    }
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
            printf("\nDEBUGGING INFORMATION\n--------------------\nPORT:%d\nIP:%s\nRUNNING TIME:%lf\n",8888,ip_address,(double)((end-begin)/CLOCKS_PER_SEC));
            
        }
    }

    client_sock=accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
    if (client_sock < 0){
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    while((read_size = recv(client_sock, client_message, sizeof(client_message)-1,0 ))>0){
        puts(client_message);
        if(strcmp(client_message,"\0")==0){
            puts("empty message");
        } 
        regex_t reg;
        regmatch_t postmatch[2];
        fflush(stdin);
        int postMatch = regcomp(&reg,"[(^GET)|(^POST)|(HEAD)]",0);

        char *token=strtok((client_message)," ");
        
        char *requestType=token;
        token=strtok(NULL," ");
        char *fileName=token;

        if(strcmp(requestType,"POST")==0){
            while((read_size = recv(client_sock, client_message2, sizeof(client_message),0 ))>0){
            char *token2=strtok((client_message2),":");
            char* secondLine=token2;
            
            token2=strtok(NULL,":");
            int contentLength=atoi(token2);

            if(strcmp(secondLine,"Content-Length")==0){
                //printf("\n%s\n%s\n%d\n",requestType,fileName,contentLength);
                WAIT: while((read_size = recv(client_sock, client_message3, sizeof(client_message),0 ))>0){
                    if (strlen(client_message3)>2){
                        FILE *fp;
                        
                        if(fopen(fileName,"w")!=NULL){
                            char *token3=strtok(client_message3," ");
                            while (token3!=NULL){
                                puts(token3);
                                //fwrite(token3,1,sizeof(token3),fp);
                                token3=strtok(NULL," ");
                            }
                            
                            fclose(fp);
                        }
                        else{
                            puts("ERROR writing to file");
                        }

                    }
                    else{
                        goto WAIT;
                    }
                }
                }
            }
        }
        else{
            write(client_sock,"Error\n",6);
            write(client_sock,"IDIOT",5);
        }
        goto WAIT;
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