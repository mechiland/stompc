#ifndef STOMP_FRAME_H
#define STOMP_FRAME_H

#ifdef __cplusplus 
extern "C" { 
#endif
        
struct _stomp_frame;                   
typedef struct _stomp_frame stomp_frame;

stomp_frame *stomp_frame_create(char *buf, int size);  
char *stomp_frame_get_verb(stomp_frame *f);
char *stomp_frame_get_body(stomp_frame *f);
void stomp_frame_free(stomp_frame *f);
char *stomp_frame_serialize(stomp_frame *f, int *size);


#ifdef __cplusplus 
} 
#endif

#endif /* end of include guard: STOMP_FRAME_H */
