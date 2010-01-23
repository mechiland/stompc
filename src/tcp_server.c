#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include "tcp_server.h"
#include "stomp_protocol.h"
     
static void handle_tcp_client(int clntSocket);
static void interrupt_handler(int sig); 

static int sock;

static void interrupt_handler(int sig) 
{
	puts("Shutting down server, stop listening.");
	close(sock);
}   

static void set_interrupt_handler() 
{
	struct sigaction handler;
	handler.sa_handler = interrupt_handler;
	if(sigfillset(&handler.sa_mask) < 0)
	{
		die_with_system_message("sigfillset() failed");
		close(sock);
	}
	handler.sa_flags = 0;
	
	if(sigaction(SIGINT, &handler, 0) < 0)
	{
		die_with_system_message("sigaction() failed");
		close(sock);
	}	
}

void start_server()
{
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		die_with_system_message("socket() failed"); 
		close(sock);		
	} 

	struct sockaddr_in servAddr; 
	memset(&servAddr, 0, sizeof(servAddr)); 
	servAddr.sin_family = AF_INET; 
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servAddr.sin_port = htons(PORT); 

	if (bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
		die_with_system_message("bind() failed"); 
		close(sock);
	}		

	if (listen(sock, MAXPENDING) < 0) {
		die_with_system_message("listen() failed");
		close(sock);
	}
	
	set_interrupt_handler();
	
	for (;;) { 
		struct sockaddr_in clntAddr; 
		socklen_t clntAddrLen = sizeof(clntAddr); 
		int clntSock = accept(sock, (struct sockaddr *) &clntAddr, &clntAddrLen); 
		if (clntSock < 0) 
			die_with_system_message("accept() failed"); 

		handle_tcp_client(clntSock); 
	} 	
}    

static void handle_tcp_client(int clientSock) 
{  
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

