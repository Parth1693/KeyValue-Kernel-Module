#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int listCreate(keyval_store **kv_store)
{
  *kv_store = (keyval_store*)malloc(sizeof(keyval_store));

  if(kv_store == NULL)
  {
  	return -1; 
  }
  (*kv_store)->head = NULL;
  (*kv_store)->tail = NULL;
  (*kv_store)->count = 0;
  
  return 0;

}

void listEmpty(keyval_store* list)
{
	keyval *curr = list->head;
  	
	while (curr != NULL)
  	{
		keyval *temp = curr;
		curr = curr->next;
		free(temp->data);
    		free(temp);
  	}

	list->head = NULL;
	list->tail = NULL;
	list->count = 0;
}


int listSet(keyval_store* list, uint64_t key, uint64_t size, void* data)
{
  	keyval* node = NULL;
	char *new_data = NULL;
	int i;
	char *in_data = (char *)data;
	keyval *curr = NULL;

	node = (keyval*)malloc(sizeof(keyval));
	if(node == NULL)
	{
		printf("Couldn't allocate node on heap\n");
		return -1;
	}

	new_data = (char *)malloc(size);
	if(new_data == NULL)
	{
		printf("Couldn't allocate new data on heap\n");
		return -1;
	}

  	node->key = key;
	node->size = size;
  	node->next = NULL;
	for(i=0; i<size; i++)
	{
		new_data[i] = in_data[i];
	}
	node->data = (void *)new_data;

	if(list->head == NULL)
	{	
		list->head = node;
		list->tail = node;
	} 	
	else
	{
		curr = list->head;
		while(curr != NULL)
		{
			if(curr->key == key)
			{
				//Replace
				free(curr->data);
				curr->data = (void *)new_data;
				curr->size = size;
				free(node);
				return 0;
			}
			curr = curr->next;
		}
		list->tail->next = node;
		list->tail = node;
	}		
	list->count++;
	return 0;
}

int listDelete(keyval_store *list, uint64_t key)
{
	keyval *curr = NULL;
	keyval *prev = NULL;
	
	if( list->head == NULL)
		return -1;

	curr = list->head;
	
	while(curr != NULL)
	{
		if ( curr->key == key )
		{
			//Remove node				

			//1. Head
			if (curr == list->head)
			{
				if(list->head == list->tail)
				{
					list->tail = NULL;
				}
				list->head = curr->next;
			}
			//2. Middle
			else
			{
				//3. Tail
				if(curr == list->tail)
				{
					prev->next = NULL;
					list->tail = prev;					
				}
				else
				{
					prev->next = curr->next;
				}
			}
			free(curr->data);
			free(curr);
			list->count--;
			return 0;
		}
		prev = curr;
		curr = curr->next;
	}
	return -1;		
}

int listGet(keyval_store* list, uint64_t key, uint64_t *size, void *data)
{
	keyval *curr = list->head;
	int i;
	char *out_data = (char *)data;
	char *my_data;

	while(curr != NULL)
	{
		if(curr->key == key)
		{
			my_data = (char *)curr->data;
			*size = curr->size;
			for(i=0; i<(*size); i++)
			{
				out_data[i] = my_data[i];
			}
			return 0;
		}
		curr = curr->next;
	}
	return -1;
}

void listPrint(keyval_store* list)
{
    keyval *curr = list->head;
    unsigned int i;
    char *data;

    //Print all entries in list
    while(curr != NULL)
    {
        data = (char *)curr->data;
        printf("Key:%lld\t", curr->key);
        printf("Value:");
        for(i=0; i<(curr->size); i++)
        {
            printf("%c", data[i]);     
        }
        printf("\n");

        curr = curr->next;
    }

    return;
}

int main()
{
	keyval_store *my_kv = NULL;
	uint64_t size[100], check_size;
	char * data = (char *)malloc(4096);
	char * check_data = (char *)malloc(4096);
	int i,j;
	int test = 1;

	//Create
	if(listCreate(&my_kv) == -1)
		printf("Unable to create kv_store\n");

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
	listDelete(my_kv, 50);

	if(listGet(my_kv, 50, &check_size, (void *)check_data) == -1)
		printf("Success delete\n");

	if(test == 1)
		printf("Sucess\n");


	listEmpty(my_kv);

	return 0;
}
