#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include "tcp_server.h"
#include "stomp_protocol.h"

int start_server()
{
	int servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock < 0) 
		die_with_system_message("socket() failed"); 

	struct sockaddr_in servAddr; 
	memset(&servAddr, 0, sizeof(servAddr)); 
	servAddr.sin_family = AF_INET; 
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servAddr.sin_port = htons(PORT); 

	if (bind(servSock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) 
		die_with_system_message("bind() failed"); 

	if (listen(servSock, MAXPENDING) < 0) 
		die_with_system_message("listen() failed");

	return servSock;
}    

void handle_tcp_client(int clientSock) {  
	char buf[BUFSIZE];

	for (;;) {    
		int bytes_recv = recv(clientSock, buf, BUFSIZE, 0);
		if (bytes_recv < 0)
			continue;

		stomp_frame *f = stomp_frame_create(buf, bytes_recv); 
		stomp_frame *response_frame = stomp_process(f); 

		int response_size;
		char *response_content = stomp_frame_serialize(response_frame, &response_size);
		if (send(clientSock, response_content, response_size, 0) != response_size) {
			perror("send frame data failed");
		}                                                                     
            
		free(response_content);
		stomp_frame_free(f);
		stomp_frame_free(response_frame);
	}
}                          

