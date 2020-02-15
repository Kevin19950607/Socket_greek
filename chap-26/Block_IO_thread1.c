
#include "common.h"
#include "tcp_server.h"



#define MAX_LINE 16384

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

void loop_echo(int fd) {
    char outbuf[MAX_LINE + 1];
    size_t outbuf_used = 0;
    ssize_t result;
    while (1) {
        char ch;
        result = recv(fd, &ch, 1, 0);

        //断开连接或者出错
        if (result == 0) {
            break;
        } else if (result == -1) {
            error(1, errno, "read error");
            break;
        }

        if (outbuf_used < sizeof(outbuf)) {
            outbuf[outbuf_used++] = rot13_char(ch);
        }

        if (ch == '\n') {
            send(fd, outbuf, outbuf_used, 0);
            outbuf_used = 0;
            continue;
        }
    }
}

void thread_run(void* arg)
{
	// 每个连接都由一个线程单独处理，服务器程序不需要对每个子线程进行终止，在入口函数的开始地方设置分离
	// 在它终止后自动回收相关线程资源，不要调用 pthread_join() 
	pthread_detach( pthread_self());
	int fd = (int)arg;
	loop_echo(fd);
}


int main(int agvc, char* argv[])
{
	int listener_fd = tcp_server_listen(SERV_PORT);
	pthread_t tid;
	
	while(1)
	{
		struct sockaddr_storage ss;
		socklen_t slen = sizeof(ss);
		
		int fd = accept(listener_fd, (struct sockaddr *)&ss, &slen);
		
		if(fd <0)
		{
			error(1, errno, "accept faied");
			
		}
		else
		{
			pthread_create(&tid, NULL, &thread_run, (void*)fd);
		}
	}
	
	return 0;
}



