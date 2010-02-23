#include <UnitTest++.h> 
#include "../src/stomp.h"

static int data_sent = 0;
static char *sent_data;
static int sent_data_size;                     
static int sock;

int stub_send_handler(int actual_sock, char *actual_buf, int actual_size)
{
	data_sent = 1;
	CHECK_EQUAL(sock, actual_sock);
	CHECK_EQUAL(sent_data_size, actual_size);
	CHECK(strncmp(sent_data, actual_buf, sent_data_size) == 0);    
	return actual_size;
}   
                                             
TEST(should_not_send_frame_if_received_data_is_not_valid_frame)
{                                                         
	data_sent = 0;
	sock = 5;
	stomp *stp = stomp_create(sock, stub_send_handler, NULL);
	                   
	char *recv_data = "invalid frame";
	stomp_receive(stp, recv_data, strlen(recv_data));
	
	CHECK_EQUAL(0, data_sent);
}

TEST(should_send_response_frame_if_received_recoganizable_frame_data)
{
	data_sent = 0;
	sock = 5;                         
	sent_data = "CONNECTED\n\n\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "CONNECT\n\n\0";
	stomp *stp = stomp_create(sock, stub_send_handler, NULL);

	stomp_receive(stp, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(1, data_sent); 
}                             

TEST(should_not_send_response_frame_if_received_unrecoganizable_frame_data)
{
	data_sent = 0;
	sock = 5;                         
	
	char *recv_data = "NOT_SUPPOTED_VERB\n\n\0";
	stomp *stp = stomp_create(sock, stub_send_handler, NULL);

	stomp_receive(stp, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(0, data_sent); 
	
} 

TEST(should_store_received_data_if_the_data_is_not_valid_frame)
{
	data_sent = 0;
	sock = 5;                         
	sent_data = "CONNECTED\n\n\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "CONNECT";
	stomp *stp = stomp_create(sock, stub_send_handler, NULL);

	stomp_receive(stp, recv_data, strlen(recv_data));	
	CHECK_EQUAL(0, data_sent); 	                     
	
	stomp_receive(stp, "\n\n\0", 3);	
	CHECK_EQUAL(1, data_sent); 	                     	
}    

TEST(should_clear_buffered_recv_data_once_it_can_be_recoganized_as_frame)
{
	data_sent = 0;
	sock = 5;                         
	sent_data = "CONNECTED\n\n\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "CONNECT";
	stomp *stp = stomp_create(sock, stub_send_handler, NULL);

	stomp_receive(stp, recv_data, strlen(recv_data));	
	CHECK_EQUAL(0, data_sent); 	                     
	
	stomp_receive(stp, "\n\n\0", 3);	
	CHECK_EQUAL(1, data_sent); 	                     	
	
	data_sent = 0;
	stomp_receive(stp, "more data", 9);	
	CHECK_EQUAL(0, data_sent); 	                     		
}