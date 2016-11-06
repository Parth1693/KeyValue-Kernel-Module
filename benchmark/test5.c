//Producer and consumer threads test case

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <keyvalue.h>
#include <fcntl.h>
#include <assert.h>
#include <pthread.h>

#define N 40

char *data;

pthread_t producers[N];
pthread_t consumers[N];

void* producerThread(void* arg)
{
	pthread_t id = pthread_self();
	int i;
	int devfd;
	__u64 size[100000];
	int start = -1;
	int tid;

	for(i=0; i<N; i++)
	{
		if(pthread_equal(id, producers[i]))
		{
			printf("Producer thread %d STARTED!\n", i);
			start = i;
		}
	}

	devfd = open("/dev/keyvalue", O_RDWR);
	if(devfd < 0)
	{
	  fprintf(stderr, "Device open failed in producer thread!\n");
	  exit(1);
	}

	if(start != -1) {
	for(i = N*start; i<(N*start)+5; i++)
	{
		//k = rand();	//Randomize key
		size[i] = (i*10) % 4096;
		tid = kv_set(devfd, i, size[i], (void *)data);
	}
	}
	else
		printf("Thread ID ERROR!!\n");

	printf("Producer thread %d FINISHED!\n", start);

	return NULL;
}

void* consumerThread(void* arg)
{
	pthread_t id = pthread_self();
	int i, j;
	int devfd;
	char *check_data = (char *)malloc(4096);
	__u64 size[100000];
	int start = -1;
	__u64 check_size;
	int test = 1;
	int tid;

	for(i=0; i<N; i++)
	{
		if(pthread_equal(id, consumers[i]))
		{
			printf("Consumer thread %d STARTED!\n", i);
			start = i;
		}
	}

	devfd = open("/dev/keyvalue", O_RDWR);
	if(devfd < 0)
	{
	  fprintf(stderr, "Device open failed in producer thread!\n");
	  exit(1);
	}

	//Read data from keyvalue store
	if(start != -1) {
	for(i = N*start; i<(N*start)+5; i++)
	{
		//k = rand();	//Randomize key
		size[i] = (i*10) % 4096;
		tid = kv_get(devfd, i, &check_size, (void *)check_data);
		if(tid != -1)
		{
			if(check_size != size[i])
			{
				printf("ERROR in consumer thread at size %d\n", i);
				test = 0;
				break;	
			}
			else
			{
				for(j=0; j<check_size; j++)
				{
					if(check_data[j] != data[j])
					{
						printf("ERROR in data!!\n");
						test = 0;
						break;
					}
				}
			} 			
		}
		else
			printf("NOTE:   Consumer %d ran before producer!!!!\n", i);

	}
	}
	else
		printf("Thread ID ERROR!!\n");

	if(test == 0)
		printf("Consumer thread %d FAILED!!\n", start);

	printf("Consumer thread %d FINISHED!\n", start);

	return NULL;
}

int main(int argc, char *argv[])
{
	int i = 0, number_of_threads = 1, number_of_keys = 1024, number_of_transactions = 65536; 
	int k, j;
	int err, a;

	data = (char *)malloc(4096);

	if(argc < 3)
	{
	  fprintf(stderr, "Usage: %s number_of_keys number_of_transactions\n", argv[0]);
	  exit(1);
	}

	number_of_keys = atoi(argv[1]);
	number_of_transactions = atoi(argv[2]);

	// Allocate some random data
	for(i=0; i<4096; i++)
	{
		data[i] = rand() % 256;
	}

	for(i=0; i<N; i++)
	{
		err = pthread_create(&(producers[i]), NULL, &producerThread, NULL);
		if (err != 0)
            	printf("\nCan't create producer thread %d", i);

            err = pthread_create(&(consumers[i]), NULL, &consumerThread, NULL);
            if (err != 0)
            	printf("\nCan't create consumer thread %d", i);
	}

	for(i=0; i<N; i++)
	{
		pthread_join(producers[i], NULL);
	}

	for(i=0; i<N; i++)
	{
		pthread_join(consumers[i], NULL);
	}
	
	sleep(1);
	return 1;
}
