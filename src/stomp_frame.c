#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "stomp_frame.h"

frame stomp_frame_create(uint8_t *buf, int size)
{
	int i, frame_end = -1;
	for(i = 0; i < size; i++) {
		if(buf[i] == '\0') {
			frame_end = i;
			break;
		}
	}         
	
	frame f = malloc(sizeof(*f));
	if (frame_end < 0) {
		f->size = 0;
		f->content = NULL;
	} else {
		f->size = frame_end + 1;
		f->content = malloc(sizeof(f->size));
		memcpy(f->content, buf, f->size);        		
	}

	return f;
}

void stomp_frame_destroy(frame f) 
{
	free(f->content);
	free(f);
}
