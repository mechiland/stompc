#include <stdlib.h>
#include "stomp.h"
#include "scs.h"

struct _stomp {
	int sock;
	send_handler *send_handler;
	close_handler *close_handler;
	scs *buffer;
};  

stomp_frame *stomp_process(stomp_frame *f)
{       
	stomp_frame *rf = NULL;    
	if(!strcmp(stomp_frame_get_verb(f), "CONNECT"))
	{
		rf = stomp_frame_create("CONNECTED", "");
	} 
	
	return rf;
}       

stomp *stomp_create(int sock, send_handler *send_handler, close_handler *close_handler)
{
	stomp *stp = malloc(sizeof(*stp));
	stp->sock = sock;
	stp->send_handler = send_handler;
	stp->close_handler = close_handler; 
	stp->buffer = scs_create("");
}   

void stomp_receive(stomp *stp, char *buf, int size) 
{   
	scs_nappend(stp->buffer, buf, size);
	stomp_frame *f = stomp_frame_parse(stp->buffer); 
	if (f == NULL) {
		return; 
	} 
    
	stomp_frame *rf = stomp_process(f);

	if (rf) {
		scs *s = stomp_frame_serialize(rf); 
		if (stp->send_handler(stp->sock, scs_get_content(s), scs_get_size(s)) != scs_get_size(s)) {
			perror("Failed to send response frame data");
		}                                                                     
		scs_free(s);
		stomp_frame_free(rf);
	} 

	stomp_frame_free(f); 
	scs_clear(stp->buffer);
}
