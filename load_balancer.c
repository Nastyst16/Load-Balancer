// Copyright 2023 Nastase Cristian-Gabriel 315CA

#include "utils.h"
#include "load_balancer.h"
#include "server.h"

// #define DEBUG

// function made for qsort, for sorting
// the ring in asscending order
int ring_compare(const void *a, const void *b) {
    unsigned int a_val = *(unsigned int*) a;
    unsigned int b_val = *(unsigned int*) b;

    if (a_val == b_val) {
        return 0;
    } else if (a_val < b_val) {
        return -1;
    } else {
        return 1;
    }
}

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *)a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

// initialising the load_ballancer struct
load_balancer *init_load_balancer() {
    load_balancer *main = malloc(sizeof(load_balancer));
    DIE(main == NULL, "Ballancer alloc failed\n");

    main->servers = NULL;
    main->hashring = NULL;
    main->servers_count = 0;

    return main;
}

// function made for qsort, for sorting the
// servers in asscending order, by their id-s
int compare_servers(const void *s1, const void *s2) {
    const struct server_memory *server1 = *(const struct server_memory **)s1;
    const struct server_memory *server2 = *(const struct server_memory **)s2;

    if (server1->server_key < server2->server_key) {
        return -1;
    } else if (server1->server_key > server2->server_key) {
        return 1;
    } else {
        return 0;
    }
}

