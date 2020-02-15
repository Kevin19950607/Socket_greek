
#define MAX_LINE 4096

char rot13_char(char c)
{
	if((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
	{
		return c + 13;
	}
	else if((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
	{
		return c - 13;
	}
	else
	{
		return c;
	}
}

void child_run(int fd)
{
	char outbuf[MAX_LINE + 1];
	size_t outbuf_used = 0;
	ssize_t result;
	
	while(1)
	{
		char ch;
		result = recv(fd, &ch, 1, 0);
		if(0 == result)
		{
			break;
		}
		else if( -1 == result)
		{
			perror("read");
			break;
		}
		
		if( outbuf_used < sizeof(outbuf))
		{
			outbuf[outbuf_used++] = rot13_char(ch);
		}
		
		if( ch == '\n')
		{
			send(fd, outbuf_used, 0);
			outbuf_used = 0;
			continue;
		}
	}
	
}


// WNOHANG 用来告诉内核，即使还有未终止的子进程也不要阻塞在 waitpid 上
// 函数 sigchld_handler，在一个循环体内调用了 waitpid 函数，以便回收所有已终止的子进程
void sigchld_handler(int sig)
{
	while( waitpid(-1, 0, WNOHANG) >0);
	
	return;
}

int main(int argc, char* argv[])
{
	int listener_fd = tcp_server_listen(SERV_PORT);
	// 注册一个 SIGCHOLD 函数
	signal(SIGCHLD, sigchld_handler);
	while(1)
	{
		struct sockaddr_storage ss;
		socklen_t slen = sizeof(ss);
		int fd = accept(listener_fd, (struct sockaddr *) &ss, &slen);
		if(fd <0)
		{
			error(1, error, "accept failed");
			exit(1);
		}
		// 子进程处理逻辑
		if(0 == fork())
		{
			close(listener_fd); // 子进程不需要关心监听套接字，这里关闭掉监听套接字 listen_fd
			child_run(fd);
			exit(0);
		}
		// 父进程逻辑
		else
		{
			close(fd); // 父进程不需要关心连接套接字，所以在这里关闭连接套接字
		}
				
	}  
	
	return 0;
	
}