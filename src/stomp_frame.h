#ifndef STOMP_FRAME_H
#define STOMP_FRAME_H

#include <stdint.h>


#ifdef __cplusplus 
extern "C" { 
#endif

struct _frame {
	uint8_t *content;
	int size;
};
typedef struct _frame *frame;

frame stomp_frame_create(uint8_t *buf, int size);
void stomp_frame_destroy(frame f);


#ifdef __cplusplus 
} 
#endif

#endif /* end of include guard: STOMP_FRAME_H */
