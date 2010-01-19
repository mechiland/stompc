#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <xlocale.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdlib.h>
#include "config.h"           
#include "error_handle.h" 
#include "delimiter_frame.h"
                                  
static const int MAXPENDING = 5; 
void HandleTCPClient(int clntSocket);

int main(int argc, char *argv[]) 
{
	if (argc != 2) 
		die_with_user_message("Parameter(s)", "<Server Port>"); 

	in_port_t servPort = atoi(argv[1]); 

	int servSock; 
	if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
		die_with_system_message("socket() failed"); 

	struct sockaddr_in servAddr; 
	memset(&servAddr, 0, sizeof(servAddr)); 
	servAddr.sin_family = AF_INET; 
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servAddr.sin_port = htons(servPort); 

	if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) 
		die_with_system_message("bind() failed"); 

	if (listen(servSock, MAXPENDING) < 0) 
		die_with_system_message("listen() failed"); 

	for (;;) { 
		struct sockaddr_in clntAddr; 
		socklen_t clntAddrLen = sizeof(clntAddr); 
		int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen); 
		if (clntSock < 0) 
			die_with_system_message("accept() failed"); 

		char clntName[INET_ADDRSTRLEN]; 
		if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, 
			sizeof(clntName)) != NULL) 
			printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port)); 
		else 
			puts("Unable to get client address");
		HandleTCPClient(clntSock); 
	} 
}      

void HandleTCPClient(int clntSocket) {                          
	FILE *f = fdopen(clntSocket, "r+");
	if (f == NULL) {
		die_with_system_message("fdopen error");
	}
	
	uint8_t buffer[BUFSIZE]; 
	size_t num_recv = get_message(f, buffer, BUFSIZE);
	size_t num_appended = snprintf((char *)buffer + num_recv, BUFSIZE - num_recv, "%s", " from server");
	send_message(buffer, num_recv + num_appended, f);
	
	fclose(f); 
} 
