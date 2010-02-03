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
#include "stomp.h"                        

static void create_server_sock_and_listen();
static void handle_new_connection(fd_set *socks);
static void handle_tcp_client(int client_socket);
static void interrupt_handler(int sig); 
static void handle_fatal_error(const char* message);

static void	set_reuse_addr();
static void set_interrupt_handler(); 
static void set_non_blocking(int sock); 
static void set_file_descriptors(fd_set *socks);

static int send_data(int sock, char *buf, int size);
static void close_client_socket(int sock);

static int sock;
int connections[FD_SETSIZE];
int connection_num = 0;
static stomp *stomp_machine;

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
	puts("Shutting down server, stop listening."); // TODO: we need better way to handle logs
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

static void set_non_blocking(int sock) {
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

static int bind_to_port(int sock, int port_no){
	struct sockaddr_in servAddr; 
	memset(&servAddr, 0, sizeof(servAddr)); 
	servAddr.sin_family = AF_INET; 
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servAddr.sin_port = htons(port_no); 

	return bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr));
}

static void create_server_sock_and_listen()
{
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		handle_fatal_error("socket() failed"); 
	} 
	set_reuse_addr();
	set_non_blocking(sock);	

	if (bind_to_port(sock, PORT) < 0) {
		handle_fatal_error("bind() failed"); 
	}		

	if (listen(sock, MAXPENDING) < 0) {
		handle_fatal_error("listen() failed");
	}	
}                           

void start_server()
{                           
	create_server_sock_and_listen();
	set_interrupt_handler();

	for(;;) {
		fd_set socks;
		set_file_descriptors(&socks);

		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		
		int read_ready_socks = select(FD_SETSIZE, &socks, NULL, NULL, &timeout); 

		if (read_ready_socks < 0) {
			handle_fatal_error("select error");
		}                                      
		if (read_ready_socks == 0) {
			continue;
		}

		if (FD_ISSET(sock, &socks)) { 
			printf("Handling new client connection\n");
			handle_new_connection(&socks);
		}                    
         
		int i;
		for(i = 0; i < connection_num; ++i)	{
			if (connections[i] >= 0) { //TODO: need better way to remove a sock from the array
				printf("Handling new client data\n");
				if (FD_ISSET(connections[i], &socks)) {
					handle_tcp_client(connections[i]);
				}                         
			}
		}       		
	}
}      

static void set_file_descriptors(fd_set *socks)
{
	FD_ZERO(socks);
	FD_SET(sock, socks); 
	int i;
	for(i = 0; i < connection_num; ++i) {
		if (connections[i] >= 0) { //TODO: need better way to remove a sock from the array
			FD_SET(connections[i], socks);			
		}
	}		

}

static void handle_new_connection(fd_set *socks)
{
	struct sockaddr_in clntAddr; 
	socklen_t clntAddrLen = sizeof(clntAddr); 
	int client_sock = accept(sock, (struct sockaddr *) &clntAddr, &clntAddrLen);

	if (client_sock < 0) 
		die_with_system_message("accept() failed"); 	                     
		
	connections[connection_num++] = client_sock; 
	set_non_blocking(client_sock);  
	                                          
	// TODO: to handle multiple clients same time, we need store all stomp machines
	stomp_machine = stomp_create(client_sock, send_data, close_client_socket);
}

static void handle_tcp_client(int client_sock) 
{  
	char buf[BUFSIZE];

	int i = 0;
	int bytes_recv, total_bytes_recv = 0;

	for(;;) {
		bytes_recv = recv(client_sock, buf + total_bytes_recv, BUFSIZE - total_bytes_recv, 0);
		if (bytes_recv <=0)
			break;

		total_bytes_recv += bytes_recv;
		i++;
	}            

	printf("Got %d bytes: %s\n", total_bytes_recv, buf);
	if (total_bytes_recv <= 0)
		return;                                           

	stomp_receive(stomp_machine, buf, total_bytes_recv);
}                          

static int send_data(int sock, char *buf, int size) 
{
	if (send(sock, buf, size, 0) != size) {
		perror("send frame data failed");
	}                                         
	return size;                            
}            

static void close_client_socket(int sock)
{
	close(sock);
	int i;
	for(i = 0; i < connection_num; ++i) {
		if (connections[i] == sock) {
			connections[i] = -1; //TODO: need better way to remove a sock from the array
		}
	}		
}