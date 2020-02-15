
// 非阻塞IO + select 多路复用


#define MAX_LINE 1024
#define FD_INIT_SIZE 128


char rot13_char(char c)
{
	if((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
	{
		return c + 13；
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


// 数据缓冲区
struct Buffer
{
	int connect_fd;   
	char buffer[MAX_LINE];
	size_t writeIndex;       // 缓冲写入位置
	size_t readIndex;        // 缓冲读取位置
	int readable;            // 是否可读
};


struct Buffer *alloc_Buffer()
{
	struct Buffer *buffer = (struct Buffer *)malloc(sizeof(struct Buffer);
	if( buffer != NULL)
	{
		buffer->connect_fd = 0;
		buffer->writeIndex = 0;
		buffer->readIndex  = 0;
		buffer->readable   = 0;
	}
	
	return buffer;
}


void free_Buffer(struct Buffer *buffer)
{
	if(buffer != NULL)
	{
		free(buffer);
	}	
}

int onSocketRead(int fd, struct Buffer *buffer)
{
	char buf[1024];
	int i;
	ssize_t result;
	
	while(1)
	{
		result = recv(fd, buf, sizeof(buf), 0);
		if(result <=0)
		{
			break;
		}
		for(i=0; i<result; ++i)
		{
			if(buffer->writeIndex < sizeof(buffer->buffer))
			{
				buffer->buffer[buffer->writeIndex++] = rot13_char(buf[i]);
			}
			if(buf[i] == '\n')
			{
				buffer->readable = 1;    // 缓冲区可读
			}
		}
	}
	if(result == 0)
	{
		return 1;
	}
	else if(result < 0)
	{
		if(errno == EAGAIN)
		{
			return 0;
		}
		return -1;
	}
	
	return 0;
	
}

int onSocketWrite(int fd, struct Buffer *buffer)
{
	
}



int main(int argc, char* argv[])
{
	int listen_fd;
	int i, maxfd;
	
	struct Buffer *pbuffer[FD_INIT_SIZE];
	
	for(int i=0; i<FD_INIT_SIZE; i++)
	{
		pbuffer[i] = alloc_Buffer();
	}
	
	listen_fd = tcp_nonblocking_server_listen(SERV_PORT);
	
	fd_set readset, writeset, exset;
	FD_ZERO(&readset);
	FD_ZERO(&writeset);
	FD_ZERO(&exset);
	
	
	while(1)
	{
		maxfd = listen_fd;
		
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		FD_ZERO(&exset);
		
		// listener加入readset
		
		FD_SET(listen_fd, &readset);
		
		for(i=0; i<FD_INIT_SIZE; i++)
		{
			if(pbuffer[i]->connect_fd >0)
			{
				if(pbuffer[i]->connect_fd > maxfd)
				{
					maxfd = pbuffer[i]->connect_fd;
				}
				FD_SET(pbuffer[i]->connect_fd, &readset);
				if(buffer[i]->readable)
				{
					FD_SET(pbuffer[i]->connect_fd, &writeset);
				}
			}
		}
		
		if(select(maxfd + 1, &readset, &writeset, &exset, NULL) <0)
		{
			error(1, errno, "select error");
		}
		
		if(FD_ISSET(listen_fd, &readset))
		{
			printf("listening socket readable");
			sleep(5);
			struct sockaddr_storage ss;          
			socklen_t slen = sizeof(ss);           
			int fd = accept(listen_fd, (struct sockaddr *) &ss, &slen);
			if(fd < 0)
			{
				error(1, 0, "accept error");
			}
			else if(fd > FD_INIT_SIZE)
			{
				error(1, 0, "too many connection");
				close(fd);
			}
			else
			{
				make_noblocking(fd);
				if(pbuffer[i]->connect_fd == 0)
				{
					pbuffer[fd]->connect_fd = fd;
				}
				else
				{
					error(1, 0, "too many connection");
				}
			}
		}

		for(i=0; i<maxfd + 1; ++i)
		{
			int r = 0;
			if( i == listen_fd)
			{
				continue;
			}
			
			if(FD_ISSET(i, &readset))
			{
				r = onSocketRead(i, pbuffer[i])
			}
			if(FD_ISSET(i, &writeset))
			{
				 r = onSocketWrite(i, pbuffer[i]);
			}
			if(r)
			{
				buffer[i]->connect_fd = 0;
				close(i);
			}
		}
	}
}






















