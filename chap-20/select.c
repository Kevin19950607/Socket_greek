
#include "common.h"


#define  MAXLINE     1024

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		error(1, 0, "usage: select01 <IPaddress>");
	}
	
	int socket_fd =  tcp_client(argv[i], SERV_PORT);

	char recv_line[MAXLINE], send_line[MAXLINE];
	int n;
	
	fd_set readmask;
	fd_set allreads;
	
	//  初始化了一个描述符集合，这个描述符读集合是空的：
	FD_ZERO(&allreads);
	
	// 将描述符 0，即标准输入，以及连接套接字描述符 3 设置为 待检测：
	FD_SET(0, &allreads);               // 标准输入待检测
	FD_SET(socket_fd, &allreads);       // 套接字描述待检测
	
	
	for(;;)
	{
		readmask = allreads;   // 每次循环开头重置待测试的描述符集合
		int rc = select(socket_fd+1, &readmask, NULL, NULL, NULL);
		
		if(rc <0)
		{
			error(1, 0, "select failed");
		}
		// 下面是通过 select 来检测套接字描述字有数据可读，或者标准输入有数据可读
		
		if(FD_ISSET(socket_fd, &readmask))         // 判断连接描述符可读
		{
			n = read(socket_fd, recv_line, MAXLINE);
			if(n < 0)
			{
				error(1, error, "read error");
			}
			else if( 0 == n)
			{
				error(1, 0, "server terminater\n");
			}
			
			recv_line[n] = 0;
			fputc(recv_line, stdout);
			fputc("\n", stdout);
			
		}
		
		if (FD_ISSET(STDIN_FILENO, &readmask))      // 判断标准输入描述符可读
		{
            if (fgets(send_line, MAXLINE, stdin) != NULL) 
			{
                int i = strlen(send_line);
                if (send_line[i - 1] == '\n') 
				{
                    send_line[i - 1] = 0;
                }

                printf("now sending %s\n", send_line);
                ssize_t rt = write(socket_fd, send_line, strlen(send_line));  // size_t
                if (rt < 0) 
				{
                    error(1, errno, "write failed ");
                }
                printf("send bytes: %zu \n", rt);
            }
        }
				
	}

	
	return 0;
}











