#include <UnitTest++.h> 
#include "../src/stomp_protocol.h"

TEST(should_return_null_if_frame_can_not_be_recoganized)
{                                                      
	stomp_frame *f = stomp_frame_create("invalid verb", "body");
	stomp_frame *rf = stomp_process(f);
	CHECK_EQUAL((stomp_frame *)NULL, rf);
	stomp_frame_free(f);
}

TEST(should_return_connected_frame_if_given_connect_frame)
{
	stomp_frame *f = stomp_frame_create("CONNECT", "body");
	stomp_frame *rf = stomp_process(f);
	CHECK_EQUAL("CONNECTED", stomp_frame_get_verb(rf));
	CHECK_EQUAL("", stomp_frame_get_body(rf));
	stomp_frame_free(f);	
	stomp_frame_free(rf);	
}