
#include "common.h"
#include "tcp_server.h"

#define MAXEVENTS 128

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


int main()
{
	int listen_fd, socket_fd;
	
	int n, i;
	int efd;
	
	struct epoll_event event;
	struct epoll_event *pevents;
	
	listen_fd = tcp_nonblocking_server_listen(SERV_PORT);
	
	efd =  epoll_create1(0);
	if(efd == -1)
	{
		error(1, 0, "epoll create failed");
	}
	
	event.data.fd = listen_fd;
	event.events = EPOLLIN | EPOLLET;
	
	if( epoll_ctl(efd, EPOLL_CTL_ADD, listen_fd, &event) == -1)
	{
		error(1, errno, "epoll_ctl add listen_fd failed");
	}
	
	/* Buffer where events are returned */
	pevents = calloc(MAXEVENTS, sizeof(event));
	
	while(1)
	{
		n = epoll_wait(efd, pevents, MAXEVENTS, -1);
		printf("epoll_wait wakeup\n");
		
		for(i=0; i<n; i++)
		{
			if( (pevents[i].events & EPOLLERR) || (pevents[i].events & EPOLLHUP) || (!(pevents[i].events & EPOLLIN)) )
			{
				fprintf(stderr, "epoll error\n");
				close(pevents[i].data.fd);
				continue;
			}
			else if(listen_fd == pevents[i].data.fd)
			{
				struct sockaddr_storage ss;
				socklen_t slen = sizeof(ss);
				
				int fd = accept(listen_fd, (struct sockaddr *) &ss, &slen);
				if(fd < 0)
				{
					error(1, errno, "accept failed");
				}
				else
				{
					make_nonblocking(fd);
					event.data.fd = fd;					
					event.events = EPOLLIN | EPOLLET; //edge-triggered
					
					if(epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event) == -1)
					{
						error(1, errno, "epoll_ctl");
					}
					
				}
				continue;
			}
			else
			{
				socket_fd = pevents[i].data.fd;
				printf("get event on socket fd == %d \n", socket_fd);
				
				while(1)
				{
					char buf[512];
					if((n = read(socket_fd, buf, sizeof(buf))) < 0)
					{
						if( errno != EAGAIN)
						{
							error(1, errno, "read error");
							close(socket_fd);
						}
						break;
					}
					else if(n == 0)
					{
						close(socket_fd);
						break;
					}
					else
					{
						for(i=0; i<n; ++i)
						{
							buf[i] = rot13_char(buf[i]);
						}
						if(write(socket_fd, buf, n) < 0)
						{
						    error(1, errno, "write error");	
						}
					}
				}
			}
		}		
	}
	
	printf("hello world");

	free(pevents);
	close(listen_fd);
	
}











