#include <stdlib.h>
#include "stomp_protocol.h"

stomp_frame *stomp_proto_process(stomp_frame *f)
{       
	stomp_frame *rf = NULL;    
	if(!strcmp(stomp_frame_get_verb(f), "CONNECT"))
	{
		rf = stomp_frame_create("CONNECTED", "");
	} 
	
	return rf;
}       
