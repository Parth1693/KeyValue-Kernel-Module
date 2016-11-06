//////////////////////////////////////////////////////////////////////
//                       North Carolina State University
//
//
//
//                             Copyright 2016
//
////////////////////////////////////////////////////////////////////////
//
//   Group Members:
//   Parth Bhogate      Unity ID: pbhogat
//   Radhika Sakhare    Unity ID: rssakhar
//  
//   Description:
//     CSC 501 Project 2 - Key Value Pseudo Device
//
//   Date: 11/2/2016
//
////////////////////////////////////////////////////////////////////////

#include "keyvalue.h"

#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/poll.h>
#include <linux/semaphore.h>

#define SIZE 256

unsigned transaction_id;
struct semaphore lock;

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
    int count;      //Number of entries currently stored at this hash table index.
} keyval_store;

typedef struct s_hashmap {
    keyval_store* table;
    int table_size;
    long count;     //Total number of entries in hash map
} hashmap;

hashmap *my_hash;

//Forward declaration of list API
static hashmap* hashCreate(void);
static unsigned int getHashIndex(hashmap *map, unsigned int key);
static int hashSet(hashmap *map, uint64_t key, uint64_t size, void* data);
static int hashGet(hashmap *map, uint64_t key, uint64_t *size, void *data);
static int hashDelete(hashmap *map, uint64_t key);
static void hashEmpty(hashmap *map);
static void hashPrint(hashmap* map);

static void free_callback(void *data)
{

}

static long keyvalue_get(struct keyvalue_get __user *ukv)
{
    int ret;

    struct keyvalue_get kv;
    kv.key = ukv->key;
    kv.size = ukv->size;
    kv.data = ukv->data;

    printk("Keyvalue GET called\n");
		
    //Obtain lock
    down_interruptible(&lock);

    ret = hashGet(my_hash, kv.key, kv.size, kv.data);
    
    if (ret != -1)
    {
        ret = transaction_id;
        transaction_id++;
    }

    //Release lock
    up(&lock);

    return ret;
}

static long keyvalue_set(struct keyvalue_set __user *ukv)
{
    int ret;

    struct keyvalue_set kv;
    kv.key = ukv->key;
    kv.size = ukv->size;
    kv.data = ukv->data;

    printk("Keyvalue SET called\n");
    //hashPrint(my_hash);	    
		
    //Obtain lock
    down_interruptible(&lock);

    ret = hashSet(my_hash, kv.key, kv.size, kv.data);

    if (ret != -1)
    {
        ret = transaction_id;
        transaction_id++;
    }

    //Release lock
    up(&lock);

    return ret;
}

static long keyvalue_delete(struct keyvalue_delete __user *ukv)
{
    int ret;

    struct keyvalue_delete kv;
    kv.key = ukv->key;

    //Obtain lock
    down_interruptible(&lock);

    ret = hashDelete(my_hash, kv.key);

    if (ret != -1)
    {
        ret = transaction_id;
        transaction_id++;
    }

    //Release lock
    up(&lock);

    return ret;
}
     
unsigned int keyvalue_poll(struct file *filp, struct poll_table_struct *wait)
{
    unsigned int mask = 0;
    printk("keyvalue_poll called. Process queued\n");
    return mask;
}

static long keyvalue_ioctl(struct file *filp, unsigned int cmd,
                                unsigned long arg)
{
    switch (cmd) {
    case KEYVALUE_IOCTL_GET:
        return keyvalue_get((void __user *) arg);
    case KEYVALUE_IOCTL_SET:
        return keyvalue_set((void __user *) arg);
    case KEYVALUE_IOCTL_DELETE:
        return keyvalue_delete((void __user *) arg);
    default:
        return -ENOTTY;
    }
}

static int keyvalue_mmap(struct file *filp, struct vm_area_struct *vma)
{
    return 0;
}

static const struct file_operations keyvalue_fops = {
    .owner                = THIS_MODULE,
    .unlocked_ioctl       = keyvalue_ioctl,
    .mmap                 = keyvalue_mmap,
//    .poll		  = keyvalue_poll,
};

static struct miscdevice keyvalue_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "keyvalue",
    .fops = &keyvalue_fops,
};

static int __init keyvalue_init(void)
{
    int ret;
    my_hash = NULL;

    sema_init(&lock, 1);

    my_hash = hashCreate();
    if(my_hash == NULL)
        printk("Unable to create hashmap\n");

    if ((ret = misc_register(&keyvalue_dev)))
        printk(KERN_ERR "Unable to register \"keyvalue\" misc device\n");
    return ret;
}

