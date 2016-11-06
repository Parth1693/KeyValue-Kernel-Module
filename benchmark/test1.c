//Basic single threaded test case
//Test set, get and delete for a single process

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <keyvalue.h>
#include <fcntl.h>
#include <assert.h>

int main(int argc, char *argv[])
{
	int i = 0, number_of_threads = 1, number_of_keys = 1024, number_of_transactions = 65536; 
	int k, j;
	int a;
	int tid;
	int test = 1;
	__u64 size[number_of_transactions];
	__u64 keys[number_of_transactions];
	__u64 check_size;
	char * data = (char *)malloc(4096);
	char * check_data = (char *)malloc(4096);
	int devfd;      //keyvalue device file descriptor

	if(argc < 3)
	{
	  fprintf(stderr, "Usage: %s number_of_keys number_of_transactions\n", argv[0]);
	  exit(1);
	}

	number_of_keys = atoi(argv[1]);
	number_of_transactions = atoi(argv[2]);
	devfd = open("/dev/keyvalue", O_RDWR);

	if(devfd < 0)
	{
	  fprintf(stderr, "Device open failed");
	  exit(1);
	}

	srand((int)time(NULL)+(int)getpid());

/*********************************************************************************************************/
    	//Allocate some random data
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
		assert(tid != -1);
		fprintf(stderr, "SET\t%llu\n", keys[i]);
		//fprintf(stderr,"S\t%d\t%d\t%d\t%d\n", tid, i, size[i], data);
	}

	fprintf(stderr, "\n\n");

	for(i = 0; i < number_of_transactions; i++)
	{
		tid = kv_get(devfd, keys[i], &check_size, (void *)check_data);
		assert(tid != -1);
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

		fprintf(stderr, "GET\t%llu\n", keys[i]);
		//fprintf(stderr,"G\t%d\t%d\t%s\n", tid, check_size, check_data);  
	}

	if(test == 1)
		printf("Success single set get!\n");

/*********************************************************************************************************/
	test = 1;

    	//Allocate some random new data
	for(i=0; i<4096; i++)
	{
		data[i] = rand() % 256;
	}

	// Initializing the key_value store again
	for(i = 0; i < number_of_transactions; i++)
	{
		size[i] = rand() % 4096;
		tid = kv_set(devfd, keys[i], size[i], (void *)data);
		assert(tid != -1);
		fprintf(stderr, "SET\t%llu\n", keys[i]);
		//fprintf(stderr,"S\t%d\t%d\t%d\t%d\n", tid, i, size[i], data);
	}

	fprintf(stderr, "\n\n");

	for(i = 0; i < number_of_transactions; i++)
	{
		tid = kv_get(devfd, keys[i], &check_size, (void *)check_data);
		assert(tid != -1);
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

		fprintf(stderr, "GET\t%llu\n", keys[i]);
		//fprintf(stderr,"G\t%d\t%d\t%s\n", tid, check_size, check_data);  
	}

	if(test == 1)
		printf("Success overwrite set get!\n");

/*********************************************************************************************************/

	test = 1;

	for(i = 0; i < number_of_transactions; i++)
	{
		tid = kv_delete(devfd, keys[i]);
		assert(tid != -1);

		tid = kv_get(devfd, keys[i], &check_size, (void *)check_data);
		if(tid != -1)
		{
			test = 0;
			printf("Error in delete %d at Key:%llu\n", i,keys[i]);
		}


	}

	if(test == 1)
		printf("Success delete and get!\n");

/*********************************************************************************************************/	
	close(devfd);
	return 0;
}
