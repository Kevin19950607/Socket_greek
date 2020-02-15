#include <stdio.h>
#include <sys/types.h>         
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>


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


// 每次读取1K的数据然后休眠1秒
void readdata(int fd)
{
	size_t n;
	char buf[1024];
	int time = 0;
	
	for(; ;)
	{
		fprintf(stdout, "block in ready\n");
		
		if( (n = readn(fd, (void*)buf, 1024)) == 0 )
		{
			return;
		}
		
		time ++;
		
		fprintf(stdout, "1K read for %d \n", time);        
		usleep(1000000);
				
	}
	
}


int main(int agvc, char* argv[])
{
	int listenfd, newclientfd;
	struct sockaddr_in  serveraddr, newclientaddr;
	socklen_t newclitent_len;    // socklen_t -> int 
	
	
	bzero(&serveraddr, sizeof(serveraddr));   // 对地址进行置0处理，相当于初始化。 该函数只能在 linux GCC 下编译通过，如果跨平台使用 memset()
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(3000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(listenfd <0)
	{
		printf("creat socket error");
		
		return -1;
	}
	
	if( bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr) ) == -1)
	{
		printf(" bind error");
		
		return -1;
	}
	
	//  int listen(int sockfd, int backlog);   backlog 参数表示容纳客户端最大数量
	listen(listenfd, 1024);  // SOMAXCONN ?
	
	
	for(; ;)
	{
		newclitent_len = sizeof(newclientaddr);
		newclientfd = accept(listenfd, (struct sockaddr*)&newclientaddr , &newclitent_len);
		
		readdata(newclientfd);		 // 读取数据
		
		close(newclientfd);		     // 关闭套接字，注意不是监听套接字
	}
	
	
	
	
	return 0;
}