#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#define BUFSIZE 65537       
#define MAXPENDING 5      
#define PORT 61613            

int start_server();
void handle_tcp_client(int clntSocket);

#endif /* end of include guard: TCP_SERVER_H */
