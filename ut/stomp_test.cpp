#include <UnitTest++.h> 
#include "../src/stomp.h"

static int data_sent = 0;
static char *sent_data;
static int sent_data_size;                     
static int sock;
static int closed = 0;

static int stub_send_handler(int actual_sock, char *actual_buf, int actual_size)
{
	data_sent = 1;
	CHECK_EQUAL(sock, actual_sock);
	CHECK_EQUAL(sent_data_size, actual_size);
	CHECK(strncmp(sent_data, actual_buf, sent_data_size) == 0);    
	return actual_size;
}   

static void stub_close_handler(int actual_sock)
{
	CHECK_EQUAL(sock, actual_sock);      
	closed = 1;
}
	
static void send_connect_frame(int sock)
{
	data_sent = 0;
	sent_data = "CONNECTED\n\n\0";
	sent_data_size = strlen(sent_data) + 1;

	stomp_create(sock, stub_send_handler, NULL);

	char *recv_data = "CONNECT\n\n\0";
	stomp_receive(sock, recv_data, strlen(recv_data) + 1);

	CHECK_EQUAL(1, data_sent);
}
	
TEST(should_not_send_frame_if_received_data_is_not_valid_frame)
{                                                         
	data_sent = 0;
	sock = 5;
	stomp_create(sock, stub_send_handler, NULL);
	char *recv_data = "invalid frame";
	stomp_receive(sock, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(0, data_sent);
	stomp_close(sock);
}

TEST(should_send_response_frame_if_received_recoganizable_frame_data)
{
	data_sent = 0;
	sock = 6;                         
	sent_data = "CONNECTED\n\n\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "CONNECT\n\n\0";
	stomp_create(sock, stub_send_handler, NULL);

	stomp_receive(sock, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(1, data_sent); 
	stomp_close(sock);
}                             

TEST(should_store_received_data_if_the_data_is_not_valid_frame)
{
	data_sent = 0;
	sock = 8;                         
	sent_data = "CONNECTED\n\n\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "CONNECT";
	stomp_create(sock, stub_send_handler, NULL);

	stomp_receive(sock, recv_data, strlen(recv_data));	
	CHECK_EQUAL(0, data_sent); 	                     
	
	stomp_receive(sock, "\n\n\0", 3);	
	CHECK_EQUAL(1, data_sent); 	                     	
	stomp_close(sock);
}    

TEST(should_clear_buffered_recv_data_once_it_can_be_recoganized_as_frame)
{
	data_sent = 0;
	sock = 9;                         
	sent_data = "CONNECTED\n\n\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "CONNECT";
	stomp_create(sock, stub_send_handler, NULL);

	stomp_receive(sock, recv_data, strlen(recv_data));	
	CHECK_EQUAL(0, data_sent); 	                     
	
	stomp_receive(sock, "\n\n\0", 3);	
	CHECK_EQUAL(1, data_sent); 	                     	
	
	data_sent = 0;
	stomp_receive(sock, "more data", 9);	
	CHECK_EQUAL(0, data_sent); 	                     		
	stomp_close(sock);
}

TEST(should_close_connection_when_given_disconnect_frame)
{                                 
	closed = 0;
	sock = 10;
	
	stomp_create(sock, NULL, stub_close_handler);
	
	char *recv_data = "DISCONNECT\n\n\0";
	stomp_receive(sock, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(1, closed);
	stomp_close(sock);
}

TEST(should_send_ERROR_frame_if_received_invalid_verb)
{
	sock = 11;
	send_connect_frame(sock);
	
	data_sent = 0;
	sent_data = "ERROR\nmessage:invalid verb\n\nHeader NOT_SUPPORTED_VERB is not invalid.\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "NOT_SUPPORTED_VERB\n\n\0";
	stomp_receive(sock, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(1, data_sent);
	
	stomp_close(sock);
}

TEST(should_send_ERROR_frame_if_receive_non_CONNECT_frame_and_stomp_is_not_connected)
{
	data_sent = 0;
	sock = 12;
	sent_data = "ERROR\nmessage:not connected\n\nYou are not connected yet, and it's not a CONNECT frame.\0";
	sent_data_size = strlen(sent_data) + 1;
	
	stomp_create(sock, stub_send_handler, NULL);

	char *recv_data = "SEND\ndestination:/queue/a\n\nhello queue a\n\0";
	stomp_receive(sock, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(1, data_sent); 

	stomp_close(sock);
}

TEST(should_be_able_to_subscribe_to_existed_queue)
{
	sock = 13;
	send_connect_frame(sock);

	data_sent = 0;
	
	char *recv_data = "SUBSCRIBE\ndestination:/queue/a\n\nhello queue a\n\0";
	stomp_receive(sock, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(0, data_sent); 

	stomp_close(sock);
}

TEST(should_send_ERROR_frame_if_subscribe_frame_has_no_destination_header)
{
	sock = 14;
	send_connect_frame(sock);
	
	data_sent = 0;
	sent_data = "ERROR\nmessage:header missed\n\nSubscribe frame should contain a destination header.\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "SUBSCRIBE\n\n\0";
	stomp_receive(sock, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(1, data_sent);
	
	stomp_close(sock);
}

TEST(should_send_ERROR_frame_if_unsubscribe_frame_has_no_destination_header)
{
	sock = 15;
	send_connect_frame(sock);
	
	data_sent = 0;
	sent_data = "ERROR\nmessage:header missed\n\nUnsubscribe frame should contain a destination header.\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "UNSUBSCRIBE\n\n\0";
	stomp_receive(sock, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(1, data_sent);
	
	stomp_close(sock);
}

TEST(should_send_ERROR_frame_if_send_frame_has_no_destination_header)
{
	sock = 16;
	send_connect_frame(sock);
	
	data_sent = 0;
	sent_data = "ERROR\nmessage:header missed\n\nSend frame should contain a destination header.\0";
	sent_data_size = strlen(sent_data) + 1;
	
	char *recv_data = "SEND\n\n\0";
	stomp_receive(sock, recv_data, strlen(recv_data) + 1);
	
	CHECK_EQUAL(1, data_sent);
	
	stomp_close(sock);
}
