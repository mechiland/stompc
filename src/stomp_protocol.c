#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "stomp_protocol.h"

struct _stomp {
	int sock;
	send_handler *send_handler;
	close_handler *close_handler;
	scs *buffer;
	int connected;
	stomp *next;
};

static stomp *stomp_machines;

static void add_stomp_to_machines(stomp *last);
static void remove_stomp_from_machines(int client_sock);

stomp *add_stomp(int sock, send_handler *send_handler, close_handler *close_handler)
{	
	stomp *last = malloc(sizeof(*last));
	last->sock = sock;
	last->send_handler = send_handler;
	last->close_handler = close_handler; 
	last->buffer = scs_create("");
	last->connected = 0;
	
	add_stomp_to_machines(last);
	
	return last;
}

void close_stomp(stomp *stp)
{
	if(stp == NULL)
		return;
	
	if(stp->close_handler != NULL)
	{
		stp->close_handler(stp->sock);	
	}
	scs_free(stp->buffer);
		
	remove_stomp_from_machines(stp->sock);
		
	free(stp);
}

int get_client_sock(stomp *stp)
{
	return stp->sock;
}

scs *get_buffer(stomp *stp)
{
	return stp->buffer;
}

stomp *get_stomp(int client_sock)
{
	stomp *machine = stomp_machines;
	while(machine != NULL){
		if(machine->sock == client_sock){
			return machine;
		}
		machine = machine->next;
	}
	return NULL;
}

static void add_stomp_to_machines(stomp *last)
{ 	
	stomp *prev, *next;
	prev = next = stomp_machines;
	while(NULL != next){
		prev = next;
		next = prev->next;
	}
	
	if(prev == NULL){
		stomp_machines = last;
		stomp_machines->next = NULL;
	}
	else{
		prev->next = last;
		last->next = NULL;	
	}
}

static void remove_stomp_from_machines(int client_sock)
{
	stomp *prev, *next;
	prev = next = stomp_machines;
	while(NULL != next && (next->sock != client_sock)){
		prev = next;
		next = prev->next;
	}
	
	if(next == NULL)
	{
		return;
	}	
	if(prev == next)
	{
		stomp_machines = NULL;
	}
	else
	{
		prev->next = next->next;
	}
}

void send_response_frame(stomp *stp, stomp_frame *f)
{
	scs *s = stomp_frame_serialize(f);
	int result = stp->send_handler(stp->sock, scs_get_content(s), scs_get_size(s));
    scs_free(s);
	if (result)
	{
    	perror("Failed to send response frame data");
    }
	printf("Send %s response to client %d\n",get_verb(f), stp->sock);
}

void set_to_connected(stomp *stp)
{
	stp->connected = 1;
}

int is_connected(stomp *stp)
{
	return stp->connected;
}