// this function is searching binary in the ring for
// the smallest number, greater then "key"
unsigned int find_next_server_key(struct load_balancer *main,
                                  unsigned int key) {
    int left = 0;
    int right = (3 * main->servers_count) - 1;
    int mid = 0;
    while (left <= right) {
        mid = left + (right - left) / 2;
        if (main->hashring[mid] <= key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    if (left >= (3 * main->servers_count)) {
        return main->hashring[0];
    } else {
        return main->hashring[left];
    }
}

// fishing products from all the servers, that fits in the new server
void fishing_products(load_balancer *main, unsigned int next_server_key,
                      server_memory *server)
{
    int index;
    // searching for the server with the "next_server_key"
    for (int i = 0; i < main->servers_count; i++) {
        if (main->servers[i]->hash_key == next_server_key ||
            main->servers[i]->hash_copy1_key == next_server_key ||
            main->servers[i]->hash_copy2_key == next_server_key) {
            index = i;
            break;
        }
    }

    // fishing products from the server with the next_server_key
    for (unsigned int i = 0; i < main->servers[index]->server_hash->hmax; i++) {
        ll_node_t *curr = main->servers[index]->server_hash->buckets[i]->head;
        while (curr && main->servers[index] != server) {
            // adding the products in the new server
            server_store(server, ((info *)curr->data)->key,
                         ((info *)curr->data)->value);

            if (curr->next != NULL)
                curr = curr->next;
            else
                break;
        }
    }
}

void loader_add_server(load_balancer *main, int server_id) {
    // initialising the new server
    server_memory *server = init_server_memory();
    // adding new server in the server list
    main->servers = realloc(main->servers, (1 + main->servers_count) *
                            sizeof(struct server_memory *));
    DIE(main->servers == NULL, "Server realloc failed\n");

    // 138 -> 145: storing the keys and the hashes
    server->server_key = server_id;
    server->hash_key = hash_function_servers(&server_id);

    server->sv_copy1_key = server_id + 100000;
    server->hash_copy1_key = hash_function_servers(&server->sv_copy1_key);

    server->sv_copy2_key = server_id + 200000;
    server->hash_copy2_key = hash_function_servers(&server->sv_copy2_key);

    // storing new server in server list, and sorting the list
    main->servers[main->servers_count] = server;
    qsort(main->servers, main->servers_count + 1,
          sizeof(struct server_memory *), compare_servers);

    main->servers_count++;

    // adding the new 3 hashes in the ring and sorting it
    // hashring has size 3 times greater than number of servers
    main->hashring = realloc(main->hashring, (3 * main->servers_count) *
                             sizeof(unsigned int));
    DIE(main->hashring == NULL, "Hashring realloc failed\n");
    main->hashring[3 * (main->servers_count - 1)] = server->hash_key;
    main->hashring[3 * (main->servers_count - 1) + 1] = server->hash_copy1_key;
    main->hashring[3 * (main->servers_count - 1) + 2] = server->hash_copy2_key;

    // sroting hashes by key
    qsort(main->hashring, 3 * main->servers_count,
          sizeof(unsigned int), ring_compare);

    // now, we call "fishing_products" 3 times, one time for normal hash,
    // one time for copy1 hash, one time for copy2 hash.
    // each hash steals products from different servers.
    unsigned int next_server_key = find_next_server_key(main, server->hash_key);
    fishing_products(main, next_server_key, server);

    next_server_key = find_next_server_key(main, server->hash_copy1_key);
    fishing_products(main, next_server_key, server);

    next_server_key = find_next_server_key(main, server->hash_copy2_key);
    fishing_products(main, next_server_key, server);
}

void loader_remove_server(load_balancer *main, unsigned int server_id) {
    // searching and storing the server we are about to remove
    // in "aux_server" because it's easier to work with
    server_memory *aux_server = NULL;
    int index = 0, dummy;
    // serching for the server with the given id
    for (int i = 0; i < main->servers_count; i++) {
        if (main->servers[i]->server_key == server_id) {
            // printf("Server found\n");
            aux_server = main->servers[i];
            index = i;
            break;
        }
    }
    if (aux_server == NULL) {
        printf("Server not found\n");
        return;
    }

    // we are entering the process of removing the server: first, we are
    // initialising the hashes (coresponding the server we are removing with)
    // "-1" (this is the max value for the unsinged int variable).
    for (int i = 0; i < 3 * main->servers_count; i++)
        if (main->hashring[i] == aux_server->hash_key ||
            main->hashring[i] == aux_server->hash_copy1_key ||
            main->hashring[i] == aux_server->hash_copy2_key) {
            main->hashring[i] = -1;
        }

    // This is great, beacuse after that we can sort the hashring
    qsort(main->hashring, 3 * main->servers_count,
          sizeof(unsigned int), ring_compare);

    // and resizing it :)
    main->hashring = realloc(main->hashring, (3 * main->servers_count - 3) *
                             sizeof(unsigned int));
    --main->servers_count;

    // now that the old hashes are not bothering us anymore, we go through
    // the products corresponding to the server we want to remove
    for (unsigned int i = 0; i < aux_server->server_hash->hmax; i++) {
        ll_node_t *curr = aux_server->server_hash->buckets[i]->head;

        while (curr) {
            loader_store(main, ((info *)curr->data)->key,
                         ((info *)curr->data)->value, &dummy);
            curr = curr->next;
        }
    }
    // doing all the necesary frees
    free_server_memory(aux_server);

    ht_free(aux_server->server_hash);
    free(aux_server);

    // eliminating the server from the array
    for (int i = index; i < main->servers_count; i++) {
        main->servers[i] = main->servers[i + 1];
    }
    main->servers = realloc(main->servers, main->servers_count *
                            sizeof(server_memory *));
}

void loader_store(load_balancer *main, char *key, char *value, int *server_id) {
    unsigned int h_key = hash_function_key(key);
    // "where_to_store" variabile represents the hash
    // of the server we are storing the product
    unsigned int where_to_store = find_next_server_key(main, h_key);

    for (int i = 0; i <= main->servers_count; i++)
        // 3 conditions, because "where_to_store" variable may be
        // hash_key....or hash_copy1_key....or hash_copy2_key
        if (main->servers[i]->hash_key == where_to_store ||
            main->servers[i]->hash_copy1_key == where_to_store ||
            main->servers[i]->hash_copy2_key == where_to_store) {
            server_store(main->servers[i], key, value);
            *server_id = main->servers[i]->server_key;
            return;
        }
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id) {
    unsigned int h_key = hash_function_key(key);
    // "where_is_stored" variable, represents the hash
    // of the server where the product is stored
    unsigned int where_is_stored = find_next_server_key(main, h_key);

    for (int i = 0; i < main->servers_count; i++)
        // 3 conditions, because "where_is_stored" variable may be
        // hash_key....or hash_copy1_key....or hash_copy2_key
        if (main->servers[i]->hash_key == where_is_stored ||
            main->servers[i]->hash_copy1_key == where_is_stored ||
            main->servers[i]->hash_copy2_key == where_is_stored) {
            *server_id = main->servers[i]->server_key;
            return server_retrieve(main->servers[i], key);
        }
    return NULL;
}

void free_load_balancer(load_balancer *main) {
    // freeing all the servers
    for (int i = 0; i < main->servers_count; i++) {
        free_server_memory(main->servers[i]);
        ht_free(main->servers[i]->server_hash);
        free(main->servers[i]);
    }
    free(main->servers);
    free(main->hashring);
    free(main);
}
