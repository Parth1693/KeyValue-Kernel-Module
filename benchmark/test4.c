//Single threaded test case to test error conditions
//Key not found, data request for more than 4KB 

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
	__u64 check_size;
	char * data = (char *)malloc(8092);
	char * check_data = (char *)malloc(8092);
	int devfd;      //keyvalue device file descriptor

	if(argc < 3)
	{
	  fprintf(stderr, "Usage: %s number_of_keys number_of_transactions\n", argv[0]);
	  exit(1);
	}

	number_of_keys = atoi(argv[1]);
	number_of_transactions = atoi(argv[2]);
	__u64 size[1000];
	__u64 keys[1000];
	
	devfd = open("/dev/keyvalue", O_RDWR);

	if(devfd < 0)
	{
	  fprintf(stderr, "Device open failed");
	  exit(1);
	}

	srand((int)time(NULL)+(int)getpid());

	//1. Test for key not found
	for(i=0; i<8092; i++)
	{
		data[i] = rand() % 256;
	}

	//Put keys and values into key_value store
	for(i=0; i<100; i++)
	{
		size[i] = rand() % 4096;
		tid = kv_set(devfd, i, size[i], (void *)data);
		assert(tid != -1);
		fprintf(stderr, "SET\t%d\t%d\t%d\n", tid, i, size[i]);
	}

	for(i=0; i<100; i++)
	{
		a = (rand() % 100) + 101;
		tid = kv_get(devfd, a, &check_size, (void *)check_data);
		if(tid != -1)
		{
			fprintf(stderr, "ERROR - Key found at %d!!\n", a);
		}
	}

	//2.Test for trying to put data more than 4KB
	tid = kv_set(devfd, 100, 5000, (void *)data);
	if(tid != -1)
	{
		fprintf(stderr, "ERROR - Data size!!\n");
	}

	tid = kv_set(devfd, 102, 4800, (void *)data);
	if(tid != -1)
	{
		fprintf(stderr, "ERROR - Data size!!\n");
	}

	//3.Test for hashmap collision - Map size = 128
	size[126] = rand() % 4096;
	tid = kv_set(devfd, 126, size[126], (void *)data);
	assert(tid != -1);

	size[254] = rand() % 4096;
	tid = kv_set(devfd, 254, size[254], (void *)data);
	assert(tid != -1);
	
	size[382] = rand() % 4096;
	tid = kv_set(devfd, 382, size[382], (void *)data);
	assert(tid != -1);

	tid = kv_get(devfd, 254, &check_size, (void *)check_data);
	if(tid == -1)
	{
		printf("ERORR!!\n");	
	}	
	else
	{
		if(check_size != size[254])
		{
			printf("ERROR in size!!\n");
			test = 0;	
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
	

	tid = kv_get(devfd, 382, &check_size, (void *)check_data);
	if(tid == -1)
		printf("ERORR!!\n");
	else
	{
		if(check_size != size[382])
		{
			printf("ERROR in size!!\n");
			test = 0;	
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

	tid = kv_get(devfd, 126, &check_size, (void *)check_data);
	if(tid == -1)
		printf("ERORR!!\n");
	else
	{
		if(check_size != size[126])
		{
			printf("ERROR in size!!\n");
			test = 0;	
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

	tid = kv_delete(devfd, 254);
	if(tid == -1)
	{
		printf("ERROR in delete!\n");
		test = 0;
	}

	tid = kv_get(devfd, 254, &check_size, (void *)check_data);
	if(tid != -1)
	{
		printf("ERORR!!\n");
		test = 0;
	}

	
	tid = kv_get(devfd, 382, &check_size, (void *)check_data);
	if(tid == -1)
		printf("ERORR!!\n");
	else
	{
		if(check_size != size[382])
		{
			printf("ERROR in size!!\n");
			test = 0;	
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

	if(test == 1)
		printf("Test passed!!\n");
	else
		printf("Test failed!!\n");

	return 1;

}