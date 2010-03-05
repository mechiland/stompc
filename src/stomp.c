#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "stomp.h"
#include "scs.h"
#include "stomp_protocol.h"
#include "stomp_frame.h"

static void send_response_frame_and_free(stomp *stp, stomp_frame *f);
static stomp_frame *create_error_frame(char *message, char *body);
static void stomp_process(stomp *stp, stomp_frame *f);

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
	if (f == NULL) 
	{
		return; 
	}
    
	printf("Get %s frame from client %d\n", f == NULL ? NULL : get_verb(f), get_client_sock(stp));
	stomp_process(stp, f);

	stomp_frame_free(f); 
	scs_clear(get_buffer(stp));
}

static void stomp_process(stomp *stp, stomp_frame *f)
{
	char *verb = stomp_frame_get_verb(f); 
	if(strcmp("DISCONNECT", verb) == 0)
	{
		close_stomp(stp);
		return;
	}
	
	stomp_frame *rf = NULL;   
	if(0 == is_connected(stp))
	{
		if(0 == strcmp("CONNECT", verb))
		{
			rf = stomp_frame_create("CONNECTED", "");
			set_to_connected(stp);
		}
		else
		{
			rf = create_error_frame("not connected", "You are not connected yet, and it's not a CONNECT frame.");
		}
	}
	else 
	{
		if(0 == strcmp("SEND", verb))
		{
			rf = stomp_frame_create("MESSAGE", get_body(f));
		}
		else if(0 == strcmp("SUBSCRIBE", verb))
		{
			char *dest_name = get_header(f, "destination");
			subscribe_to_destination(get_client_sock(stp), dest_name);
		}
		else if(0 == strcmp("UNSUBSCRIBE", verb))
		{
			char *dest_name = get_header(f, "destination");
			unsubscribe_to_destination(get_client_sock(stp), dest_name);
		}
		else
		{				
			char *body = malloc(strlen("Header  is not invalid.") + strlen(verb) + 1);
			sprintf(body, "Header %s is not invalid.", verb);
			rf = create_error_frame("invalid header", body);
			free(body);
		}
	}

	send_response_frame_and_free(stp, rf);
}

static void send_response_frame_and_free(stomp *stp, stomp_frame *f)
{
	if(f == NULL)
		return;
	send_response_frame(stp, f);    
    stomp_frame_free(f);
}

static stomp_frame *create_error_frame(char *message, char *body)
{
	stomp_frame *f = stomp_frame_create("ERROR", body);
	add_frame_header(f, "message", message);
	return f;
}
