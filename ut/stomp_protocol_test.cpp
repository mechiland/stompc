#include <UnitTest++.h> 
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "../src/stomp_protocol.h"

static int sock;
static int closed = 0;
static int data_sent = 0;

static int stub_send_handler(int actual_sock, char *actual_buf, int actual_size)
{
	data_sent = 1;
	CHECK_EQUAL(sock, actual_sock);
	return actual_size;
}   

static void stub_close_handler(int actual_sock)
{
	closed = 1;
	CHECK_EQUAL(sock, actual_sock);      
}

TEST(should_return_client_sock)
{
	int sock = 101;
	stomp *stp = add_stomp(sock, NULL, NULL);
	
	CHECK_EQUAL(sock, get_client_sock(stp));
	close_stomp(stp);
}

TEST(should_return_buffer)
{
	stomp *stp = add_stomp(sock, NULL, NULL);
	scs *s = get_buffer(stp);
	CHECK_EQUAL(strlen(""), scs_get_size(s));
	CHECK_EQUAL(0, strncmp("", scs_get_content(s), scs_get_size(s)));
	close_stomp(stp);
}

TEST(should_return_NULL_stomp_when_given_not_existed_client_sock)
{
	CHECK_EQUAL((stomp *)NULL, get_stomp(-1));
}

TEST(should_return_stomp_when_given_client_sock)
{
	int sock = 102;
	stomp *stp = add_stomp(sock, NULL, NULL);
	
	stomp *result = get_stomp(sock);
	CHECK((stomp *)NULL != result);
	CHECK_EQUAL(sock, get_client_sock(result));
	close_stomp(stp);
}

TEST(should_create_and_remove_stomp)
{
	sock = 103;
	stomp *stp = add_stomp(sock, NULL, stub_close_handler);
	
	stomp *result = get_stomp(sock);
	CHECK((stomp *)NULL != result);
	CHECK_EQUAL(sock, get_client_sock(result));
	
	close_stomp(stp);
	CHECK_EQUAL((stomp *)NULL, get_stomp(sock));
}

TEST(should_not_throw_exception_when_remove_not_existed_stomp)
{
	close_stomp((stomp *)NULL);
}

TEST(should_not_throw_exception_when_given_NULL_close_handler)
{
	sock = 104;
	stomp *stp = add_stomp(sock, NULL, NULL);
	close_stomp(stp);
}

TEST(should_send_response_frame_to_client)
{
	sock = 105;
	data_sent = 0;
	
	stomp *stp = add_stomp(sock, stub_send_handler, NULL);
	stomp_frame *f = stomp_frame_create("CONNECTED", "");
	
	send_response_frame(stp, f);
	CHECK_EQUAL(1, data_sent);
	
	close_stomp(stp);
	stomp_frame_free(f);
}

TEST(should_return_stomp_connected_status)
{	
	stomp *stp = add_stomp(sock, stub_send_handler, NULL);
	CHECK_EQUAL(0, is_connected(stp));
	
	close_stomp(stp);
}

TEST(should_set_stomp_to_connected_status)
{	
	stomp *stp = add_stomp(sock, stub_send_handler, NULL);
	CHECK_EQUAL(0, is_connected(stp));
	set_to_connected(stp);
	CHECK_EQUAL(1, is_connected(stp));
	
	close_stomp(stp);
}

TEST(should_return_null_when_given_null_dest_name)
{
	int sock = 106;
	destination *dest = subscribe_to_destination(sock, NULL);
	CHECK_EQUAL((destination *)NULL, dest);
}

TEST(should_return_1_when_subscribed_to_a_destination)
{
	int sock = 107;
	destination *dest = subscribe_to_destination(sock, "new_destination");
	CHECK_EQUAL(1, is_subscribed_to_destination(sock, dest));
}

TEST(should_return_0_when_not_subscribed_to_a_destination)
{
	int sock = 108;
	destination *dest = subscribe_to_destination(sock, "another_destination");
	CHECK_EQUAL(0, is_subscribed_to_destination(107, dest));
}