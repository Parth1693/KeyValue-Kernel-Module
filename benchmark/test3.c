//Multi-process test case
//Test set, get and delete for multiple process accessing the key value store 

#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <keyvalue.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <assert.h>

int main(int argc, char *argv[])
{
	int number_of_keys, number_of_transactions; 
	pid_t pid;
	int test = 1;
	__u64 *size;
	__u64 *keys;
	__u64 check_size;
	char * data;
	char * check_data;
	int devfd;      //keyvalue device file descriptor
	int i, j, tid, status;

	if(argc < 3)
	{
	  fprintf(stderr, "Usage: %s number_of_keys number_of_transactions\n", argv[0]);
	  exit(1);
	}

	devfd = open("/dev/keyvalue", O_RDWR);
	if(devfd < 0)
	{
	  fprintf(stderr, "Device open failed");
	  exit(1);
	}

	number_of_keys = atoi(argv[1]);
	number_of_transactions = atoi(argv[2]);

	//Heap allocation
	size = (__u64 *)malloc(number_of_transactions * sizeof(__u64));
	keys = (__u64 *)malloc(number_of_transactions * sizeof(__u64));
	data = (char *)malloc(4096);
	check_data = (char *)malloc(4096);

	//Allocate some random data
	for(i=0; i<4096; i++)
	{
		data[i] = rand() % 256;
	}

	for(i = 0; i < number_of_transactions; i++)
	{
		keys[i] = i;
		size[i] = rand() % 4096;
	}

	/*************************************************************************************************************/
	pid = fork();
	if(pid == 0) //child
	{
		srand((int)time(NULL)+(int)getpid());

		for(i = 0; i < number_of_transactions>>1; i++)
		{
			tid = kv_set(devfd, keys[i], size[i], (void *)data);
			assert(tid != -1);
		}
	}
	/*************************************************************************************************************/
	else //parent
	{
		srand((int)time(NULL)+(int)getpid());

		for(i = number_of_transactions>>1; i < number_of_transactions; i++)
		{
			tid = kv_set(devfd, keys[i], size[i], (void *)data);
			assert(tid != -1);
		}

		waitpid(pid, &status, 0);	

		for(i = 0; i<number_of_transactions; i++)
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
		}
		if(test == 1)
			printf("Process test successful!\n");

	}
	/*************************************************************************************************************/

	free(size);
	free(keys);
	free(data);
	free(check_data);

	return 0;
}
