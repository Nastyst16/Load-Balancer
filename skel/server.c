/* Copyright 2023 <> */


#include "utils.h"
#include "server.h"
#include "load_balancer.h"

// struct server_memory {
// 	hashtable_t *server_hash;
// 	unsigned int server_key;
// 	// hashtable_t *server_copy1;
// 	// hashtable_t *server_copy2;
// };

server_memory *init_server_memory()
{
	server_memory *server = malloc(sizeof(server_memory));
	DIE(server == NULL, "Server alloc failed\n");

	server->server_hash = ht_create(HMAX, hash_function_key, compare_function_strings);

	return server;
}

void server_store(server_memory *server, char *key, char *value) {

	ht_put(server->server_hash, key, strlen(key) + 1, value, strlen(value) + 1);
}

char *server_retrieve(server_memory *server, char *key) {

	return ht_get(server->server_hash, key);
}

void server_remove(server_memory *server, char *key) {
	/* TODO 4 */
	// printf("debug\n");
}

void free_server_memory(server_memory *server) {
	/* TODO 5 */
	// printf("debug\n");
}
