#include "hash.h"

hashmap* hashCreate(void)
{
	int i;
	hashmap* map;
	map = (hashmap *)malloc(sizeof(hashmap));
	if(map == NULL)
	{
		return NULL;
	}

	map->table = (keyval_store *)malloc(sizeof(keyval_store)*SIZE);
	if(map->table == NULL)
	{
		return NULL;
	}
	map->table_size = SIZE;
	map->count = 0;

	//Initialize all entries in hash table
	for(i=0; i<SIZE; i++)
	{
		map->table[i].head = NULL;
		map->table[i].tail = NULL;
		map->table[i].count = 0;
	}

	return map;

}

unsigned int getHashIndex(hashmap *map, unsigned int key)
{
	/* Robert Jenkins' 32 bit Mix Function */
	key += (key << 12);
	key ^= (key >> 22);
	key += (key << 4);
	key ^= (key >> 9);
	key += (key << 10);
	key ^= (key >> 2);
	key += (key << 7);
	key ^= (key >> 12);

	/* Knuth's Multiplicative Method */
	key = (key >> 3) * 2654435761;

	return (key % map->table_size);
}

int hashGet(hashmap *map, uint64_t key, uint64_t *size, void *data)
{
	unsigned int index;
	unsigned int key_hash = (unsigned int)key;
	char *out_data = (char *)data;
	char *my_data;
	
	index = getHashIndex(map, key_hash);	

	//Search at 'index' in the hash table
	if(map->table[index].head == NULL)
	{
		return -1;
	}
	else
	{
		//Traverse linked list
		keyval *curr = map->table[index].head;

		while(curr != NULL)
		{
			if(curr->key == key)
			{
				my_data = (char *)curr->data;
				*size = curr->size;
				unsigned int i;
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
}

int hashSet(hashmap *map, uint64_t key, uint64_t size, void* data)
{
	unsigned int index;
	unsigned int key_hash = (unsigned int)key;
	char *new_data = NULL;
	unsigned int i;
	char *in_data = (char *)data;
	keyval *curr;
	
	keyval *node = NULL;
	
	node = (keyval *)malloc(sizeof(keyval));
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

	index = getHashIndex(map, key_hash);

	//Insert data at 'index' location in the hash table
	if(map->table[index].head == NULL)
	{
		map->table[index].head = node;
		map->table[index].tail = node;
	}
	else
	{
		curr = map->table[index].head;
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

		(map->table[index].tail)->next = node;
		map->table[index].tail = node;
	}
	map->table[index].count++;

	return 0;
}

int hashDelete(hashmap *map, uint64_t key)
{
	unsigned int index;
	unsigned int key_hash = (unsigned int)key;
	keyval *curr = NULL;
	keyval *prev = NULL;

	index = getHashIndex(map, key_hash);

	if(map->table[index].head == NULL)
	{
		return -1;
	}

	curr = map->table[index].head;

	while(curr != NULL)
	{
		if (curr->key == key)
		{
			//Remove node

			//1.Head
			if(curr == map->table[index].head)
			{
				map->table[index].head = (map->table[index].head)->next;
				if(curr == map->table[index].tail)
				{
					map->table[index].tail = NULL;
					//Only one entry, so head and tail both become NULL;
					//assert(map->table[index].tail == NULL);
				}
			}
			//2. Middle
			else
			{
				//3. Tail
				if(curr == map->table[index].tail)
				{	
					prev->next = NULL;
					map->table[index].tail = prev;
				}
				else
				{
					prev->next = curr->next;
				}
			}
			free(curr->data);
			free(curr);
			map->table[index].count--;
			map->count--;
			return 0;
		}
		prev = curr;
		curr = curr->next;
	}

	return -1;
}

void hashEmpty(hashmap *map)
{
	//Delete the entire hashmap
	keyval *curr;
	int i;

	for(i=0; i<(map->table_size); i++)
	{
		curr = map->table[i].head;
		keyval *temp;
		while(curr != NULL)
		{
			temp = curr;
			curr = curr->next;
			free(temp->data);
			free(temp);
		}
		map->table[i].head = NULL;
		map->table[i].tail = NULL;
		map->table[i].count = 0;

	}

	free(map->table);
	map->count = 0;
}

void hashPrint(hashmap* map)
{
    keyval *curr;
    unsigned int i;
    char *data;

    //Print all entries in hashmap
    for(i=0; i<(map->table_size); i++)
    {
        curr = map->table[i].head;
        printf("Table entry %d:  ", i);
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
    }

    return;
}


