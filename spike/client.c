#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "error_handle.h"    

#define BUFSIZE 10

int main(int argc, char** argv) 
{
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	int result = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
	if(result == 0)
	{
		die_with_user_message("inet_pton() failed", "invalid address string");
	}
	else if(result < 0)
	{
		die_with_system_message("inet_pton() failed");
	}
	server_addr.sin_port = htons(3000);

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock < 0)
	{
		die_with_system_message("socket() failed");
	}
	if(connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		die_with_system_message("connect() failed");
	}

	char* s = "hello world";
	size_t slen = strlen(s);

	if (slen != send(sock, s, slen, 0)) 
	{
		die_with_user_message("send()", "failed");
	}

	unsigned int total_bytes_received = 0;
	fputs("Received: ", stdout);
	while(total_bytes_received < slen) 
	{
		char buffer[BUFSIZE];
		int num_bytes = recv(sock, buffer, BUFSIZE - 1, 0);  
		if (num_bytes <= 0)
		{
			die_with_system_message("recv() failed");
		}
		total_bytes_received += num_bytes;
		buffer[num_bytes] = '\0';
		fputs(buffer, stdout);
	}                         

	fputc('\n', stdout);

	close(sock);

	return 0;
}