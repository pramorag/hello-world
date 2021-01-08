#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int a;

void *thread1 (void * arg) {
	a = 5;

	while(a == 5) {

	}
	printf("Exiting thread 1 \n");
}

void *thread2 (void * arg) {
	
	sleep(5);

	a = 6;

	printf("Exiting thread 2 \n");
}

int main()
{

	pthread_t tid[2];

	pthread_create(&tid[0], 0, thread1, NULL);
	pthread_create(&tid[1], 0, thread2, NULL);

	pthread_join(tid[0], 0);
	pthread_join(tid[1], 0);

	return 0;
}
