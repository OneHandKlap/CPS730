#include "server.h"


struct Request{
	char* type;
	char* path;
	char* protocol;
	char* fileEnding;
}client_request;


int main(int argc, char *argv[]){


	
	//Create a socket
	int err_flag=0;
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
	write(client_sock, "Disconnect by typing : '!Q'\n",28);

	//main loop to handle client requests
	MAIN:
	while((read_size = recv(client_sock, client_message, sizeof(client_message),0)) > 0){	
		
	//divide incoming string into tokens

		char* token = strtok(client_message, " ");
		int i = 1;
		while (token != NULL) { 
			
			chop_newLine(token);
			if ((strcmp(token, "\0")==0)||(strcmp(token,"\n")==0)){
				break;
			}
			
			if((strcmp(token, "GET") != 0 && strcmp(token, "POST") != 0  && strcmp(token, "HEAD") != 0 && strcmp(token,"!Q")!=0)&&(i==1)){
				
				
				send_error(client_sock, NOT_IMPLEMENTED);
				err_flag=1;
				break;				
			}
			else if(i==1){
				chop_newLine(token);
				puts(token);
				if(strcmp(token,"!Q")==0){
					exit(0);
				}
				client_request.type=token;
				
			}
			else if(i == 2){
				if(token==NULL){
					send_error(client_sock, BAD_REQUEST);
					err_flag=1;
					break;
				}
				client_request.path = token;
			}
			else if(i == 3){
				if (token ==NULL){
					send_error(client_sock, BAD_REQUEST);
					err_flag=1;
					break;
				}
				client_request.protocol = token;
			}
			token = strtok(NULL, " ");
			++i;	
		}
		if(err_flag==1){
			err_flag=0;
			break;
		}
		if(client_request.protocol == NULL && client_request.type==NULL && client_request.path==NULL){
			goto MAIN;
		}
		
		if (client_request.protocol != NULL && client_request.type!=NULL && client_request.path!=NULL){
			chop_newLine(client_request.protocol);
			char temp[sizeof(client_request.path)];
			char*fileEnding = malloc(sizeof(client_request.path));
			
			memcpy(fileEnding,client_request.path,sizeof(client_request.path)*4);
			
			fileEnding=strtok(fileEnding,".");
			fileEnding=strtok(NULL,".");
			
			if((check_forbidden(client_request.path)==1)){
				send_error(client_sock, FORBIDDEN);

				break;
				
			}

			if((check_config(client_request.protocol)==1) & (check_config(fileEnding)==1)){
				process_request(client_sock, client_request.type, client_request.path);
			}
			else{
				send_error(client_sock,BAD_REQUEST);
				break;
				
			}
		}
		else{
			send_error(client_sock,BAD_REQUEST);
			break;

		}
		

	
	}

    if(read_size==0){
        puts("Client disconnected");

    }
    else if(read_size==-1){
        perror("receive failed");
    }

	exit (0);
}
