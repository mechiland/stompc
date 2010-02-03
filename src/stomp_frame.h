#ifndef STOMP_FRAME_H
#define STOMP_FRAME_H
   
#include "scs.h"

#ifdef __cplusplus 
extern "C" { 
#endif
        
typedef struct _frame_header{
	char * key;
	char * value;
	struct _frame_header *next;
}frame_header;

struct _stomp_frame;                   
typedef struct _stomp_frame stomp_frame;
                                                      
stomp_frame *stomp_frame_create(const char *verb, const char *body);
void add_frame_header(stomp_frame *frame, char *key, char *value);

stomp_frame *stomp_frame_parse(scs *s);  
char *stomp_frame_get_verb(stomp_frame *f);
char *stomp_frame_get_body(stomp_frame *f);
void stomp_frame_free(stomp_frame *f);
scs *stomp_frame_serialize(stomp_frame *f);
                                            
char *get_verb(stomp_frame *f);
char *get_body(stomp_frame *f);
frame_header * get_headers(stomp_frame *f);

#ifdef __cplusplus 
} 
#endif

#endif /* end of include guard: STOMP_FRAME_H */
