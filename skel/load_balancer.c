/* Copyright 2023 <> */

#include "utils.h"
#include "load_balancer.h"
#include "server.h"

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

load_balancer *init_load_balancer() {
    load_balancer *main = malloc(sizeof(load_balancer));
    DIE(main == NULL, "Ballancer alloc failed\n");

    main->servers = NULL;
    main->hashring = NULL;
    main->servers_count = 0;

    return main;
}

void loader_add_server(load_balancer *main, int server_id) {
    server_memory *server = init_server_memory();
    main->servers = realloc(main->servers, (1 + main->servers_count) * sizeof(struct server_memory *));
    DIE(main->servers == NULL, "Server realloc failed\n");

    server->server_key = server_id;
    server->hash_key = hash_function_servers(&server_id);

    server->sv_copy1_key = server_id + 100000;
    server->hash_copy1_key = hash_function_servers(&server->sv_copy1_key);

    server->sv_copy2_key = server_id + 200000;
    server->hash_copy2_key = hash_function_servers(&server->sv_copy2_key);

    main->servers[main->servers_count] = server;

    main->servers_count++;

#ifdef DEBUG
    printf("server hash: %u\n", server->hash_key);
    printf("server copy1 hash: %u\n", server->hash_copy1_key);
    printf("server copy2 hash: %u\n", server->hash_copy2_key);
#endif

    main->hashring = realloc(main->hashring, (3 * main->servers_count) * sizeof(unsigned int));
    DIE(main->hashring == NULL, "Hashring realloc failed\n");
    main->hashring[3 * (main->servers_count - 1)] = server->hash_key;
    main->hashring[3 * (main->servers_count - 1) + 1] = server->hash_copy1_key;
    main->hashring[3 * (main->servers_count - 1) + 2] = server->hash_copy2_key;


    // // printing ring;
    // for (int i = 0; i < main->servers_count; i++) {
    //     printf("%u ", main->hashring[i]);
    // }
    // printf("\n");


    // sroting servers by key
    qsort(main->hashring, 3 * main->servers_count, sizeof(unsigned int), ring_compare);


    // // printing ring;
    // printf("ring: \n");
    // for (int i = 0; i < 3 * main->servers_count; i++) {
    //     printf("%u ", main->hashring[i]);
    // }
    // printf("\n");
}

void loader_remove_server(load_balancer *main, int server_id) {
    /* TODO 3 */
    printf("debug\n");
}

unsigned int find_next_server_key(struct load_balancer *main, unsigned int key) {
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

void loader_store(load_balancer *main, char *key, char *value, int *server_id) {
    /* TODO 4 */
    unsigned int h_key = hash_function_key(key);
    unsigned int where_to_store = find_next_server_key(main, h_key);

    // printf("h_key: %u\n", h_key);
    // printf("\n");
    // printf("where_to_store: %u\n", where_to_store);

    for (int i = 0; i < main->servers_count; i++) {
        if (main->servers[i]->hash_key == where_to_store) {
            server_store(main->servers[i], key, value);
            *server_id = main->servers[i]->server_key;
            return;
        }
        if (main->servers[i]->hash_copy1_key == where_to_store) {
            server_store(main->servers[i], key, value);
            *server_id = main->servers[i]->server_key;
            return;
        }
        if (main->servers[i]->hash_copy2_key == where_to_store) {
            server_store(main->servers[i], key, value);
            *server_id = main->servers[i]->server_key;
            return;
        }
    }
}

char *loader_retrieve(load_balancer *main, char *key, int *server_id) {

    unsigned int h_key = hash_function_key(key);
    unsigned int where_is_stored = find_next_server_key(main, h_key);

    for (int i = 0; i < main->servers_count; i++) {
        if (main->servers[i]->hash_key == where_is_stored) {
            *server_id = main->servers[i]->server_key;
            return server_retrieve(main->servers[i], key);
        }
        if (main->servers[i]->hash_copy1_key == where_is_stored) {
            *server_id = main->servers[i]->server_key;
            return server_retrieve(main->servers[i], key);
        }
        if (main->servers[i]->hash_copy2_key == where_is_stored) {
            *server_id = main->servers[i]->server_key;
            return server_retrieve(main->servers[i], key);
        }
    }


    return NULL;
}

void free_load_balancer(load_balancer *main) {
    /* TODO 6 */
    // printf("debug\n");
}
