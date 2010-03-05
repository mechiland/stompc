#ifndef STOMP_PROTOCOL_H
#define STOMP_PROTOCOL_H                         

#include "stomp_frame.h" 
#include "stomp.h"

#ifdef __cplusplus 
extern "C" { 
#endif                       

struct _subscriber;
typedef struct _subscriber subscriber;

struct _destination;
typedef struct _destination destination;

stomp * add_stomp(int sock, send_handler *send_handler, close_handler *close_handler);
stomp *get_stomp(int sock);
void close_stomp(stomp *stp);	

void set_to_connected(stomp *stp);
int is_connected(stomp *stp);
int get_client_sock(stomp *stp);
scs *get_buffer(stomp *stp);

destination *subscribe_to_destination(int sock, char *dest_name);
int is_subscribed_to_destination(int sock, destination *dest);

void send_response_frame(stomp *stp, stomp_frame *f);

#ifdef __cplusplus 
} 
#endif

#endif /* end of include guard: STOMP_PROTOCOL_H */
