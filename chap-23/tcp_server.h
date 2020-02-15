
#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "common.h"

int tcp_server(int port);
int tcp_server_listen(int port);
void make_nonblocking(int fd);
int tcp_nonblocking_server_listen(int port);

#endif