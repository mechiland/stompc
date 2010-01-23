#include <UnitTest++.h> 
#include "../src/stomp_protocol.h"

TEST(should_return_null_if_frame_can_not_be_recoganized)
{                                                      
	stomp_frame *f = stomp_frame_create("invalid verb", "body");
	stomp_frame *rf = stomp_proto_process(NULL, f);
	CHECK_EQUAL((stomp_frame *)NULL, rf);
	stomp_frame_free(f);
}

TEST(should_return_connected_frame_if_given_connect_frame)
{
	stomp_frame *f = stomp_frame_create("CONNECT", "body");
	stomp_frame *rf = stomp_proto_process(NULL, f);
	CHECK_EQUAL("CONNECTED", stomp_frame_get_verb(rf));
	CHECK_EQUAL("", stomp_frame_get_body(rf));
	stomp_frame_free(f);	
	stomp_frame_free(rf);	
}                                                          

static int sock;
static int closed = 0;
static void stub_close_handler(int actual_sock)
{
	CHECK_EQUAL(sock, actual_sock);      
	closed = 1;
}               

TEST(should_close_connection_if_given_disconnect_frame)
{                                 
	closed = 0;
	sock = 1;
	
	stomp *stp = stomp_create(sock, NULL, stub_close_handler);
	
	stomp_frame *f = stomp_frame_create("DISCONNECT", "");
	CHECK_EQUAL((stomp_frame *)NULL, stomp_proto_process(stp, f));
	CHECK_EQUAL(1, closed);
	
	stomp_frame_free(f);	
}

