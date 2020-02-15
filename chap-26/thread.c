
#include <stdio.h>
#include <pthread.h>


int another_shared = 0;

void thread_run(void* arg)
{
	int* calculator = (int*)arg;
	printf("tid = %d\n", pthread_self());
	
	for(int i=0; i < 1000; i++)
	{
		*calculator += 1;
		another_shared += 1;
	}
	
}


int main(int argc, char* argv[])
{
	int calculator;
	
	// pthread_t -> unsigned long int
	pthread_t tid1;
	pthread_t tid2;
	
	pthread_create(&tid1, NULL, &thread_run, &calculator);
	pthread_create(&tid2, NULL, &thread_run, &calculator);
	
	
	pthread_join(tid1, NULL);
	pthread_join(tid1, NULL);
	
	
	printf("calculator is %d\n", calculator);
	printf("another_shared is %d\n", another_shared);
	
	return 0;
}







