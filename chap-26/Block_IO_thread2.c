
#include "common.h"
#include "tcp_server.h"


#define  MAX_LINE           16384
#define  THREAD_NUMBER      4
#define  BLOCK_QUEUE_SIZE   100

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


typedef struct 
{
    pthread_t thread_tid;        /* thread ID */
    long thread_count;           /* # connections handled */
} Thread_T;

Thread_T* pthread_array;

//定义一个队列
typedef struct {
    int number;  //队列里的描述字最大个数
    int* fd;     //这是一个数组指针
    int front;   //当前队列的头位置
    int rear;    //当前队列的尾位置
    pthread_mutex_t mutex;  //锁
    pthread_cond_t cond;    //条件变量
} block_queue_T;


// init queue
void block_queue_init(block_queue_T* pblockQueue, int number)
{
	pblockQueue->number = number;
	pblockQueue->fd = calloc(number, sizeof(int));
	pblockQueue->front = 0;
	pblockQueue->rear = 0;
	pthread_mutex_init(&pblockQueue->mutex, NULL);
	pthread_cond_init(&pblockQueue->cond, NULL);
	
}

//往队列里放置一个描述字fd
void block_queue_push(block_queue_T* pblockQueue, int fd)
{
	// 先加锁，因为有多个线程需要读写队列
	pthread_mutex_lock(&pblockQueue->mutex);
	
	// 将描述符放到队列尾部位置
	pblockQueue->fd[pblockQueue->rear] = fd;
	
	// 如果已经到最后，重置尾的位置
	if(++ pblockQueue->rear == pblockQueue->number)
	{
		pblockQueue->rear = 0;
	}
	
	printf("push fd %d", fd);
	
	// 通知其他等待读的线程，有新的连接子等待处理
	pthread_cond_signal(&pblockQueue->cond);
	
	// 解锁
	pthread_mutex_unlock(&pblockQueue->mutex);
	
}

int block_queue_pop(block_queue_T* pblockQueue)
{
	// 加锁
	pthread_mutex_lock(&pblockQueue->mutex);
	//判断队列里没有新的连接字可以处理，就一直条件等待，直到有新的连接字入队列
	while(pblockQueue->front == pblockQueue->rear)
	{
		pthread_cond_wait(&pblockQueue->cond, &pblockQueue->mutex);
		
		// 取出队列头的连接字
		int fd = pblockQueue->fd[pblockQueue->front];
		// 如果已经到最后，重置头的位置
		
		if(++pblockQueue->front == pblockQueue->number)
		{
			pblockQueue->front = 0;
		}
		
		printf("pop fd %d\n", fd);
		
		// 解锁
		pthread_mutex_unlock(&pblockQueue->mutex);
		
		return fd;
	}
	
}

void thread_run(void *arg)
{
	pthread_t tid = pthread_self();
	pthread_detach(tid);
	
	block_queue_T* pblockQueue = (block_queue_T*)arg;
	
	while(1)
	{
		int fd = block_queue_pop(pblockQueue);
		printf("get fd in thread, fd = %d, tid = %d\n", fd, tid);
		loop_echo(fd);		
	}
	
}



int main(int agvc, char* argv[])
{
	int listener_fd = tcp_server_listen(SERV_PORT);
	
	block_queue_T tblockQueue;
	
	block_queue_init(&tblockQueue, BLOCK_QUEUE_SIZE);
	
	pthread_array = (Thread_T*)calloc(THREAD_NUMBER, sizeof(Thread_T));
	
	int i;
	for( i=0; i < THREAD_NUMBER; i++)
	{
		pthread_create(&(pthread_array[i].thread_tid), NULL, &thread_run, (void*)&tblockQueue);		
	}
	
	while(1)
	{
		struct sockaddr_storage ss;
		socklen_t slen = sizeof(ss);
		
		int fd = accept(listener_fd, (struct sockaddr *) &ss, &slen);
		
		if(fd <0)
		{
			error(1, errno, "accept failde");
		}
		else
		{
			block_queue_push(&tblockQueue, fd);
		}
	}
	
	
	return 0;
	
	
}








