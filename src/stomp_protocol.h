#ifndef STOMP_PROTOCOL_H
#define STOMP_PROTOCOL_H                         

#include "stomp_frame.h" 
#include "stomp.h"

#ifdef __cplusplus 
extern "C" { 
#endif                       

stomp_frame *stomp_proto_process(stomp *stp, stomp_frame *f);

#ifdef __cplusplus 
} 
#endif

#endif /* end of include guard: STOMP_PROTOCOL_H */
