// Purpose: Implementations for the functions declared in hashtable.h

#include "utils.h"
#include "hashtable_impl.h"


#define MAX_STRING_SIZE	256
// #define HMAX 99999

// #define DEBUG

linked_list_t *ll_create(unsigned int data_size)
{
    linked_list_t* ll;

    ll = malloc(sizeof(*ll));

    ll->head = NULL;
    ll->data_size = data_size;
    ll->size = 0;

    return ll;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Daca n >= nr_noduri, noul nod se adauga la finalul listei. Daca
 * n < 0, eroare.
 */
void ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *prev, *curr;
    ll_node_t* new_node;

	// new_node->next = NULL;

    if (!list) {
        return;
    }

    /* n >= list->size inseamna adaugarea unui nou nod la finalul listei. */
    if (n > list->size) {
        n = list->size;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    new_node = malloc(sizeof(*new_node));
    new_node->data = malloc(list->data_size);
    memcpy(new_node->data, new_data, sizeof(info));

    new_node->next = curr;
    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = new_node;
    } else {
        prev->next = new_node;
    }

    list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Daca n >= nr_noduri - 1, se elimina nodul de
 * la finalul listei. Daca n < 0, eroare. Functia intoarce un pointer spre acest
 * nod proaspat eliminat din lista. Este responsabilitatea apelantului sa
 * elibereze memoria acestui nod.
 */
void ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *prev, *curr;

    if (!list || !list->head) {
        return;
    }

    /* n >= list->size - 1 inseamna eliminarea nodului de la finalul listei. */
    if (n > list->size - 1) {
        n = list->size - 1;
    }

    curr = list->head;
    prev = NULL;
    while (n > 0) {
        prev = curr;
        curr = curr->next;
        --n;
    }

    if (prev == NULL) {
        /* Adica n == 0. */
        list->head = curr->next;
    } else {
        prev->next = curr->next;
    }

    list->size--;

	info *info_key;
	info_key = (info *)curr->data;
	free(info_key->key);
	free(info_key->value);
	free(info_key);
	free(curr);
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int ll_get_size(linked_list_t* list)
{
     if (!list) {
        return -1;
    }

    return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista si actualizeaza la
 * NULL valoarea pointerului la care pointeaza argumentul (argumentul este un
 * pointer la un pointer).
 */
void ll_free(linked_list_t** pp_list)
{
    // ll_node_t* currNode;

    if (!pp_list || !*pp_list) {
        return;
    }

    while (ll_get_size(*pp_list) > 0)
        ll_remove_nth_node(*pp_list, 0);

    free(*pp_list);
    *pp_list = NULL;
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza int-uri. Functia afiseaza toate valorile int stocate in nodurile
 * din lista inlantuita separate printr-un spatiu.
 */
void ll_print_int(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    curr = list->head;
    while (curr != NULL) {
        printf("%d ", *((int*)curr->data));
        curr = curr->next;
    }

    printf("\n");
}

/*
 * Atentie! Aceasta functie poate fi apelata doar pe liste ale caror noduri STIM
 * ca stocheaza string-uri. Functia afiseaza toate string-urile stocate in
 * nodurile din lista inlantuita, separate printr-un spatiu.
 */
void ll_print_string(linked_list_t* list)
{
    ll_node_t* curr;

    if (!list) {
        return;
    }

    // printf("print string\n");

    curr = list->head;
    while (curr != NULL) {
        printf("%s ", (char*)((info *)(curr->data))->value);
        curr = curr->next;
    }

    printf("\n");
}

/*
 * Functii de comparare a cheilor:
 */
// int compare_function_ints(void *a, void *b)
// {
// 	int int_a = *((int *)a);
// 	int int_b = *((int *)b);

// 	if (int_a == int_b) {
// 		return 0;
// 	} else if (int_a < int_b) {
// 		return -1;
// 	} else {
// 		return 1;
// 	}
// }

int compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functii de hashing:
 */
// unsigned int hash_function_int(void *a)
// {
// 	/*
// 	 * Credits: https://stackoverflow.com/a/12996028/7883884
// 	 */
// 	unsigned int uint_a = *((unsigned int *)a);

// 	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
// 	uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
// 	uint_a = (uint_a >> 16u) ^ uint_a;
// 	return uint_a;
// }

// unsigned int hash_function_string(void *a)
// {
// 	/*
// 	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
// 	 */
// 	unsigned char *puchar_a = (unsigned char*) a;
// 	unsigned long hash = 5381;
// 	int c;

// 	while ((c = *puchar_a++))
// 		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

// 	return hash;
// }

// /*
//  * Functie apelata pentru a elibera memoria ocupata de cheia si valoarea unei
//  * perechi din hashtable. Daca cheia sau valoarea contin tipuri de date complexe
//  * aveti grija sa eliberati memoria luand in considerare acest aspect.
//  */
// void key_val_free_function(void *data) {
// 	/* TODO */
// }

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t *hash;
	hash = malloc(sizeof(*hash));
	DIE(!hash, "Malloc failed\n");

	hash->hash_function = hash_function;
	hash->compare_function = compare_function;
	// hash->key_val_free_function = key_val_free_function;

	hash->hmax = hmax;
	hash->buckets = malloc(hmax * sizeof(linked_list_t *));
	DIE(!hash->buckets, "Malloc failed\n");

	hash->size = 0;

	for (unsigned int i = 0; i < hmax; i++)
		hash->buckets[i] = ll_create(sizeof(info));

	return hash;
}

void *ht_get(hashtable_t *ht, void *key)
{
    unsigned int index = ht->hash_function(key) % ht->hmax;
    ll_node_t *curr = ht->buckets[index]->head;

    // printf("data: %s\n", (char *)((info *)(curr->data))->key);

    while (curr != NULL) {
        if (ht->compare_function(((info *)curr->data)->key, key) == 0) {

            // printf("aici:key: %s\n", (char *)((info *)(curr->data))->key);
            // printf("aici: value: %s\n", (char *)((info *)(curr->data))->value);


            return ((info *)(curr->data))->value;
        }
        curr = curr->next;
    }

    return NULL;
}

/*
 * Functie care intoarce:
 * 1, daca pentru cheia key a fost asociata anterior o valoare in hashtable
 * folosind functia put;
 * 0, altfel.
 */
int ht_has_key(hashtable_t *ht, void *key)
{
	if (ht_get(ht, key))
		return 1;
	else
		return 0;
}

/*
 * Atentie! Desi cheia este trimisa ca un void pointer (deoarece nu se impune
 * tipul ei), in momentul in care se creeaza o noua intrare in hashtable (in
 * cazul in care cheia nu se gaseste deja in ht), trebuie creata o copie a
 * valorii la care pointeaza key si adresa acestei copii trebuie salvata in
 * structura info asociata intrarii din ht. Pentru a sti cati octeti trebuie
 * alocati si copiati, folositi parametrul key_size.
 *
 * Motivatie:
 * Este nevoie sa copiem valoarea la care pointeaza key deoarece dupa un apel
 * put(ht, key_actual, value_actual), valoarea la care pointeaza key_actual
 * poate fi alterata (de ex: *key_actual++). Daca am folosi direct adresa
 * pointerului key_actual, practic s-ar modifica din afara hashtable-ului cheia
 * unei intrari din hashtable. Nu ne dorim acest lucru, fiindca exista riscul sa
 * ajungem in situatia in care nu mai stim la ce cheie este inregistrata o
 * anumita valoare.
 */

// functia mea de hash put
void ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	int hash = ht->hash_function(key);
	hash %= ht->hmax;

	info *info_key;
	info_key = malloc(sizeof(*info_key));

	info_key->key = malloc(key_size);
	memcpy(info_key->key, key, key_size);

	info_key->value = malloc(value_size);
	memcpy(info_key->value, value, value_size);


#ifdef DEBUG
	printf("info :%s\n", (char*)info_key->key);

	printf("value :%s\n", (char*)info_key->value);

	printf("hash: %d\n", hash);
#endif

    // printf("ono: key: %d\n", *(int*)info_key->key);
    // printf("dos: value: %s\n", (char*)info_key->value);

	ll_add_nth_node(ht->buckets[hash],ht->buckets[hash]->size, info_key);
    // ll_print_string(ht->buckets[hash]);

	free(info_key);
}

// functie chat gpt de put

/*
 * Procedura care elimina din hashtable intrarea asociata cheii key.
 * Atentie! Trebuie avuta grija la eliberarea intregii memorii folosite pentru o
 * intrare din hashtable (adica memoria pentru copia lui key --vezi observatia
 * de la procedura put--, pentru structura info si pentru structura Node din
 * lista inlantuita).
 */
void ht_remove_entry(hashtable_t *ht, void *key)
{
	int hash = ht->hash_function(key);
	hash %= ht->hmax;

	while (ht->buckets[hash]->size)
		ll_remove_nth_node(ht->buckets[hash], 0);
}

/*
 * Procedura care elibereaza memoria folosita de toate intrarile din hashtable,
 * dupa care elibereaza si memoria folosita pentru a stoca structura hashtable.
 */
void ht_free(hashtable_t *ht)
{
    for (unsigned int i = 0; i < ht->hmax; i++)
		ll_free(&ht->buckets[i]);

	for (unsigned int i = 0; i < ht->hmax; i++)
		free(ht->buckets[i]);
	free(ht->buckets);

	free(ht);
}

unsigned int ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}

