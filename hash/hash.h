#ifndef HASH_H_INCLUDED
#define HASH_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#define SIZE 1024

typedef unsigned long long uint64_t;

//Struct to define each node in linked list
typedef struct s_element {
	uint64_t key;
	void* data;
	uint64_t size;
  	struct s_element* next;
} keyval;

//Struct stored at each hash table entry.
typedef struct s_list {
	keyval *head;
  	keyval *tail;
  	int count;		//Number of entries currently stored at this hash table index.
} keyval_store;

typedef struct s_hashmap {
 	keyval_store* table;
 	int table_size;
 	long count;		//Total number of entries in hash map
} hashmap;

hashmap* hashCreate(void);
unsigned int getHashIndex(hashmap *map, unsigned int key);
int hashSet(hashmap *map, uint64_t key, uint64_t size, void* data);
int hashGet(hashmap *map, uint64_t key, uint64_t *size, void *data);
int hashDelete(hashmap *map, uint64_t key);
void hashEmpty(hashmap *map);
void hashPrint(hashmap* map);

#endif /*HASH_H_INCLUDED*/
