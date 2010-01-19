#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <stdlib.h>       
                          
#define BUFSIZE 65537       
#define MAXPENDING 5      
#define PORT 61613            

void die_with_user_message(const char *message, const char *detail);
void die_with_system_message(const char *message);                    
int start_server();

void handle_tcp_client(int clntSocket);
int receive_frame(int clientSock, uint8_t *buf, int size);
int	send_frame(int clientSock, uint8_t *buf, int size);

int main() 
{        
	int servSock = start_server();   

	for (;;) { 
		struct sockaddr_in clntAddr; 
		socklen_t clntAddrLen = sizeof(clntAddr); 
		int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen); 
		if (clntSock < 0) 
			die_with_system_message("accept() failed"); 

		handle_tcp_client(clntSock); 
	} 
}  

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
		int frame_size = receive_frame(clientSock, buf, BUFSIZE); 
		if (frame_size >0) {
			send_frame(clientSock, buf, frame_size);
		}
	}
} 

int receive_frame(int clientSock, uint8_t *buf, int size)
{
	int bytes_recv = recv(clientSock, buf, size, 0);
	if (bytes_recv < 0)
		return -1;    
		
	int i, frame_end = -1;
	for(i = 0; i < bytes_recv; i++) {
		if(buf[i] == '\0') {
			frame_end = i;
			break;
		}
	}                 

	return frame_end;
}

int	send_frame(int clientSock, uint8_t *buf, int size)
{
	int bytes_sent;
	buf[size] = '\0';
	if ((bytes_sent = send(clientSock, buf, size + 1, 0)) != size + 1) {
		die_with_system_message("send frame data failed");
	}                             
	return bytes_sent;
}   

void die_with_user_message(const char *message, const char *detail)
{
	fputs(message, stderr);
	fputs(": ", stderr);
	fputs(detail, stderr);
	fputc('\n', stderr);
	exit(1);
}   

void die_with_system_message(const char *message)
{
	perror(message);
	exit(1);
}