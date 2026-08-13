#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

#include <pthread.h>
#include <semaphore.h>

#define BUF_SIZE (2*1024)
#define BUF_HDR_SIZE (1)

#define NUM_CHILDREN 4  // 3 prod + 1 cons

typedef struct my_shm {
	pthread_mutex_t mutex;	
	sem_t			semaphore;

	size_t			read_pos;
	size_t			write_pos;
	size_t			bytes_used;
	bool			end_process;
	int				test_val;

	char			buffer[BUF_SIZE] __attribute__ ((aligned(4)));

} my_shm;


#define PRODUCER1_SIZE 16
#define PRODUCER2_SIZE 32
#define PRODUCER3_SIZE 64

int get_free_space(void *ptr) {
	struct my_shm *shm_ptr = ptr;
	size_t wp = shm_ptr->write_pos, rp = shm_ptr->read_pos;
	size_t size = 0;

#if 0
	if (wp >= rp) {
		size = BUF_SIZE - (wp - rp);
	} else {
		size = (rp - wp);	
	}
#else
	size = (BUF_SIZE - shm_ptr->bytes_used);

#endif
	return size;
}

void write_buffer(void *ptr, char c, int size) {
	struct my_shm *shm_ptr = ptr;
	size_t wp = shm_ptr->write_pos, rp = shm_ptr->read_pos;

	/* Prepare payload */
	char *buf = (char *)malloc(sizeof(char) * (size + 1));
	buf[0] = size;
	for (int i = 1; i <= size; i++)
		buf[i] = c;

	if (wp >= rp) {
		if ((wp + size + 1) >= BUF_SIZE) {
			int bytes_remain = ((wp + size + 1) % BUF_SIZE);
			memcpy(&(shm_ptr->buffer[wp]), buf, (size + 1 - bytes_remain));
			memcpy(&(shm_ptr->buffer[0]), (void *)&buf[size + 1 - bytes_remain], bytes_remain);
		} else {
			memcpy(&(shm_ptr->buffer[wp]), buf, (size + 1));
		}
	} else {
		memcpy(&(shm_ptr->buffer[wp]), buf, (size + 1));
	}

	shm_ptr->write_pos = ((wp + size + 1) % BUF_SIZE);
	shm_ptr->bytes_used += (size + 1);
	printf("write: size = %d, c = %c, read = %ld, write = %ld, used = %ld \n",
			size, c, shm_ptr->read_pos, shm_ptr->write_pos, shm_ptr->bytes_used);
}

void read_buffer(void *ptr) {

	struct my_shm *shm_ptr = ptr;
	size_t wp = shm_ptr->write_pos, rp = shm_ptr->read_pos;

	int size = (int)shm_ptr->buffer[rp];
	shm_ptr->read_pos = ((rp + size + 1) % BUF_SIZE);
	char c = shm_ptr->buffer[((rp + 1) % BUF_SIZE)];
	shm_ptr->bytes_used -= (size + 1);
	printf("read: size = %d, c = %c, read = %ld, write = %ld, used = %ld \n",
			size, c, shm_ptr->read_pos, shm_ptr->write_pos, shm_ptr->bytes_used);
}

void producer (void *ptr, int value) {
	char p1 = 'a', p2 = 'c', p3 = 'e';
	char c;
	struct my_shm *shm_ptr = ptr;

	char out;
	int size;
	char *prod;

	srand(time(NULL));

	printf("Starting producer%d \n", value);
	sleep(1); // settling time for producers

	switch (value) {
		case 1:
			c = p1;
			// size = PRODUCER1_SIZE+BUF_HDR_SIZE;
			size = PRODUCER1_SIZE;
			prod = "P1";
			break;
		case 2:
			c = p2;
			// size = PRODUCER2_SIZE+BUF_HDR_SIZE;
			size = PRODUCER2_SIZE;
			prod = "P2";
			break;
		case 3:
			c = p3;
			// size = PRODUCER3_SIZE+BUF_HDR_SIZE;
			size = PRODUCER3_SIZE;
			prod = "P3";
			break;
	}

	while (shm_ptr->end_process == false) {
		pthread_mutex_lock(&(shm_ptr->mutex));
		int free_space = get_free_space(ptr);

		if (free_space >= size) {
			write_buffer(ptr, c, size);
			printf("[%s]:%d bytes written \n", prod, size);
			sem_post(&(shm_ptr->semaphore));
		} else {
			printf("[%s]:buffer full, dropping payload \n", prod);
		}
		pthread_mutex_unlock(&(shm_ptr->mutex));

		int random_value = (rand() % 3) + 1;
		random_value *= 100000; // msec
		usleep(random_value);
	}
}

void consumer (void *ptr, int value) {
	struct my_shm *shm_ptr = ptr;

	printf("Starting consumer \n");

	while (shm_ptr->end_process == false) {
		sem_wait(&(shm_ptr->semaphore));
		pthread_mutex_lock(&(shm_ptr->mutex));
		read_buffer(ptr);
		pthread_mutex_unlock(&(shm_ptr->mutex));
	}
}

int main()
{
	const char *name = "/my_shm_region";

	int shm_fd;
	void *ptr;

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1) {
		printf("shm_open() failed \n");
		exit(-1);
	} else {
		printf("shm_open() succeeded \n");
	}

	if (ftruncate(shm_fd, sizeof(struct my_shm)) == -1) {
		printf("ftruncate() failed \n");
		exit(-1);
	} else {
		printf("ftruncate() succeeded \n");
	}

	ptr = mmap(0, sizeof(struct my_shm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("mmap() failed \n");
		exit(-1);
	} else {
		printf("mmap() succeeded \n");
	}

	/* Initialize shared memory */
	memset(ptr, 0, sizeof(struct my_shm));

	struct my_shm *shm_ptr = ptr;
	pthread_mutex_t *p_mutex = &shm_ptr->mutex;
	pthread_mutexattr_t attr;

	shm_ptr->read_pos		= 0L;
	shm_ptr->write_pos		= 0L;
	shm_ptr->bytes_used		= 0L;
	shm_ptr->end_process	= false;
	shm_ptr->test_val		= 790;

	pthread_mutexattr_init(&attr);	
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(p_mutex, &attr);

	if (sem_init(&(shm_ptr->semaphore), 1, 0) == -1) {
		printf("sem_init() failed \n");
		exit(-1);
	} else {
		printf("sem_init() succeeded \n");
	}

	pid_t pid[NUM_CHILDREN];

	for (int i = 0; i < NUM_CHILDREN; i++) {
		fflush(stdout);
		pid[i] = fork();

		if (pid[i] < 0) {
			printf("fork() failed \n");
			exit(-1);
		} else if (pid[i] == 0) { // child
			printf("i = %d \n", i);
			fflush(stdout);
			switch(i) {
				case 0:
					consumer(ptr, i);
					exit(0);
				case 1:
					producer(ptr, i);
					exit(0);
				case 2:
					producer(ptr, i);
					exit(0);
				case 3:
					producer(ptr, i);
					exit(0);
			}
		} else { // parent
			sleep(1);
		}
	}

	/* sleep for 60 secs */
	sleep(60);
	shm_ptr->end_process    = true;

	for (int i = 1; i < NUM_CHILDREN; i++) {
		int status;
		waitpid(pid[i], &status, 0);
	}
	kill(pid[0], SIGTERM); // kill the consumer

	munmap(ptr, sizeof(struct my_shm));
	close(shm_fd);

	printf("Parent process exiting! \n");
	return 0;
}
