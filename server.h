#define OK 200
#define CREATED 201
#define BAD_REQUEST 400
#define NOT_FOUND 404

#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h> 
#include <regex.h>

void slice_str(const char * str, char * buffer, int start, int end);
int content_chars_length(int n);
int send_status(char operation, int succeed);
void write_headers(int client_sock, int message_length);
char* get_filename(char* file);
void send_get(int client_sock, char* file);
void send_head(int client_sock, char* file);
void send_post(int client_sock, char* file);
void process_request(int client_sock, char* type, char* file);
void clear_buffer();


