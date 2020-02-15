
typedef struct
{
	u_int32_t type;
	char data[1024];
}messageObject_T;

#define MSG_PING  1
#define MSG_PONG  2
#define MSG_TYPE1 11
#define MSG_TYPE2 21


#define KEEP_ALIVE_TIME       10
#define KEEP_ALIVE_INTERVAL   3
#define KEEP_ALIVE_PROBETIMES 3


int main(int argc, char *argv[])
{
	if(argc !=2)
	{
		error(1, 0, "usage: tcpclient<IPaddress>");
	}
	
	int socket_fd;
	socket_fd = socket(AF_INET, SOCK_STREAM , 0);
	
	struct sockadd_in server_addr;   // ipv4
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
	
	socklen_t server_len = sizeof(server_addr);
	
	int connect_rt = connect(socket_fd, (struct sockaddr *)&server_addr, server_len);
	if(connect_rt < 0)
	{
		error(1, errno, "connect failed");
	}
	
	char recv_line[MAXLINE + 1];
	int n;
	
	fd_set readmask;
	fd_set allreads;
	
	struct timeval tv;
	int heartbeats = 0;
	
	tv.tv_sec = KEEP_ALIVE_TIME;
	tv.tv_usec = 0;
	
	messageObject_T tmessageObject 
	
	FD_ZERO(&allreads);
	FD_SET(socket_fd, &allreads);
	
	for(;;)
	{
		readmask = allreads;
		int rt = select(socket_fd+1, &readmask, NULL, NULL, &tv);
		if(rt < 0)
		{
			error(1, errno, "select failed");
		}
		if(rt == 0)
		{
			if(++heartbeats > KEEP_ALIVE_PROBETIMES)
			{
				error(1, 0, "connection dead\n");
			}
			printf("sending heartbeats #%d\n", heartbeats);
			tmessageObject.type = htonl(MSG_PING);
			rt = send(socket_fd, (char *)&tmessageObject, sizeof(tmessageObject), 0);
			
			if(rt < 0)
			{
				error(1, 0, "send failed")
			}
			tv.tv_sec = KEEP_ALIVE_INTERVAL;
			continue;			
		}
		if(FD_ISSET(socket_fd, &readmask))
		{
			n = read(socket_fd, recv_line, MAXLINE);
			if(n < 0)
			{
				error(1, errno, "read error");
			}
			else if(n==0)
			{
				error(1, 0, "server terminated\n");
			}
			printf("received heartbeats, make heartbeats to 0\n");
			heartbeats = 0;
			tv.tv_sec = KEEP_ALIVE_TIME;
			
		}
	}
	
}





