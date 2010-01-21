#include <UnitTest++.h> 
#include "../src/stomp_frame.h"

TEST(should_return_null_if_buf_is_empty)
{                                             
	uint8_t buf[0];
	stomp_frame f = stomp_frame_create(buf, 0);

	CHECK_EQUAL((stomp_frame)NULL, f);
}

TEST(should_return_null_if_buf_not_contain_stomp_frame)
{                           
	char *buf = "not stomp frame";        
	int len = strlen(buf);
	uint8_t *b = (uint8_t *)malloc(len * sizeof(*b));
	memcpy(b, buf, len);

	stomp_frame f = stomp_frame_create(b, len);

	CHECK_EQUAL((stomp_frame)NULL, f);                 
	free(b);
}                                     

TEST(should_return_null_if_buf_is_not_valid_stomp_frame)
{
	char *buf = "invalid stomp frame\n\0abc";
	int len = strlen(buf) + 4;
	uint8_t *b = (uint8_t *)malloc(len * sizeof(*b));
	memcpy(b, buf, len);

	stomp_frame f = stomp_frame_create(b, len);

	CHECK_EQUAL((stomp_frame)NULL, f);	     

	free(b);
}

TEST(should_return_frame_if_buf_contain_valid_stomp_frame)
{                                                        
	char *buf = "CONNECT\n\n\0";                           	
	int len = strlen(buf) + 1;
	uint8_t *b = (uint8_t *)malloc(len * sizeof(*b));
	memcpy(b, buf, len);

	stomp_frame f = stomp_frame_create(b, len);

	CHECK_EQUAL("CONNECT", f->verb);
	CHECK_EQUAL("", f->body);                

	stomp_frame_destroy(f);   	
	free(b);
}