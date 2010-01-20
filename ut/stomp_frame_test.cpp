#include <UnitTest++.h> 
#include "../src/stomp_frame.h"

TEST(should_return_empty_frame_if_buf_is_empty)
{                                             
	uint8_t buf[0];
	frame f = stomp_frame_create(buf, 0);
	
	CHECK_EQUAL(0, f->size);
	CHECK_EQUAL((uint8_t *)0, f->content);
	
	stomp_frame_destroy(f);
}
 
TEST(should_return_empty_frame_if_buf_not_contain_delimiter)
{                                             
	uint8_t buf[3] = {1, 1, 1};
	frame f = stomp_frame_create(buf, 3);
	
	CHECK_EQUAL(0, f->size);
	CHECK_EQUAL((uint8_t *)0, f->content);
	
	stomp_frame_destroy(f);
}

TEST(should_return_frame_with_delimiter_if_buf_contain_delimiter)
{                                             
	uint8_t buf[4] = {1, 2, '\0', 4};
	frame f = stomp_frame_create(buf, 4);
	
	CHECK_EQUAL(3, f->size);
	CHECK_EQUAL(1, f->content[0]);
	CHECK_EQUAL(2, f->content[1]);
	
	stomp_frame_destroy(f);
}