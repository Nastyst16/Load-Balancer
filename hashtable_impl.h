// Copyright 2023 Nastase Cristian-Gabriel 315CA

#include "utils.h"

#ifndef HASHTABLE_IMPL_H_
#define HASHTABLE_IMPL_H_

#define MAX_STRING_SIZE	256
#define HMAX 32

struct ll_node_t;
typedef struct ll_node_t ll_node_t;

struct ll_node_t
{
    void* data;
    struct ll_node_t* next;
};

typedef struct linked_list_t
{
    ll_node_t* head;
    unsigned int data_size;
    unsigned int size;
} linked_list_t;

typedef struct info info;
struct info {
	void *key;
	void *value;
};


linked_list_t *ll_create(unsigned int data_size);
void ll_add_nth_node(linked_list_t *list, unsigned int n, const void *new_data);
void ll_remove_nth_node(linked_list_t* list, unsigned int n);
unsigned int ll_get_size(linked_list_t* list);
void ll_free(linked_list_t** pp_list);

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets; /* Array de liste simplu-inlantuite. */
	/* Nr. total de noduri existente curent in toate bucket-urile. */
	unsigned int size;
	unsigned int hmax; /* Nr. de bucket-uri. */
	/* (Pointer la) Functie pentru a calcula valoarea hash asociata cheilor. */
	unsigned int (*hash_function)(void*);
	/* (Pointer la) Functie pentru a compara doua chei. */
	int (*compare_function)(void*, void*);
};

int compare_function_strings(void *a, void *b);
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*));
void *ht_get(hashtable_t *ht, void *key);
int ht_has_key(hashtable_t *ht, void *key);
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);
void ht_remove_entry(hashtable_t *ht, void *key);
void ht_free(hashtable_t *ht);
unsigned int ht_get_size(hashtable_t *ht);
unsigned int ht_get_hmax(hashtable_t *ht);

#endif  // HASHTABLE_IMPL_H_
