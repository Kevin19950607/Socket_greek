
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>

// 创建套接字，绑定端口，参数是端口号，返回套接字描述符
int makeSocket(short port)
{
	int fd;
	struct sockaddr_in name; 
	
	fd  = socket(AF_INET, SOCK_STREAM, 0);
	
	if(fd <0)
	{
		printf("creat socket error\n");
		return -1;
	}
	
	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if( -1 == bind(fd, (struct sockaddr*)&name, sizeof(name)))
	{
		printf("bind error\n");
		return -1;
	}
	
	return fd;
		
}

// 从 socket fd 描述符中读取 size 个字节
// typedef unsigned long size_t
// typedef long ssize_t
size_t readn(int fd, void* buffer, size_t size)
{
	char* buffer_pointer = buffer;
	int length = size;
	
	while( length >0)
	{
		int result = read(fd, buffer_pointer, length);
		
		if(result <0)
		{
			if( errno == EINTR)
				continue;        // 考虑非阻塞的情况，没有数据可读，需要再次调用 read
			else
				return -1;
		}
		else if(result == 0)
		{
			break;              // EOF 表示套接字关闭
		}
		
		length -= result;
		
		buffer_pointer += result;
				
	}
	
	return (size - length);  // 返回实际读取的字节数（读取 EOF 跳出循环后）
		
}





int main(int argc, char* argv[])
{
	int servefd;
	
	servefd = makeSocket(3000);
	
	printf("%d\n", servefd);
	
	return 0;	
}





