#include <UnitTest++.h> 
#include "../src/scs.h"    

TEST(should_create_correct_stompc_string)
{                           
	char *cstr = "hello";
	scs *s = scs_create(cstr);
	CHECK(strncmp(cstr, scs_get_content(s), scs_get_size(s)) == 0);
	CHECK_EQUAL(strlen(cstr), scs_get_size(s));	
	
	scs_free(s);
}                       

TEST(should_be_able_to_append_c_string_to_stompc_string)
{
	char *cstr = "hello";
	char *cstr2 = ", world";	
	char *expected_string = (char *)malloc(sizeof(char) * (strlen(cstr) + strlen(cstr2) + 1));
	sprintf(expected_string, "%s%s", cstr, cstr2);
	
	scs *s = scs_create(cstr);
	scs_append(s, cstr2);
	
	CHECK(strncmp(expected_string, scs_get_content(s), scs_get_size(s)) == 0);
	CHECK_EQUAL(strlen(expected_string), scs_get_size(s));	
	                                              
	free(expected_string);
	scs_free(s);	
}                      

TEST(should_be_able_to_append_numbered_cstr)
{
	char *cstr = "hello";
	char *cstr2 = "\0abc";	                                          
	int expected_len = strlen(cstr) + 4;
	char *expected_string = (char *)malloc(sizeof(char) * expected_len);
	memcpy(expected_string, cstr, strlen(cstr));
	memcpy(expected_string + strlen(cstr), cstr2, 4);
	
	scs *s = scs_create(cstr);
	scs_nappend(s, cstr2, 4);
	
	CHECK(strncmp(expected_string, scs_get_content(s), scs_get_size(s)) == 0);
	CHECK_EQUAL(expected_len, scs_get_size(s));	
	                                              
	free(expected_string);
	scs_free(s);	
}                                                             

TEST(should_handle_appending_empty_string)
{
	char *cstr = "hello";
	
	scs *s = scs_create(cstr);
	scs_nappend(s, "", 0);
	scs_nappend(s, "aaa", 0);
	
	CHECK(strncmp(cstr, scs_get_content(s), scs_get_size(s)) == 0);
	CHECK_EQUAL(strlen(cstr), scs_get_size(s));	
	                                              
	scs_free(s);	
	
}