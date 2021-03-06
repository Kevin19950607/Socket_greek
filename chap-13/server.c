
#include "common.h"

static int count;

static void sig_int(int signo)
{
	printf("\nreceived %d datagrams\n", count);
	exit(0);
}


// size_t readn(int fd, void *buffer, size_t length) 
// {
    // size_t count;
    // ssize_t nread;
    // char *ptr;

    // ptr = buffer;
    // count = length;
    // while (count > 0) {
        // nread = read(fd, ptr, count);

        // if (nread < 0) {
            // if (errno == EINTR)
                // continue;
            // else
                // return (-1);
        // } else if (nread == 0)
            // break;                /* EOF */

        // count -= nread;
        // ptr += nread;
    // }
    // return (length - count);        /* return >= 0 */
// }

size_t readn(int fd, void *buffer, size_t size) {
    char *buffer_pointer = buffer;
    int length = size;

    while (length > 0) {
        int result = read(fd, buffer_pointer, length);

        if (result < 0) {
            if (errno == EINTR)
                continue;     /* 考虑非阻塞的情况，这里需要再次调用read */
            else
                return (-1);
        } else if (result == 0)
            break;                /* EOF(End of File)表示套接字关闭 */

        length -= result;
        buffer_pointer += result;
    }
    return (size - length);        /* 返回的是实际读取的字节数*/
}



size_t read_message(int fd, char *buffer, size_t length) {
    u_int32_t msg_length;
    u_int32_t msg_type;
    int rc;

    /* Retrieve the length of the record */

    rc = readn(fd, (char *) &msg_length, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;
    msg_length = ntohl(msg_length);

    rc = readn(fd, (char *) &msg_type, sizeof(msg_type));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;

    /* 判断buffer是否可以容纳下数据  */
    if (msg_length > length) {
        return -1;
    }

    /* Retrieve the record itself */
    rc = readn(fd, buffer, msg_length);
    if (rc != msg_length)
        return rc < 0 ? -1 : 0;
    return rc;
}


int main(int argc, char *argv[])
{
	int listenfd;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERV_PORT);
	
	int on = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	int rt1 = bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(rt1 < 0)
	{
		error(1, errno, "bind failed");
	}
		
	int rt2 = listen(listenfd, LISTENQ);
	if(rt2 < 0)
	{
		error(1, errno, "listen failed");
	}
	
	signal(SIGPIPE, SIG_IGN);
	
	int connfd;
	
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	
	if((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len)) < 0)
	{
		error(1, errno, "accept failed");
		
	}
	
	char buf[128];
	count = 0;
	
	while(1)
	{
		int n = read_message(connfd, buf, sizeof(buf));
		if(n < 0)
		{
			error(1, errno, "error read message");
		}
		else if(n == 0)
		{
			//printf("why");	
			error(1, 0, "client close\n");
		}
		
		buf[n] = 0;
		printf("recived %d bytes : %s\n", n, buf);
		
		count ++;				
	}
	
	exit(0);

}

