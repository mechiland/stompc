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
		int i = 0;
		int bytes_recv, total_bytes_recv = 0;
		                                  
		while(i < 3) {
			bytes_recv = recv(clientSock, buf + total_bytes_recv, BUFSIZE - total_bytes_recv, 0);
			if (bytes_recv <=0)
				break;
				                              
			total_bytes_recv += bytes_recv;
			i++;
		}            
		
		if (total_bytes_recv < 0)
			continue;                                           
			                                                                
		stomp_frame *f = stomp_frame_parse(buf, total_bytes_recv); 
		if (f == NULL) 
			continue; 
		            			
		stomp_frame *rf = stomp_process(f);
		
		if (rf) {
			scs *s = stomp_frame_serialize(rf); 
                     
			if (send(clientSock, scs_get_content(s), scs_get_size(s), 0) != scs_get_size(s)) {
				perror("send frame data failed");
			}                                                                     
			scs_free(s);
			stomp_frame_free(rf);
		} 
            
		stomp_frame_free(f);
	}
}                          