static void __exit keyvalue_exit(void)
{
    //hashPrint(my_hash);
    hashEmpty(my_hash);
    misc_deregister(&keyvalue_dev);
}

//Hashmap functions
static hashmap* hashCreate(void)
{
    int i;
    hashmap* map;
    map = (hashmap *)kmalloc(sizeof(hashmap), GFP_KERNEL);
    if(map == NULL)
    {
        return NULL;
    }

    map->table = (keyval_store *)kmalloc(sizeof(keyval_store)*SIZE, GFP_KERNEL);
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

static unsigned int getHashIndex(hashmap *map, unsigned int key)
{
    return (key % map->table_size);
}

static int hashGet(hashmap *map, uint64_t key, uint64_t *size, void *data)
{
    unsigned int index;
    unsigned int key_hash = (unsigned int)key;
    char *out_data = (char *)data;
    char *my_data;
    keyval *curr;
    unsigned long copy;
    
    index = getHashIndex(map, key_hash);    

    //Search at 'index' in the hash table
    if(map->table[index].head == NULL)
    {
        return -1;
    }
    else
    {
        //Traverse linked list
        curr = map->table[index].head;

        while(curr != NULL)
        {
            if(curr->key == key)
            {
                my_data = (char *)curr->data;
                *size = curr->size;
                
                /*
                for(i=0; i<(*size); i++)
                {   
                    out_data[i] = my_data[i];
                }
                */
                copy = copy_to_user(out_data, my_data, (*size));
                if(copy != 0)
                    printk("Data copy failed!!\n");
                
                return 0;
            }
            curr = curr->next;
        }
        return -1;
    }
}

static int hashSet(hashmap *map, uint64_t key, uint64_t size, void* data)
{
    unsigned int index;
    unsigned int key_hash = (unsigned int)key;
    char *new_data = NULL;
    unsigned long copy;
    keyval *curr;
    
    keyval *node = NULL;
    
    node = (keyval *)kmalloc(sizeof(keyval), GFP_KERNEL);
    if(node == NULL)
    {
        printk("Couldn't allocate new node on heap\n");
        return -1;
    }

    //Check size <= 4096
    if(size>4096)
    {
        printk("Maximum data size is 4KB\n");
        return -1;
    }

    new_data = (char *)kmalloc(size, GFP_KERNEL);
    if(new_data == NULL)
    {
        printk("Couldn't allocate new data on heap\n");
        return -1;
    }

    node->key = key;
    node->size = size;
    node->next = NULL;
    /*
    for(i=0; i<size; i++)
    {
        new_data[i] = in_data[i];
    }
    */
    copy = copy_from_user(new_data, data, size); 
    if(copy != 0)
        printk("Data copy failed!!\n"); 

    node->data = (void *)new_data;

    //Insert data at 'index' location in the hash table
    index = getHashIndex(map, key_hash);
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
                kfree(curr->data);
                curr->data = (void *)new_data;
                curr->size = size;
                kfree(node);
                return 0;               
            }
            curr = curr->next;
        }

        (map->table[index].tail)->next = node;
        map->table[index].tail = node;
    }

    map->table[index].count++;
    map->count++;

    return 0;
}

static int hashDelete(hashmap *map, uint64_t key)
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
            kfree(curr->data);
            kfree(curr);
            map->table[index].count--;
            map->count--;
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }

    return -1;
}

static void hashEmpty(hashmap *map)
{
    //Delete the entire hashmap
    keyval *curr;
    keyval *temp;
    int i;

    for(i=0; i<(map->table_size); i++)
    {
        curr = map->table[i].head;        
        while(curr != NULL)
        {
            temp = curr;
            curr = curr->next;
            kfree(temp->data);
            kfree(temp);
        }
        map->table[i].head = NULL;
        map->table[i].tail = NULL;
        map->table[i].count = 0;

    }

    kfree(map->table);
    map->count = 0;

    return;
}

static void hashPrint(hashmap* map)
{
    keyval *curr;
    int i;
    char *data;

    //Print all entries in hashmap
    for(i=0; i<(map->table_size); i++)
    {
        curr = map->table[i].head;
        printk("Table entry %d:  ", i);
        while(curr != NULL)
        {
            data = (char *)curr->data;
            printk("Key:%lld\t", curr->key);
            printk("Value:");
            for(i=0; i<(curr->size); i++)
            {
                printk("%c", data[i]);     
            }
            printk("\n");

            curr = curr->next;
        }
    }

    return;
}

MODULE_AUTHOR("Parth Bhogate <pbhogat@ncsu.edu>");
MODULE_AUTHOR("Radhika Sakhare <rssakhar@ncsu.edu>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
module_init(keyvalue_init);
module_exit(keyvalue_exit);
