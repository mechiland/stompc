#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "stomp.h"
#include "scs.h"
#include "stomp_protocol.h"
#include "stomp_frame.h"

stomp *stomp_create(int sock, send_handler *send_handler, close_handler *close_handler)
{	
	return add_stomp(sock, send_handler, close_handler);
}

void stomp_close(int sock)
{
	stomp *stp = get_stomp(sock);
	close_stomp(stp);	
}

void stomp_receive(int sock, char *buf, int size) 
{
	stomp *stp = get_stomp(sock);
	scs_nappend(get_buffer(stp), buf, size);
	stomp_frame *f = stomp_frame_parse(get_buffer(stp)); 
	printf("Get %s frame from client %d\n", f == NULL ? NULL : get_verb(f), get_client_sock(stp));
	if (f == NULL) 
	{
		return; 
	}
    
	if(strcmp(stomp_frame_get_verb(f), "DISCONNECT") == 0)
	{
		close_stomp(stp);
	}
	else
	{
		stomp_frame *rf = NULL;   
		char *verb = stomp_frame_get_verb(f); 
		if(strcmp(verb, "CONNECT") == 0)
		{
			rf = stomp_frame_create("CONNECTED", "");
		} 
		else if(strcmp(verb, "SEND") == 0)
		{
			rf = stomp_frame_create("MESSAGE", get_body(f));
		}

		if (rf) 
		{
			send_response_frame(stp, rf);    
		    stomp_frame_free(rf);
		}
	}

	stomp_frame_free(f); 
	scs_clear(get_buffer(stp));
}