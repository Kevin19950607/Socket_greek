#include <stdio.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>

#define MESSAGE_SIZE 10240
#define SERVER_PORT 3000
#define SERVER_ADDR "127.0.0.1"

void senddata(int fd)
{
	char* query;
	query = (char*)malloc(MESSAGE_SIZE+1);
	for(int i=0; i<MESSAGE_SIZE; i++)
	{
		query[i] = 'a';
	}
	query[MESSAGE_SIZE] = '\0';
	
	const char* cp = query;
	
	long len = strlen(cp); // query
	
	while(len)
	{
	  long n_written = send(fd, cp, len, 0);
	  fprintf(stdout, "send into buffer %ld \n", n_written);
	  
	  if(n_written < 0)
	  {		         
         printf("send\n");
         return ;       
	  }
	  len -= n_written;
	  cp += n_written;
	  		
	}
	
}


int main(int agvc, char* argv[])
{
	int clientfd;
	struct sockaddr_in serveraddr;
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVER_PORT);
	serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	clientfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(clientfd < 0)
	{
		printf("creat socket error");
		return -1;
	}
	
	int result = connect(clientfd, (struct sockaddr *)&serveraddr,  sizeof(serveraddr));
	if(result < 0)
	{
		printf("connect error");
		return -1;
	}
	
	senddata(clientfd);
	
	close(clientfd);
	
	printf("send data finish\n");   // 非阻塞验证
	
	return 0;
}