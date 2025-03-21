#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cond;

void *thread1(void *arg)
{

	
	while (1) {

		printf("thread1 is running\n");
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond, &mutex);
		printf("thread1 applied the condition\n");
		pthread_mutex_unlock(&mutex);

		sleep(4);
	}

}


void *thread2(void *arg)
{
	while (1) {

		printf("thread2 is running\n");
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&cond, &mutex);
		printf("thread2 applied the condition\n");
		pthread_mutex_unlock(&mutex);

		sleep(2);

	}

}

int main()
{

	pthread_t thid1, thid2;

	printf("condition variable study!\n");

	pthread_mutex_init(&mutex, NULL);

	pthread_cond_init(&cond, NULL);

	pthread_create(&thid1, NULL, (void *)thread1, NULL);

	pthread_create(&thid2, NULL, (void *)thread2, NULL);

	do {

		pthread_cond_signal(&cond);
       sleep(1);

	} while (1);

	

	return 0;

}
