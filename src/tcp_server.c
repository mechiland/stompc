#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/select.h>
#include "tcp_server.h"
#include "stomp_protocol.h"

static void handle_new_connection(fd_set *socks, int *connections, int *connection_num);
static void handle_tcp_client(int clntSocket);
static void interrupt_handler(int sig); 
static void handle_fatal_error(const char* message);

static void	set_reuse_addr();
static void set_interrupt_handler(); 
static void set_non_blocking();

static int sock;

static void handle_fatal_error(const char* message)
{
	die_with_system_message(message);
	close(sock);	
}   

static void	set_reuse_addr()
{
	int reuse_addr = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr,	sizeof(reuse_addr));
}

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
		handle_fatal_error("sigfillset() failed");
	}
	handler.sa_flags = 0;

	if(sigaction(SIGINT, &handler, 0) < 0)
	{
		handle_fatal_error("sigaction() failed");
	}	
}

static void set_non_blocking() {
	int opts;
	opts = fcntl(sock, F_GETFL);
	if (opts < 0) {
		handle_fatal_error("get socket flag failed"); 
	}                                              
	opts = opts | O_NONBLOCK;
	if (fcntl(sock, F_SETFL, opts) < 0) {
		handle_fatal_error("set socket nonblocking failed"); 
	}
}

void start_server()
{
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		handle_fatal_error("socket() failed"); 
	} 

	struct sockaddr_in servAddr; 
	memset(&servAddr, 0, sizeof(servAddr)); 
	servAddr.sin_family = AF_INET; 
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servAddr.sin_port = htons(PORT); 

	if (bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr)) < 0) {
		handle_fatal_error("bind() failed"); 
	}		

	if (listen(sock, MAXPENDING) < 0) {
		handle_fatal_error("listen() failed");
	}

	set_interrupt_handler();
	set_reuse_addr();
	set_non_blocking();	

	int connections[FD_SETSIZE];
	int connection_num = 0;
	fd_set socks;
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	printf("Start the loop\n"); 
	printf("Server sock: %d\n", sock); 
	printf("Maximum sock num: %d\n", FD_SETSIZE); 
	for(;;) {
		FD_ZERO(&socks);
		FD_SET(sock, &socks); 
		int i;
		for(i = 0; i < connection_num; ++i) {
			FD_SET(connections[i], &socks);
		}		

		int read_ready_socks = select(FD_SETSIZE, &socks, NULL, NULL, &timeout); 

		if (read_ready_socks < 0) {
			handle_fatal_error("select error");
		}                                      
		if (read_ready_socks == 0) {
			continue;
		}

		if (FD_ISSET(sock, &socks)) { 
			printf("Handling new client connection\n");
			handle_new_connection(&socks, connections, &connection_num);
		}                    

		for(i = 0; i < connection_num; ++i)	{
			printf("Handling new client data\n");
			if (FD_ISSET(connections[i], &socks)) {
				handle_tcp_client(connections[i]);
			}
		}       		
	}
}    

static void handle_new_connection(fd_set *socks, int *connections, int *connection_num)
{
	struct sockaddr_in clntAddr; 
	socklen_t clntAddrLen = sizeof(clntAddr); 
	int clntSock = accept(sock, (struct sockaddr *) &clntAddr, &clntAddrLen);

	connections[(*connection_num)++] = clntSock;

	if (clntSock < 0) 
		die_with_system_message("accept() failed"); 	
}

static void handle_tcp_client(int clientSock) 
{  
	char buf[BUFSIZE];

	int i = 0;
	int bytes_recv, total_bytes_recv = 0;

	while(1) {
		bytes_recv = recv(clientSock, buf + total_bytes_recv, BUFSIZE - total_bytes_recv, 0);
		if (bytes_recv <=0)
			break;

		total_bytes_recv += bytes_recv;
		i++;
	}            

	printf("Got %d bytes: %s\n", total_bytes_recv, buf);
	if (total_bytes_recv <= 0)
		return;                                           

	stomp_frame *f = stomp_frame_parse(buf, total_bytes_recv); 
	if (f == NULL) 
		return; 

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

