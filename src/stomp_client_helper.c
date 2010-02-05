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

static void print_frame(stomp_frame *f){
	scs *s = stomp_frame_serialize(f);
	printf("Frame: %s\n", scs_get_content(s));
	// scs_free(s); 
}

stomp_frame * complete_command_send(){
	char *destination = prompt_and_get_string("Input destination(Example:/queue/a):\n");
	char *body = prompt_and_get_string("Input body(Example: hello queue a):\n");
		
	stomp_frame *frame = stomp_frame_create("SEND", body);
	print_frame(frame);
	add_frame_header(frame, "destination", destination);
	print_frame(frame);
	
	free(destination);
	free(body);
	return frame;
}