#include "server.h"

void slice_str(const char * str, char * buffer, int start, int end)
{
    int j = 0;
    for ( int i = start; i < end; i++, j++ ) {
        buffer[j] = str[i];
    }
}

/*
Helper function needed for dynamic memory allocation
Input: 123 (int - a number)
Output: 3 (int - number of characters in number)
*/
int content_chars_length(int n){
	int count;
 	while (n != 0) {
       		 n /= 10;
        	 ++count;
    	}
	return count;
}

void send_status(int client_sock, int status){
	char status_code_message[20];

	switch(status){
		case 200:
			sprintf(status_code_message, "\nStatus Code: %d", OK);
			write(client_sock, status_code_message, strlen(status_code_message));
			break;
		case 201:
			sprintf(status_code_message, "\nStatus Code: %d", CREATED);
			write(client_sock, status_code_message, strlen(status_code_message));
			break;
		case 400:
			sprintf(status_code_message, "\nStatus Code: %d", BAD_REQUEST);
			write(client_sock, status_code_message, strlen(status_code_message));
			break;
		case 404:
			sprintf(status_code_message, "\nStatus Code: %d", NOT_FOUND);
			write(client_sock, status_code_message, strlen(status_code_message));
			break;
		default:
			write(client_sock, "\nStatus Code: 500", 19);
			break;
	}
}

/*
Headers to be displayed to client when they receive a response from server
*/
void write_headers(int client_sock, int message_length){
        struct tm strtime;
        time_t timeoftheday;
        struct tm *loc_time;
        timeoftheday=time(NULL);
        loc_time=localtime(&timeoftheday);
	
		char message [21];
		sprintf(message, "\nContent-Length: %d", message_length);

        write (client_sock,"\nHost-Name: 10.17.175.206",25);
        write(client_sock,message,22);
        write(client_sock,"\n",2);
        write(client_sock,asctime(loc_time),strlen(asctime(loc_time)));
        write(client_sock,"Content-type: txt/html\n",23);
        write(client_sock,"\n",2);
}

/*
INPUT: char* file contains name of a file that's being read as a part of client's request.
char* file always starts with '/' character.
OUTPUT: char* get_filename returns the name of file without '/' character. 
*/
char* get_filename(char* file){
	if(file[0] == '/'){
		file++;
	}
	return file;
}

void send_get(int client_sock, char* file){
	FILE* f;
	char* temp_filename = get_filename(file);	
	int status = OK;

	if((f = fopen(temp_filename,"r")) == NULL){
		fprintf(stderr, "Can't resolve the file\n");
		status = NOT_FOUND;
	}
	char S[1000];
	
	if(status == OK){
			while(fgets(S, 1000, f) != NULL){
			fflush(stdout);
		}

		send_status(client_sock, status);
		write_headers(client_sock, strlen(S));
		write(client_sock, S, strlen(S));

		fclose(f);
	}
	else{
		send_status(client_sock, status);
		write_headers(client_sock, strlen(S));
		write(client_sock, S, strlen(S));
		close(client_sock);
	}
}

void send_head(int client_sock, char* file){
	FILE* f;
	char* temp_filename = get_filename(file);	
	int status = OK;

	if((f = fopen(temp_filename,"r")) == NULL){
		fprintf(stderr, "Can't resolve the file\n");
		status = NOT_FOUND;
	}
	char S[1000];

	while(fgets(S, 1000, f) != NULL){

		//printf("%s\n", S);
		fflush(stdout);
	}
	send_status(client_sock, status);
	write_headers(client_sock, strlen(S));

	fclose(f);
	free(temp_filename);

	if(status != 200){
		close(client_sock);
	}
}

void send_post(int client_sock, char* file){
	FILE *fp;
	int status = CREATED;
	if((fp = fopen(get_filename(file),"w+")) == NULL){
		fprintf(stderr, "Can't resolve the file\n");
		status = NOT_FOUND;
	}
	char client_message2[1000];
	char client_message3[1000];
	int read_size;
	if(fp != NULL){
	while((read_size = recv(client_sock, client_message2, sizeof(client_message2),0 ))>0){
                char *token2=strtok((client_message2),":");
                char* secondLine=token2;
                
                
                token2=strtok(NULL,":");
                int contentLength=atoi(token2);

                if(strcmp(secondLine,"Content-Length")==0){
					while((read_size = recv(client_sock, client_message3, sizeof(client_message3),0 ))>0){
                            char toWrite[contentLength];
                            slice_str(client_message3,toWrite,0,contentLength);
                            
                            fwrite(toWrite,sizeof(char),sizeof(toWrite),fp);
							fclose(fp);
							send_status(client_sock, status);
                            write_headers(client_sock, contentLength);
                            close(client_sock);
                        }
                    }
                }
	}
}

/*
GET request implementation
to-do: HEAD and POST
*/
void process_request(int client_sock, char* type, char* file){
	if(strcmp(type, "GET") == 0){
		send_get(client_sock, file);
	}
	if(strcmp(type, "HEAD") == 0){
		send_head(client_sock, file);
	}
	if(strcmp(type, "POST") == 0){
		send_post(client_sock, file);
	}
}

void send_error(int client_sock, int status_code){
	struct tm strtime;
	time_t timeoftheday;
	struct tm *loc_time;
	timeoftheday=time(NULL);
	loc_time=localtime(&timeoftheday);

	send_status(client_sock, status_code);
	
	write(client_sock, "\nContent-Length: 0", 19);
	write(client_sock,"\nHost-Name: 10.17.175.206",25);
	write(client_sock,"\n",2);
	write(client_sock,asctime(loc_time),strlen(asctime(loc_time)));
	write(client_sock,"\n",2);
	close(client_sock);
}

void clear_buffer(){
	int c;
	while ((c = getchar()) != '\n' && c != EOF) { }
}
