#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int my_global;

void* my_thread_handle(void *arg) 
{
	int val;

	val = *((int*)arg);

	printf("new thread begin, arg=%d\n", val);
	my_global += val;

	sleep(3);

	pthread_exit(&my_global);

	//  不再执行
	printf("new thread end\n");
}

int main(void)
{
	pthread_t  mythread;
	int arg;
	int ret;
	void *thread_return;

	arg = 100;
	my_global = 1000;

	printf("my_global=%d\n", my_global);
	printf("ready create thread...\n");
	
	ret = pthread_create(&mythread, 0, my_thread_handle, &arg);
	if (ret != 0) {
		printf("create thread failed!\n");
		exit(1);
	}

	printf("wait thread finished...\n");
	ret = pthread_join(mythread, &thread_return);
	if (ret != 0) {
		printf("pthread_join failed!\n");
		exit(1);
	}
	printf("wait thread end, return value is %d\n", *((int*)thread_return));
	printf("my_global=%d\n", my_global);

	printf("create thread finished!\n");
}
