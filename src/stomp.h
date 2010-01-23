#ifndef STOMP_H
#define STOMP_H

#ifdef __cplusplus 
extern "C" { 
#endif                       

struct _stomp;
typedef	struct _stomp stomp;

typedef int send_handler(int sock, char *buf, int size);                                                
typedef void close_handler(int sock);

stomp *stomp_create(int sock, send_handler *send_handler, close_handler *close_handler);
void stomp_receive(stomp *stp, char *buf, int size);         
void stomp_close_connection(stomp *stp);
      
#ifdef __cplusplus 
} 
#endif

#endif /* end of include guard: STOMP_H */
