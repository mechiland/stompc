#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "stomp_client_helper.h"

//This helper is used to collect user input

static char * command_menu = "1[SEND]	2[SUBSCRIBE]	3[UNSUBSCRIBE]	4[BEGIN]	5[COMMIT]	6[ABORT]	7[ACK]	8[DISCONNECT]";

stomp_command select_command()
{
	printf("Please select a command(From 1 to 8):\n%s\n", command_menu);
	fflush(stdout);
	
	int i = 0;
	int r;
	char *s = malloc(sizeof(1024));
	while((r = scanf("%d", &i)) != 1){
		fgets(s, 1024, stdin);
		printf("Please select a command(From 1 to 8):\n%s\n", command_menu);
	}
	getchar();//Remove redundant Enter char.
	free(s);
	return i;
}

static char * prompt_and_get_string(char *msg){
	printf(msg);
	fflush(stdout);
	
	char *s = malloc(1024);
	fgets(s,1024,stdin);
	s[strlen(s) - 1] = '\0';
	return s;
}

stomp_frame * complete_command_send(){
	char *destination = prompt_and_get_string("Input destination(Example:/queue/a):\n");
	char *body = prompt_and_get_string("Input body(Example: hello queue a):\n");
		
	stomp_frame *frame = stomp_frame_create("SEND", body);
	add_frame_header(frame, "destination", destination);
	
	free(destination);
	free(body);
	return frame;
}


stomp_frame * complete_command_unsubscribe(){
	char *destination = prompt_and_get_string("Input destination(Example:/queue/a):\n");
	char *id = prompt_and_get_string("Input id(Optional, ignore it if click enter):\n");
		
	stomp_frame *frame = stomp_frame_create("UNSUBSCRIBE", "");
	add_frame_header(frame, "destination", destination);
	if(strlen(id) != 0){
		add_frame_header(frame, "id", id);
	}
	
	free(destination);
	return frame;
}

static int is_valid_ack(char *ack){
	return strcmp("auto", ack) == 0 || strcmp("client", ack) == 0 || strlen(ack) == 0;
}

stomp_frame * complete_command_subscribe(){
	char *destination = prompt_and_get_string("Input destination(Example:/queue/a):\n");
	char *ack = prompt_and_get_string("Input ack(auto or client, Optional, ignore it if click enter):\n");
	while(!is_valid_ack(ack)){
		ack = prompt_and_get_string("Input ack(auto or client, Optional, ignore it if click enter):\n");
	}
	char *selector = prompt_and_get_string("Input selector(Optional, ignore it if click enter):\n");
	char *id = prompt_and_get_string("Input id(Optional, ignore it if click enter):\n");
		
	stomp_frame *frame = stomp_frame_create("SUBSCRIBE", "");
	add_frame_header(frame, "destination", destination);
	if(strlen(ack) != 0){
		add_frame_header(frame, "ack", ack);
	}
	if(strlen(selector) != 0){
		add_frame_header(frame, "selector", selector);
	}
	if(strlen(id) != 0){
		add_frame_header(frame, "id", id);
	}
	
	free(destination);
	return frame;
}