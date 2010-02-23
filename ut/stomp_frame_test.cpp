#include <UnitTest++.h> 
#include "../src/stomp_frame.h"    
#include "../src/scs.h"    

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
	
	CHECK_EQUAL("CONNECT", stomp_frame_get_verb(f));
	CHECK_EQUAL("aaa", stomp_frame_get_body(f));      
	CHECK_EQUAL((frame_header *)NULL, get_headers(f));  

	stomp_frame_free(f);   	
}

TEST(should_add_frame_headers_if_buf_contains_valid_header)
{                                                        
	char *data = "CONNECT\nlogin:name\npasscode:12345\n\naaa\0";                           	
	stomp_frame *f = create_frame(data, strlen(data) + 1);
	
	CHECK_EQUAL("CONNECT", stomp_frame_get_verb(f));
	CHECK_EQUAL("aaa", stomp_frame_get_body(f));                
	
	frame_header * header = get_headers(f);
	CHECK((frame_header *)NULL != header);  
	CHECK_EQUAL("login", header->key);  
	CHECK_EQUAL("name", header->value);  
	
	header = header->next;
	CHECK((frame_header *)NULL != header);  
	CHECK_EQUAL("passcode", header->key);  
	CHECK_EQUAL("12345", header->value);  
	
	header = header->next;
	CHECK_EQUAL((frame_header *)NULL, header);
	stomp_frame_free(f);   	
}

void check_null_frame(char *data, int size)
{
	CHECK_EQUAL((stomp_frame *)NULL, create_frame(data, size));                 
}

stomp_frame *create_frame(char *data, int size) 
{
	stomp_frame *f = stomp_frame_parse(scs_ncreate(data, size));
	return f;
}                  

TEST(should_return_serialized_frame)
{
	stomp_frame *f = stomp_frame_create("verb", "body");
	scs *s = stomp_frame_serialize(f);   
		                        
	char *expected_str = "verb\n\nbody\0";
	
	CHECK_EQUAL(expected_str, scs_get_content(s));
	CHECK_EQUAL(strlen(expected_str) + 1, scs_get_size(s));
		
	scs_free(s);
	stomp_frame_free(f);
}

TEST(should_return_serialized_frame_when_body_is_empty)
{
	stomp_frame *f = stomp_frame_create("verb", "");
	scs *s = stomp_frame_serialize(f);   
		                        
	char *expected_str = "verb\n\n\0";
	
	CHECK_EQUAL(expected_str, scs_get_content(s));
	CHECK_EQUAL(strlen(expected_str) + 1, scs_get_size(s));
		
	scs_free(s);
	stomp_frame_free(f);
}

TEST(should_return_serialized_frame_when_headers_are_not_empty)
{
	stomp_frame *f = stomp_frame_create("verb", "body");
	add_frame_header(f, "key1", "value1");
	add_frame_header(f, "key2", "value2");
	scs *s = stomp_frame_serialize(f);   
		                        
	char *expected_str = "verb\nkey1:value1\nkey2:value2\n\nbody\0";
	
	CHECK_EQUAL(expected_str, scs_get_content(s));
	CHECK_EQUAL(strlen(expected_str) + 1, scs_get_size(s));
		
	scs_free(s);
	stomp_frame_free(f);
}

TEST(should_return_serialized_frame_when_header_is_destination)
{
	stomp_frame *f = stomp_frame_create("SEND", "Just another test");
	add_frame_header(f, "destination", "/test");
	scs *s = stomp_frame_serialize(f);   
		                        
	char *expected_str = "SEND\ndestination:/test\n\nJust another test\0";
	
	CHECK_EQUAL(expected_str, scs_get_content(s));
	CHECK_EQUAL(strlen(expected_str) + 1, scs_get_size(s));
		
	scs_free(s);
	stomp_frame_free(f);
}


TEST(should_return_empty_headers_when_create_a_new_frame)
{
	stomp_frame *f = stomp_frame_create("verb", "");
	CHECK_EQUAL((frame_header *)NULL, get_headers(f));
	stomp_frame_free(f);
}

TEST(should_add_one_frame_header)
{
	stomp_frame *f = stomp_frame_create("verb", "");
	add_frame_header(f, "key0", "value0");
	frame_header * header = get_headers(f);
	CHECK((frame_header *)NULL != header);
	
	CHECK_EQUAL("key0", header->key);
	CHECK_EQUAL("value0", header->value);
	CHECK_EQUAL((frame_header *)NULL, header->next);
	stomp_frame_free(f);
}

TEST(should_add_another_frame_header)
{
	stomp_frame *f = stomp_frame_create("verb", "");
	add_frame_header(f, "key1", "value1");
	add_frame_header(f, "key2", "value2");
	
	frame_header * header = get_headers(f);
	CHECK((frame_header *)NULL != header);
	CHECK_EQUAL("key1", header->key);
	CHECK_EQUAL("value1", header->value);
	
	header = header->next;
	CHECK_EQUAL("key2", header->key);
	CHECK_EQUAL("value2", header->value);
	
	CHECK_EQUAL((frame_header *)NULL, header->next);
	stomp_frame_free(f);
}

TEST(should_return_header_value_when_given_header_key)
{
	stomp_frame *f = stomp_frame_create("verb", "");
	add_frame_header(f, "key1", "value1");
	add_frame_header(f, "key2", "value2");
	
	CHECK_EQUAL("value1", get_header(f, "key1"));
	CHECK_EQUAL("value2", get_header(f, "key2"));
	frame_header * header = get_headers(f);
	CHECK_EQUAL((frame_header *)NULL, header->next->next);
	
	stomp_frame_free(f);	
}

TEST(should_return_NULL_when_given_not_existed_header_key)
{
	stomp_frame *f = stomp_frame_create("verb", "");
	
	CHECK((char *)NULL == get_header(f, "key"));
	
	stomp_frame_free(f);
}