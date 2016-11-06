#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main()
{
	keyval_store *my_kv = listCreate();
	uint64_t size[100], check_size;
	char * data = (char *)malloc(4096);
	char * check_data = (char *)malloc(4096);
	int i,j;

	//Allocate some random in data
	for(i=0; i<4096; i++)
	{
		data[i] = rand() % 256;
	}

	//Set
	for(i=0; i<100; i++)
	{
		size[i] = rand() % 4096;
		listSet(my_kv, i, size[i], (void *)data);
	}
	
	//Get
	for(i=0; i<100; i++)
	{
		listGet(my_kv, i, &check_size, (void *)check_data);
		if(check_size != size[i])
		{
			printf("Error at size at %d\n",i);
		}
		else
		{
			int j;
			for(j=0; j<check_size; j++)
			{
				if(check_data[j] != data[j])
					printf("Error in data at %d\n",i);
			}
		}
	}

	//Delete

	listEmpty(my_kv);
}
