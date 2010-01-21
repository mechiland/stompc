#ifndef STOMP_FRAME_H
#define STOMP_FRAME_H

#include <stdint.h>


#ifdef __cplusplus 
extern "C" { 
#endif

struct _stomp_frame {
	char *verb;
	char *body;
};
typedef struct _stomp_frame *stomp_frame;

stomp_frame stomp_frame_create(uint8_t *buf, int size);
void stomp_frame_destroy(stomp_frame f);
uint8_t *stomp_frame_serialize(stomp_frame f, int *size);


#ifdef __cplusplus 
} 
#endif

#endif /* end of include guard: STOMP_FRAME_H */
