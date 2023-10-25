// Copyright 2023 Nastase Cristian-Gabriel 315CA

#include "utils.h"
#include "server.h"
#include "load_balancer.h"

server_memory *init_server_memory()
{
	// Allocate memory for the server
	server_memory *server = malloc(sizeof(server_memory));
	DIE(server == NULL, "Server alloc failed\n");

	// Allocate memory for the server hash table
	server->server_hash = ht_create(HMAX, hash_function_key,
							compare_function_strings);

	return server;
}

void server_store(server_memory *server, char *key, char *value) {
	// Store the key-value pair in the server hash table
	ht_put(server->server_hash, key, strlen(key) + 1, value, strlen(value) + 1);
}

char *server_retrieve(server_memory *server, char *key) {
	// Retrieve the value associated with the key from the server hash table
	return ht_get(server->server_hash, key);
}

void server_remove(ll_node_t *node) {
	// Free the memory of the node
	free(node->data);
	free(node);
}

void free_server_memory(server_memory *server) {
	// Free the memory of the server hash table
	for (unsigned int i = 0; i < server->server_hash->hmax; i++) {
		ll_free(&server->server_hash->buckets[i]);
	}
}
