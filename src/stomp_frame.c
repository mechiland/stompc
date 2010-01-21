#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include "stomp_frame.h"

stomp_frame stomp_frame_create(uint8_t *buf, int size)
{           
	char *s = malloc((size + 1) * sizeof(char));
	memcpy(s, buf, size);
	s[size] = 0;
	
	if (strlen(s) == size) {
		/* does not contain frame delimiter */
		return NULL;
	}  
	
	int i, body_start = -1;
	int has_body = 0;
	for (i = 0; i < strlen(s); i++) {
		if (s[i] == '\n') {
			if (i - body_start == 1) {
				has_body = 1;
				body_start = i;
				break;
			}         
			body_start = i;
		}
	}                                  
	
	if (!has_body) {
		return NULL;
	}               
	
	stomp_frame f = malloc(sizeof(*f));
	f->body = malloc((strlen(s) - body_start) * sizeof(char));
	memcpy(f->body, s + body_start + 1, strlen(s) - body_start -1);
	f->body[strlen(s) - body_start] = 0;
	
	for (i = 0; i < strlen(s); i++) {
		if (s[i] == '\n') {
			f->verb = malloc((i + 1) * sizeof(char));
			memcpy(f->verb, s, i);
			f->verb[i] = 0;
			break;
		}
	}
	
	
	return f;
}              

uint8_t *stomp_frame_serialize(stomp_frame f, int *size)
{
	return NULL;
}

void stomp_frame_destroy(stomp_frame f) 
{                                   
	free(f->verb);
	free(f->body);
	free(f);
}
