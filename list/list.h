#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

/** List structure (forward declaration). */
typedef unsigned long long uint64_t;

typedef struct s_element
{
  	uint64_t key;
  	void* data;
	uint64_t size;
  	struct s_element* next;
} keyval;

typedef struct s_list
{
  keyval *head;
  keyval *tail;
  int count;	//Number of current entries in linked list
} keyval_store;

int listCreate(keyval_store **list);
void listEmpty(keyval_store* list);
int listSet(keyval_store* list, uint64_t key, uint64_t size, void* data);
int listDelete(keyval_store *list, uint64_t key);
int listGet(keyval_store* list, uint64_t key, uint64_t *size, void *data);
void listPrint(keyval_store* list);
#endif


