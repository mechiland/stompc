#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "config.h"
#include "error_handle.h"    
#include "delimiter_frame.h"

int main(int argc, char** argv) 
{
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;                                      
	    
	int result = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
	if(result == 0) {
		die_with_user_message("inet_pton() failed", "invalid address string");
	}
	else if(result < 0) {
		die_with_system_message("inet_pton() failed");
	}
	server_addr.sin_port = htons(3000);

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock < 0) {
		die_with_system_message("socket() failed");
	}                                                                          
	
	if(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)	{
		die_with_system_message("connect() failed");
	}
    
	FILE* f = fdopen(sock, "r+");   
	if (f == NULL) {
		die_with_system_message("fdopen failed");
	}
	                                     
	char *message = "hello, world";
	send_message((uint8_t *)message, strlen(message), f);
                                          
	uint8_t buf[BUFSIZE];
	memset(buf, 0, BUFSIZE);
	printf("Recv %d length message\n", get_message(f, buf, BUFSIZE));
	printf("Recv: %s", buf);                         
	putc('\n', stdout);

	fclose(f);

	return 0;
}       

