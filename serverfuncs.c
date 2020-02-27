#include "server.h"

void chop_newLine(char *s) {
    while(*s && *s != '\n' && *s != '\r') s++;
    *s = 0;
}

int check_config(const char * str){
	FILE * fp;
	int isFound=0;
	fp=fopen("conf.cfg","r");
	char line[1000];
	char token[50];
	if(fp!=NULL){
		while (fscanf(fp, "%s", token) != EOF) {
			if(strcmp(token,str)==0){
					isFound=1;
					break;
			}
		}
	}
	else{
		perror("Missing config file");
	}
	fclose(fp);
	return isFound;
}

int check_forbidden(const char * str){
	FILE * fp;
	int isFound=0;
	fp=fopen("forbidden.cfg","r");
	char line[1000];
	char token[50];
	if(fp!=NULL){
		while (fscanf(fp, "%s", token) != EOF) {
			if(strcmp(token,str)==0){
					isFound=1;
					break;
			}
		}
	}
	else{
		perror("Missing forbidden file");
	}
	fclose(fp);
	return isFound;
}

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

int send_get(int client_sock, char* file){
	FILE* f;
	char* temp_filename = get_filename(file);	
	int status=OK;

	if((f = fopen(temp_filename,"r")) == NULL){

		status=NOT_FOUND;
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
	return(0);
	}
	else{
		send_status(client_sock, status);
		write_headers(client_sock, strlen(S));
		return(1);
	}
}


int send_head(int client_sock, char* file){
	FILE* f;
	char* temp_filename = get_filename(file);
	int status=OK;	

	if((f = fopen(temp_filename,"r")) == NULL){
		status=NOT_FOUND;
	}
	char S[1000];
	if(status == OK){
		while(fgets(S, 1000, f) != NULL){
			fflush(stdout);
		}
		send_status(client_sock, status);
		write_headers(client_sock, strlen(S));
		return(0);
		fclose(f);
	}
	else{
	send_status(client_sock, status);
	write_headers(client_sock, strlen(S));
	return(1);
	}
	
}

int send_post(int client_sock, char* file){
	FILE *fp;
	int status = CREATED;
	if((fp = fopen(get_filename(file),"w+")) == NULL){

		status = NOT_FOUND;
	}
	int hasHitEnter=0;
	char client_message2[1000];
	char client_message3[1000];
	int read_size;
	if(fp != NULL){
	while((read_size = recv(client_sock, client_message2, sizeof(client_message2),0 ))>0){

				char *token2=strtok((client_message2),":");
                char* secondLine=token2;
                
                
                

                if(strcmp(secondLine,"Content-Length")==0){
					
					token2=strtok(NULL,":");
					int contentLength=atoi(token2);
					int chars_written=0;
					char toWrite[contentLength];
					while((read_size = recv(client_sock, client_message3, sizeof(client_message3),0 ))>0){
							chop_newLine(client_message3);
							if(strcmp(client_message3,"\0")==0 || strcmp(client_message3,"\n")==0){
								if (hasHitEnter==1){
									//add last string then print
									
									
									fwrite(toWrite,sizeof(char),sizeof(toWrite),fp);
									fclose(fp);
									send_status(client_sock, status);
									write_headers(client_sock, contentLength);
									return(0);
								}
								else{
									
									toWrite[chars_written++]='\0';
									
									hasHitEnter=1;
								}
								
							}
							else{

								int i=0;
								while(i<=strlen(client_message3)&&chars_written<=contentLength){
									toWrite[chars_written]=client_message3[i];
									chars_written++;
									i++;
								}
							}
                            // char toWrite[contentLength];
                            // slice_str(client_message3,toWrite,0,contentLength);
                            
                            // fwrite(toWrite,sizeof(char),sizeof(toWrite),fp);
							// fclose(fp);
							// send_status(client_sock, status);
                            // write_headers(client_sock, contentLength);
							// return(0);
                        }

                    }
					status=BAD_REQUEST;
					send_error(client_sock,status);
					return(1);
                }
	}
}

/*
GET request implementation
to-do: HEAD and POST
*/
int process_request(int client_sock, char* type, char* file){
	if(strcmp(type, "GET") == 0){
		if(send_get(client_sock, file)==0){
			printf("Success");
			return (0);
		}
		else{
			printf("FAIL");
			return (1);
		}
	}
	if(strcmp(type, "HEAD") == 0){
		if(send_head(client_sock, file)==0){
			return (0);
			printf("Success");
		}
		else{
			printf("FAIL");
			return (1);
		}
	}
	if(strcmp(type, "POST") == 0){
		if(send_post(client_sock, file)==0){
			return (0);
			printf("Success");
		}
		else{
			printf("FAIL");
			return (1);
		}
	}
	return(1);
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
		case 403:
			sprintf(status_code_message, "\nStatus Code: %d", FORBIDDEN);
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
		case 501:
			sprintf(status_code_message, "\nStatus Code: %d", NOT_IMPLEMENTED);
			write(client_sock, status_code_message, strlen(status_code_message));
			break;
		default:
			write(client_sock, "\nStatus Code: 500", 19);
			break;
	}
}

/*
Function that sends a default headers in response to unsuccessful requests
*/
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

}