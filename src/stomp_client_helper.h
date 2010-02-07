#ifndef STOMP_CLIENT_HELPER_H
#define STOMP_CLIENT_HELPER_H

#include "stomp_frame.h"

typedef enum _stomp_command {COMMAND_START, SEND, SUBSCRIBE, UNSUBSCRIBE, BEGIN, COMMIT, ABORT, ACK, DISCONNECT, COMMAND_END} stomp_command;

stomp_command select_command();
stomp_frame *complete_command_send();
stomp_frame *complete_command_unsubscribe();
stomp_frame *complete_command_subscribe();
stomp_frame *complete_command_begin();
stomp_frame *complete_command_commit();
stomp_frame *complete_command_ack();
stomp_frame *complete_command_abort();

#endif