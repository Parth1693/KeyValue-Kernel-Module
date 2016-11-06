#include "hash.h"

int main(int argc, char **argv)
{
	hashmap *my_hash = NULL;
	uint64_t size[100], check_size;
	char * data = (char *)malloc(4096);
	char * check_data = (char *)malloc(4096);
	unsigned int i,j;
	int test = 1;

	//Create
	my_hash = hashCreate();
	if(my_hash == NULL)
		printf("Unable to create hashmap\n");

	//Allocate some random data
	for(i=0; i<4096; i++)
	{
		data[i] = rand() % 256;
	}

	//Set (insert data in hashmap)
	for(i=0; i<100; i++)
	{
		size[i] = rand() % 4096;
		hashSet(my_hash, i, size[i], (void *)data);
	}

	//Allocate some random data
	for(i=0; i<4096; i++)
	{
		data[i] = rand() % 256;
	}

	//Set (insert data in hashmap)
	for(i=0; i<100; i++)
	{
		size[i] = rand() % 4096;
		hashSet(my_hash, i, size[i], (void *)data);
	}
		
	//Get
	for(i=0; i<100; i++)
	{
		hashGet(my_hash, i, &check_size, (void *)check_data);
		if(check_size != size[i])
		{
			printf("Error at size at %d\n",i);
			test = 0;
			break;
		}
		else
		{
			for(j=0; j<check_size; j++)
			{
				if(check_data[j] != data[j])
				{
					printf("Error in data at %d\n",i);
					test = 0;
				}
			}	
		}
	}

	//Delete
	hashDelete(my_hash, 50);

	if(hashGet(my_hash, 50, &check_size, (void *)check_data) == -1)
		printf("Success delete\n");

	if(test == 1)
		printf("Success\n");
	
	//Clear hash map
	hashEmpty(my_hash);

	return 0;
}
