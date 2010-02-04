#include <sys/socket.h>
#include <arpa/inet.h> 
#include "tcp_server.h"                          

static int should_start_client(int argc, char const *argv[]);

int main(int argc, char const *argv[]) 
{		
	if(should_start_client(argc, argv))
		connect_stomp_server();
	else
		start_server();
	return 0;
}  

static int should_start_client(int argc, char const *argv[])
{
	int client_mode = 0;
	if(argc > 1){
		if(strcmp("client", argv[1]) == 0){
			client_mode = 1;
		}
	}
	return client_mode;
}