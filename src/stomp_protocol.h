#ifndef STOMP_PROTOCOL_H
#define STOMP_PROTOCOL_H                         

#include "stomp_frame.h" 
#include "stomp.h"

#ifdef __cplusplus 
extern "C" { 
#endif                       

stomp * add_stomp(int sock, send_handler *send_handler, close_handler *close_handler);
stomp *get_stomp(int client_sock);
void close_stomp(stomp *stp);	

void set_to_connected(stomp *stp);
int is_connected(stomp *stp);
int get_client_sock(stomp *stp);
scs *get_buffer(stomp *stp);

void send_response_frame(stomp *stp, stomp_frame *f);

#ifdef __cplusplus 
} 
#endif

#endif /* end of include guard: STOMP_PROTOCOL_H */
