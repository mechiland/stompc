#include <stdlib.h>
#include "stomp.h"
#include "scs.h"
#include "stomp_protocol.h"
#include "stomp_frame.h"

struct _stomp {
	int sock;
	send_handler *send_handler;
	close_handler *close_handler;
	scs *buffer;
};  

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
    
	stomp_frame *rf = stomp_proto_process(stp, f);

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

void stomp_close_connection(stomp *stp)
{
	stp->close_handler(stp->sock);
	scs_free(stp->buffer);
	free(stp);
}