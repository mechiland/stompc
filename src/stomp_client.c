#include <string.h>
#include <xlocale.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <sys/uio.h>
#include <stdlib.h>

#include "stomp_client.h"
#include "tcp_server.h"
#include "scs.h"

static void error_and_exit(char *msg)
{
	fprintf(stderr, msg);
	exit(1);
}

static int connect_to_server(char * address, int portno)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error_and_exit("ERROR opening socket");
    struct hostent *server = gethostbyname(address);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error_and_exit("ERROR connecting stomp server"); 
   
	return sockfd;
}

static void send_frame(int sockfd, stomp_frame * frame)
{
	if(frame == NULL)
		return;
		
	printf("Send %s command to server.\n", get_verb(frame));
	scs *s = stomp_frame_serialize(frame);
	char *data = scs_get_content(s);
	
	int n = write(sockfd, data, data == NULL? 0 : strlen(data));
    if (n < 0) 
         error_and_exit("ERROR writing to socket");
	write(sockfd, '\0', 1);

	free(frame);
}

static stomp_frame * receive_frame(int sockfd)
{
	char buf[BUFSIZE];

	int bytes_recv = read(sockfd, buf, BUFSIZE);
		
	printf("Got %d bytes: %s\n", bytes_recv, buf);
	if (bytes_recv <= 0)
		return NULL;                                           
	
	scs *s = scs_ncreate(buf, bytes_recv);
	return stomp_frame_parse(s);
}

static stomp_frame *create_connect_frame(char * username, char * passcode)
{
	stomp_frame * frame = stomp_frame_create("CONNECT", "");
	add_frame_header(frame, "login", "my_username");
	add_frame_header(frame, "passcode", "my_passcode");
	return frame;
}

static int select_command_and_send(int sockfd){
	stomp_command c = select_command(sockfd);
	if(c < SEND || c > DISCONNECT){
		printf("Invalid command, will ignore.\n");
		return;
	}
	stomp_frame *frame = NULL;
	switch(c){
	case SEND:
		frame = complete_command_send();
		break;
	case DISCONNECT:
		frame = stomp_frame_create("DISCONNECT", "");
		break;
	default:
		printf("Command %d is still not supported.\n", c);
		break;
	}
	send_frame(sockfd, frame);
	return c != DISCONNECT;
}

void connect_stomp_server(){
	int sockfd = connect_to_server("127.0.0.1", PORT);
	send_frame(sockfd, create_connect_frame("my_name", "my_passcode"));
	
	printf("Waiting for server response...\n");
	stomp_frame *frame = receive_frame(sockfd);
	if(strcmp("CONNECTED", get_verb(frame)) == 0)
		printf("Server confirmed, will talk.\n");
	else
		error_and_exit("Server response is incorrect, will exit.\n");
	int should_stop = 1;
	while(should_stop)
	{
		should_stop = select_command_and_send(sockfd);
		// stomp_frame *frame = receive_frame(sockfd);	
		// printf("Receiving frame from server: %s\n", scs_get_content(stomp_frame_serialize(frame)));	
	}
}