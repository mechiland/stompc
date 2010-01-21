#include <UnitTest++.h> 
#include "../src/stomp_frame.h"    

stomp_frame *create_frame(char *data, int size);
void check_null_frame(char *data, int size);

TEST(should_return_null_if_buf_is_empty)
{                                             
	char *data = "";
	check_null_frame(data, strlen(data));                       
}

TEST(should_return_null_if_buf_not_contain_stomp_frame)
{                    
	char *data = "not stomp frame";
	check_null_frame(data, strlen(data));                       
}                   

TEST(should_return_null_if_buf_is_not_valid_stomp_frame)
{
	char *data = "invalid stomp frame\n\0abc";
	check_null_frame(data, strlen(data) + 4);                       
	
	data = "\0";
	check_null_frame(data, strlen(data) + 1);                       

	data = "a\nb\n\0";
	check_null_frame(data, strlen(data) + 1);                       
	
}

TEST(should_return_frame_if_buf_contain_valid_stomp_frame)
{                                                        
	char *data = "CONNECT\n\naaa\0";                           	
	stomp_frame *f = create_frame(data, strlen(data) + 1);
	
	CHECK_EQUAL("CONNECT", f->verb);
	CHECK_EQUAL("aaa", f->body);                

	stomp_frame_free(f);   	
}

void check_null_frame(char *data, int size)
{
	CHECK_EQUAL((stomp_frame *)NULL, create_frame(data, size));                 
}

stomp_frame *create_frame(char *data, int size) 
{
	uint8_t *b = (uint8_t *)malloc(size * sizeof(*b));
	memcpy(b, data, size);

	stomp_frame *f = stomp_frame_create(b, size);
	free(b);	                                

	return f;
}                  

