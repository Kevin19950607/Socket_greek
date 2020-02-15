
#include "common.h"


int tcp_server(int port) {
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        error(1, errno, "bind failed ");
    }

    int rt2 = listen(listenfd, LISTENQ);
    if (rt2 < 0) {
        error(1, errno, "listen failed ");
    }

    signal(SIGPIPE, SIG_IGN);

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
        error(1, errno, "bind failed ");
    }

    return connfd;
}

int main(int argc, char *argv[])
{
	int connfd;
	char buf[1024];
	
	connfd = tcp_server(SERV_PORT);
	
	for(;;)
	{
		int n = read(connfd, buf, 1024);
		if(n < 0)
		{
			error(1, errno, "read failed");
		}
		if(n == 0)
		{
			error(1, 0, "client close");
		}
		
		sleep(5);
		
		int write_nc = send(connfd, buf, n, 0);
		if(write_nc < 0)
		{
			error(1, errno, "write error");
		}		
	}
	
	
	exit(0);
}
