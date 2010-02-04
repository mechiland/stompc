#include <stdlib.h>
#include "stomp_protocol.h"

stomp_frame *stomp_proto_process(stomp *stp, stomp_frame *f)
{       
	stomp_frame *rf = NULL;    
	if(strcmp(stomp_frame_get_verb(f), "CONNECT") == 0)
	{
		rf = stomp_frame_create("CONNECTED", "");
	} 

	if(strcmp(stomp_frame_get_verb(f), "DISCONNECT") == 0)
	{
		stomp_close_connection(stp);
	} 
	
	return rf;
}       
