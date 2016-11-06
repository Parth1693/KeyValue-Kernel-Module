
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <keyvalue.h>
#include <fcntl.h>
#include <sys/syscall.h>

pthread_t threads[2];

void* checkThreads(void *arg)
{
	int tid;
	int test = 1;
	int number_of_transactions = 16;
	int i, j, k;
	__u64 size[number_of_transactions];
	__u64 keys[number_of_transactions];
	__u64 check_size;
	char * data = (char *)malloc(4096);
	char * check_data = (char *)malloc(4096);
	int devfd;      //keyvalue device file descriptor

    pthread_t id = pthread_self();

    if(pthread_equal(id, threads[0]))
    {
        printf("\n First thread processing\n");
    }
    else if(pthread_equal(id, threads[1]))
    {
        printf("\n Second thread processing\n");
    }

	test = 1;

	devfd = open("/dev/keyvalue", O_RDWR);

	if(devfd < 0)
	{
	  fprintf(stderr, "Device open failed");
	  exit(1);
	}

      // Allocate some random data
	for(i=0; i<4096; i++)
	{
		data[i] = rand() % 256;
	}

	// Initializing the key_value store
	for(i = 0; i < number_of_transactions; i++)
	{
		k = rand();	//Randomize key
		keys[i] = k;
		size[i] = rand() % 4096;
		tid = kv_set(devfd, keys[i], size[i], (void *)data);
	}

	for(i = 0; i <number_of_transactions; i++)
	{
		tid = kv_get(devfd, keys[i], &check_size, (void *)check_data);
		if(check_size != size[i])
		{
			printf("Error at size at Key:%llu\n", keys[i]);
			test = 0;
			break;	
		}
		else
		{
			for(j=0; j<check_size; j++)
			{
				if(check_data[j] != data[j])
				{
					printf("Error in data at Key:%llu\n", keys[i]);
					test = 0;
				}
			}
		} 
	}

	if(test == 1)
		printf("Thread was successful!\n");
	else
		printf("Thread FAILED!!!!!!\n");

    return NULL;
}

int main(int argc, char **argv)
{
    int i = 0;
    int err;
   	int number_of_keys, number_of_transactions;

	if(argc < 3)
	{
	  fprintf(stderr, "Usage: %s number_of_keys number_of_transactions\n", argv[0]);
	  exit(1);
	}

	number_of_keys = atoi(argv[1]);
	number_of_transactions = atoi(argv[2]);

    while(i < 2)
    {
        err = pthread_create(&(threads[i]), NULL, &checkThreads, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Thread created successfully\n");

        i++;
    }

    sleep(2);
    return 0;
}
