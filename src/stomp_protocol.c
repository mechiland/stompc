#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "stomp_protocol.h"

stomp_frame *stomp_proto_process(stomp *stp, stomp_frame *f)
{       
	stomp_frame *rf = NULL;   
	char *verb = stomp_frame_get_verb(f); 
	if(strcmp(verb, "CONNECT") == 0)
	{
		rf = stomp_frame_create("CONNECTED", "");
	} 
	else if(strcmp(verb, "SEND") == 0)
	{
		rf = stomp_frame_create("MESSAGE", get_body(f));
	}
	else if(strcmp(stomp_frame_get_verb(f), "DISCONNECT") == 0)
	{
		stomp_close_connection(stp);
	}
	
	return rf;
}
