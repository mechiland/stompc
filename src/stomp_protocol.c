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

struct _subscriber{
	int sock;
	struct _subscriber *next;
};

struct _destination{
	char *name;
	subscriber *subscriber_list;
	struct _destination *next;
};

static destination *destination_list;

static stomp *stomp_list;

static void add_stomp_to_list(stomp *last);
static void remove_stomp_from_list(int client_sock);

static destination *get_destination_from_list(char *dest_name);
static void add_destination_to_list(destination *dest);
static void remove_destination_from_list(int sock, destination *dest);

static void add_subscriber_to_list(int sock, destination *dest);
static stomp *get_stomp_of_subscriber(subscriber *sub);
static subscriber *get_subscribers_of_destination(char *dest_name);

stomp *add_stomp(int sock, send_handler *send_handler, close_handler *close_handler)
{	
	stomp *last = malloc(sizeof(*last));
	last->sock = sock;
	last->send_handler = send_handler;
	last->close_handler = close_handler; 
	last->buffer = scs_create("");
	last->connected = 0;
	
	add_stomp_to_list(last);
	
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
		
	remove_stomp_from_list(stp->sock);
		
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
	stomp *machine = stomp_list;
	while(machine != NULL){
		if(machine->sock == client_sock){
			return machine;
		}
		machine = machine->next;
	}
	return NULL;
}

static void add_stomp_to_list(stomp *last)
{ 	
	stomp *prev, *next;
	prev = next = stomp_list;
	while(NULL != next){
		prev = next;
		next = prev->next;
	}
	
	if(prev == NULL){
		stomp_list = last;
		stomp_list->next = NULL;
	}
	else{
		prev->next = last;
		last->next = NULL;	
	}
}

static void remove_stomp_from_list(int client_sock)
{
	stomp *prev, *next;
	prev = next = stomp_list;
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
		stomp_list = NULL;
	}
	else
	{
		prev->next = next->next;
	}
}

void send_response_frame_to_stomp(stomp *stp, stomp_frame *f)
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

void send_response_frame_to_destination(char *dest_name, stomp_frame *f)
{
	scs *s = stomp_frame_serialize(f);
	subscriber *sub = get_subscribers_of_destination(dest_name);
	printf("111\n");
	while(sub != NULL)
	{
		printf("222\n");
		stomp *stp = get_stomp_of_subscriber(sub);
		printf("333 to %d\n", stp->sock);
		send_response_frame_to_stomp(stp, f);
		printf("444\n");
		sub = sub->next;
	}
}

void set_to_connected(stomp *stp)
{
	stp->connected = 1;
}

int is_connected(stomp *stp)
{
	return stp->connected;
}

destination *subscribe_to_destination(int sock, char *dest_name)
{
	if(dest_name == NULL)
		return NULL;
	destination *dest = get_destination_from_list(dest_name);
	if(dest == NULL)
	{
		dest = malloc(sizeof(*dest));
		dest->name = malloc(strlen(dest_name) + 1);
		memcpy(dest->name, dest_name, strlen(dest_name));
		dest->subscriber_list = NULL;
		dest->next = NULL;
		add_destination_to_list(dest);
	}
	if(!is_subscribed_to_destination(sock, dest))
	{
		add_subscriber_to_list(sock, dest);	
	}
	return dest;
}

void unsubscribe_to_destination(int sock, char *dest_name)
{
	if(dest_name == NULL)
		return;
	destination *dest = get_destination_from_list(dest_name);
	if(dest != NULL)
	{
		remove_destination_from_list(sock, dest);
	}
}

int is_subscribed_to_destination(int sock, destination *dest)
{
	subscriber *sub = dest->subscriber_list;
	while(sub != NULL)
	{
		if(sock == sub->sock)
		{
			return 1;
		}
		sub = sub->next;
	}
	return 0;
}

static subscriber *get_subscribers_of_destination(char *dest_name)
{
	destination *dest = destination_list;
	while(dest != NULL)
	{
		if(0 == strcmp(dest_name, dest->name))
		{
			return dest->subscriber_list;
		}
		dest = dest->next;
	}
	return NULL;
}

static stomp *get_stomp_of_subscriber(subscriber *sub)
{
	stomp *stp = stomp_list;
	while(stp != NULL && stp->sock != sub->sock)
	{
		stp = stp->next;
	}
	
	return stp;
}

static destination *get_destination_from_list(char *dest_name)
{
	destination *dest = destination_list;
	while(dest != NULL)
	{
		if(0 == strcmp(dest_name, dest->name))
		{
			return dest;
		}
		dest = dest->next;
	}
	return NULL;
}

static void add_destination_to_list(destination *last)
{
	destination *prev, *next;
	prev = next = destination_list;
	while(NULL != next){
		prev = next;
		next = prev->next;
	}
	
	if(prev == NULL){
		destination_list = last;
		destination_list->next = NULL;
	}
	else{
		prev->next = last;
		last->next = NULL;
	}
}

static void add_subscriber_to_list(int sock, destination *dest)
{
	subscriber *last = malloc(sizeof(*last));
	last->sock = sock;
	last->next = NULL;
	
	subscriber *prev, *next;
	prev = next = dest->subscriber_list;
	while(NULL != next){
		prev = next;
		next = prev->next;
	}
	
	if(prev == NULL){
		dest->subscriber_list = last;
		dest->subscriber_list->next = NULL;
	}
	else{
		prev->next = last;
		last->next = NULL;
	}
}

static void remove_destination_from_list(int sock, destination *dest)
{
	subscriber *prev, *next;
	prev = next = dest->subscriber_list;
	while(NULL != next && (next->sock != sock)){
		prev = next;
		next = prev->next;
	}
	
	if(next == NULL)
	{
		return;
	}	
	if(prev == next)
	{
		dest->subscriber_list = NULL;
	}
	else
	{
		prev->next = next->next;
	}	
}
