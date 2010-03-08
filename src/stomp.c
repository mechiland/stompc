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

static stomp_frame *handle_subscribe_frame(stomp *stp, stomp_frame *f);
static stomp_frame *handle_unsubscribe_frame(stomp *stp, stomp_frame *f);
static stomp_frame *handle_connect_frame(stomp *stp, stomp_frame *f);
static stomp_frame *handle_send_frame(stomp *stp, stomp_frame *f);

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
		rf = handle_connect_frame(stp, f);
	}
	else 
	{
		if(0 == strcmp("SUBSCRIBE", verb))
		{
			rf = handle_subscribe_frame(stp, f);
		}
		else if(0 == strcmp("UNSUBSCRIBE", verb))
		{
			rf = handle_unsubscribe_frame(stp, f);
		}
		else if(0 == strcmp("SEND", verb))
		{
			rf = handle_send_frame(stp, f);
		}
		else
		{
			char *body = malloc(strlen("Header  is not invalid.") + strlen(verb) + 1);
			sprintf(body, "Header %s is not invalid.", verb);
			rf = create_error_frame("invalid verb", body);
			free(body);
		}
	}

	send_response_frame_and_free(stp, rf);
}

static void send_response_frame_and_free(stomp *stp, stomp_frame *f)
{
	if(f == NULL)
		return;
	send_response_frame_to_stomp(stp, f);    
    stomp_frame_free(f);
}

static stomp_frame *create_error_frame(char *message, char *body)
{
	stomp_frame *f = stomp_frame_create("ERROR", body);
	add_frame_header(f, "message", message);
	return f;
}

static stomp_frame *create_message_frame(char *destination, char *body)
{
	stomp_frame *f = stomp_frame_create("MESSAGE", body);
	add_frame_header(f, "destination", destination);
	return f;
}

static stomp_frame *handle_subscribe_frame(stomp *stp, stomp_frame *f)
{
	stomp_frame *rf = NULL;
	char *dest_name = get_header(f, "destination");
	if(dest_name == NULL)
	{
		rf = create_error_frame("header missed", "Subscribe frame should contain a destination header.");
	}
	else
	{
		subscribe_to_destination(get_client_sock(stp), dest_name);
	}
	return rf;
}

static stomp_frame *handle_unsubscribe_frame(stomp *stp, stomp_frame *f)
{
	stomp_frame *rf = NULL;
	char *dest_name = get_header(f, "destination");
	if(dest_name == NULL)
	{
		rf = create_error_frame("header missed", "Unsubscribe frame should contain a destination header.");
	}
	else
	{
		unsubscribe_to_destination(get_client_sock(stp), dest_name);
	}
	return rf;
}

static stomp_frame *handle_connect_frame(stomp *stp, stomp_frame *f)
{
	stomp_frame *rf = NULL;
	
	if(0 == strcmp("CONNECT", stomp_frame_get_verb(f)))
	{
		rf = stomp_frame_create("CONNECTED", "");
		set_to_connected(stp);
	}
	else
	{
		rf = create_error_frame("not connected", "You are not connected yet, and it's not a CONNECT frame.");
	}
	return rf;
}

static stomp_frame *handle_send_frame(stomp *stp, stomp_frame *f)
{
	stomp_frame *rf = NULL;

	char *dest_name = get_header(f, "destination");
	if(dest_name == NULL)
	{
		rf = create_error_frame("header missed", "Send frame should contain a destination header.");
	}
	else
	{
		stomp_frame *frame = create_message_frame(dest_name, get_body(f));
		send_response_frame_to_destination(dest_name, frame);
	    stomp_frame_free(f);
	}
	return rf;
}