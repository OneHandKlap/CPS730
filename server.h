#define OK 200
#define CREATED 201
#define BAD_REQUEST 400
#define FORBIDDEN 403
#define NOT_FOUND 404
#define NOT_IMPLEMENTED 501

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h> 
#include <regex.h>
#include <errno.h>

int check_config(const char * str);
void chop_newLine( char *s );
void slice_str(const char * str, char * buffer, int start, int end);
int content_chars_length(int n);
void write_headers(int client_sock, int message_length);
char* get_filename(char* file);
int send_get(int client_sock, char* file);
int send_head(int client_sock, char* file);
int send_post(int client_sock, char* file);
int process_request(int client_sock, char* type, char* file);
void send_status(int client_sock, int status);
void send_error(int client_sock, int status_code);
int check_forbidden(const char * str);


