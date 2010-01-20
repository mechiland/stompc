#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include "tcp_server.h"
#include "stomp_frame.h"

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
	uint8_t buf[BUFSIZE];
	
	for (;;) {    
		int bytes_recv = recv(clientSock, buf, BUFSIZE, 0);
		if (bytes_recv < 0)
			continue;
			
		frame f = stomp_frame_create(buf, bytes_recv); 
		if (f->size <= 0)
			continue;
/*			
		stomp_message *message = stomp_create_message(frame);
		stomp_message *response = stomp_process(message);
		frame *response_frame = stomp_compse_frame(response);
*/		
		
		if (send(clientSock, f->content, f->size, 0) != f->size) {
			die_with_system_message("send frame data failed");
		}                                                                     
		
		stomp_frame_destroy(f);
/*
		stomp_frame_destroy(response_frame)
		stomp_message_destroy(message, response);
*/
	}
}      
