#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include "stomp_frame.h"

struct _stomp_frame {
	char *verb;
	frame_header *headers;
	char *body;
};

static int is_valid_frame(int frame_end, int header_end, int verb_end);

stomp_frame *stomp_frame_create(const char *verb, const char *body)
{              
	stomp_frame *f = malloc(sizeof(*f));                
	f->verb = malloc((strlen(verb) + 1) * sizeof(char));
	strcpy(f->verb, verb);
	f->headers = NULL;
	f->body = malloc((strlen(body) + 1) * sizeof(char));
	strcpy(f->body, body);
	
	return f;
}	

stomp_frame *stomp_frame_parse(scs *s)
{             
	char *buf = scs_get_content(s);
	int size = scs_get_size(s);
	
	int frame_end = -1;   
	int header_end = -1;
	int verb_end = -1; 
	int line_end = -1;
	int line_count = 0;
	int i = 0;
	char next_char;
	while (i < size) {
		next_char = buf[i];
		if (next_char == '\0') {
			frame_end = i;
			break;
		}
		if (next_char == '\n') {   
			if (line_end >= 0 && i - line_end == 1) {
				header_end = i;
			}
			line_end = i;
			line_count++;
		}                
		if (line_count == 1) {
			verb_end = line_end;
		}
		i++;
	} 

	if (!is_valid_frame(frame_end, header_end, verb_end)) {
		return NULL;
	}
	     
	stomp_frame *f = malloc(sizeof(*f));
	f->verb = malloc((verb_end + 1) * sizeof(char));
	memcpy(f->verb, buf, verb_end);
	f->verb[verb_end] = 0;
	                              
	int body_start = (header_end + 1);
	int body_len = frame_end - body_start;
	f->body = malloc((body_len + 1) * sizeof(char));
	memcpy(f->body, buf + body_start, body_len);
	f->body[body_len] = 0;
	
	return f;
}         

static int is_valid_frame(int frame_end, int header_end, int verb_end) {
	return frame_end >= 0 && header_end >= 0 && verb_end >= 0;
}   

char *stomp_frame_get_verb(stomp_frame *f)
{
	return f->verb;
}   

char *stomp_frame_get_body(stomp_frame *f) 
{
	return f->body;
}

scs *stomp_frame_serialize(stomp_frame *f)
{               
	scs *s = scs_create(f->verb);
	scs_append(s, "\n\n");
	scs_append(s, f->body);
	scs_nappend(s, "\0", 1);
	return s;
}

void stomp_frame_free(stomp_frame *f) 
{                                   
	free(f->verb);
	free(f->body);
	free(f);
}

char *get_verb(stomp_frame *f)
{
	return f->verb;
}

char *get_body(stomp_frame *f)
{
	return f->body;
}

frame_header * get_headers(stomp_frame *f)
{
	return f->headers;
}

void add_frame_header(stomp_frame *frame, char *key, char *value)
{
	frame_header *last = malloc(sizeof(*last));
	last->key = key;
	last->value = value;

	frame_header *prev, * next;
	prev = next = frame->headers;
	while(next != NULL){
		prev = next;
		next = prev->next;
	}
	if(prev == NULL){
		frame->headers = prev = last;
	}
	else{
		prev->next = last;
		prev = prev->next;
	}
	prev->next = NULL;
}
