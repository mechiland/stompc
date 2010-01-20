#include <sys/socket.h>
#include <arpa/inet.h> 
#include "tcp_server.h"
#include "error_handler.h"
                          

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

