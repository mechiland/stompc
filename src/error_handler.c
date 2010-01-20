#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "error_handler.h"

void die_with_user_message(const char *message, const char *detail)
{
	fputs(message, stderr);
	fputs(": ", stderr);
	fputs(detail, stderr);
	fputc('\n', stderr);
	exit(1);
}   

void die_with_system_message(const char *message)
{
	perror(message);
	exit(1);
}