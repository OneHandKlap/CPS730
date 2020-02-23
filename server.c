#include "server.h"

struct Request{
	char* type;
	char* path;
	char* protocol;
	char* fileEnding;
}client_request;


int main(int argc, char *argv[]){

	//Create a socket
	int port;
	int verbose=0;
	if(argc>1){
		for(int i=0; i<argc;i++){
			if(strcmp(argv[i],"-d")==0){
				verbose=1;
			}
			if(strcmp(argv[i],"-p")==0){
				port = atoi(argv[i+1]);
			}
		}
	}
	int socket_desc;
	int client_sock, c, read_size;
	char client_message[1000];
	struct sockaddr_in server, client;
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(verbose==1 & socket_desc > -1){
		printf("Socket created\n");
	}
	else if(socket_desc == -1){
		printf("Could not create a socket\n");
		return 1;
	}

	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	if(port>0){
		server.sin_port = htons(port);
	}
	else{
		server.sin_port = htons(8888);
	}

	//bind socket
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
       	       perror("bind failed");
               return 1;
 	}
	 else if(verbose==1){
    	printf ("Bind successful\n");
		if(port>0){
		printf ("Listening at port: %d\n",port);
		}
		else{
			printf("Listening at port: 8888\n");
		}
	 }
	//listen to socket

	listen(socket_desc, 3);

        //Accept incoming
   	puts("Waiting for incoming connections ... .. . ..");
	
	c=sizeof(struct sockaddr_in);

        //connect
   	client_sock=accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
   	 if (client_sock < 0){
       		 perror("accept failed");
        	 return 1;
 	}
   	 puts("Connection accepted");

	//main loop to handle client requests
	while((read_size = recv(client_sock, client_message, sizeof(client_message),0)) > 0){
		if(strcmp(client_message, "\0") == 0){
			printf("Empty message\n");
		}
		else{
			puts(client_message);
		}
		fflush(stdout);

	//divide incoming string into tokens

		char* token = strtok(client_message, " ");
		int i = 1;
		while (token != NULL) { 
			if(strcmp(token, "GET") == 0 || strcmp(token, "POST") == 0 || strcmp(token, "HEAD") == 0){
				client_request.type = token;
			}	
			if(i == 2){
                client_request.path = token;

            }

			if(i == 3){
				client_request.protocol = token;
				chop_newLine(client_request.protocol);
			}
        		token = strtok(NULL, " ");
			++i;
    		}

		char temp[sizeof(client_request.path)];
		char *fileEnding;
		memcpy(fileEnding,client_request.path,sizeof(client_request.path)*4);

		fileEnding=strtok(fileEnding,".");
		fileEnding=strtok(NULL,".");


		if((check_config(client_request.protocol)==1)& (check_config(fileEnding)==1)){
			process_request(client_sock, client_request.type, client_request.path);
		}
		else{
			puts("400 Bad Request: wrong protocol");
			close(client_sock);
		}
	}

    if(read_size==0){
        puts("Client disconnected");

    }
    else if(read_size==-1){
        //perror("receive failed");
    }


}
