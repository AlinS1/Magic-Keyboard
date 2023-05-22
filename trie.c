#include "trie.h"	

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

trie_node_t *trie_create_node(trie_t *trie)
{
	if (!trie)
		return NULL;
	trie_node_t *trie_node = malloc(sizeof(trie_node_t));
	trie_node->children = NULL;
	trie_node->end_of_word = 0;
	trie_node->n_children = 0;
	trie_node->appearances = 0;
	trie->nNodes++;

	return trie_node;
}

trie_t *trie_create(int data_size, int alphabet_size, char *alphabet)
{
	trie_t *trie = malloc(sizeof(trie_t));
	trie->root = trie_create_node(trie);
	trie->root->appearances = -1;
	trie->size = 0;
	trie->data_size = data_size;
	trie->alphabet_size = alphabet_size;
	trie->alphabet = alphabet;
	trie->nNodes = 1;
	return trie;
}

void trie_insert(trie_t *trie, char *key)
{
	if (!trie)
		return;

	// Daca exista deja, incrementam nr de aparitii
	trie_node_t *node = trie_search(trie, key);
	if (node) {
		node->appearances++;
		return;
	}

	int len = strlen(key);
	trie_node_t *current_node = trie->root;
	for (int i = 0; i < len; i++) {
		int idx = key[i] - 'a';
		current_node->n_children++;

		if (current_node->children == NULL) {
			current_node->children =
				malloc(sizeof(trie_node_t *) * trie->alphabet_size);
			for (int k = 0; k < trie->alphabet_size; k++)
				current_node->children[k] = NULL;
		}
		if (current_node->children[idx] == NULL) {
			current_node->children[idx] = trie_create_node(trie);
		}

		current_node = current_node->children[idx];
	}
	// Pentru ultimul nod
	// printf("Nr copii:%d\n", current_node->n_children);
	current_node->end_of_word = 1;
	current_node->appearances = 1;
}

trie_node_t *trie_search(trie_t *trie, char *key)
{
	if (!trie)
		return NULL;
	int len = strlen(key);
	if (len == 0)
		return trie->root;

	trie_node_t *current_node = trie->root;
	for (int i = 0; i < len; i++) {
		int idx = key[i] - 'a';
		if (i != len - 1 && (current_node->children == NULL ||
							 current_node->children[idx] == NULL))
			return NULL;
		current_node = current_node->children[idx];
	}
	// pt ultimul
	if (current_node->end_of_word == 1)
		return current_node;
}

void aux_trie_remove(trie_t *trie, char *key)
{
	int len = strlen(key);
	trie_node_t *current_node = trie->root;
	trie_node_t *prev_node;
	for (int i = 0; i < len; i++) {
		int idx = key[i] - 'a';
		prev_node = current_node;
		current_node = current_node->children[idx];
	}

	if (current_node == trie->root) {
		if (current_node->n_children == 0) {
			free(current_node->children);
			current_node->children = NULL;
		}
		return;
	}
	if (current_node->end_of_word == 1)
		return;
	if (current_node->n_children == 0) {
		// printf("rm:%c\n", key[len - 1]);
		char *new_word = malloc(sizeof(char) * len);
		for (int i = 0; i < len - 1; i++)
			new_word[i] = key[i];
		new_word[len - 1] = '\0';

		trie->nNodes--;

		free(current_node->children);
		current_node->children = NULL;
		free(current_node);
		current_node = NULL;

		int idx = key[len - 1] - 'a';
		// printf("idx:%d car:%c\n", idx, key[len - 1]);
		prev_node->children[idx] = NULL;

		aux_trie_remove(trie, new_word);
		free(new_word);
	}
}

void trie_remove(trie_t *trie, char *key)
{
	if (!trie)
		return;

	int len = strlen(key);

	// Verificam daca avem acel cuvant
	trie_node_t *node = trie_search(trie, key);
	if (!node) {
		printf("No such word\n");
		return;
	}

	node = trie->root;
	for (int i = 0; i < len; i++) {
		node->n_children--;
		int idx = key[i] - 'a';
		node = node->children[idx];
	}

	node->end_of_word = 0;
	if (node->n_children == 0) {
		char *new_word = malloc(sizeof(char) * len);
		for (int i = 0; i < len - 1; i++)
			new_word[i] = key[i];
		new_word[len - 1] = '\0';

		// Trebuie sa eliminam si nodurile din spate pana cand gasim un alt
		// cuvant.
		aux_trie_remove(trie, new_word);
		free(new_word);

		free(node->children);
		node->children = NULL;
		free(node);
		node = NULL;
		trie->nNodes--;
	}
}

trie_node_t *trie_free(trie_node_t *node)
{
	if (node == NULL)
		return NULL;
	// printf("Un nod exista macar\n");

	if (node->children == NULL) {
		free(node);
		node = NULL;
		return NULL;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		trie_node_t *current = node->children[i];
		if (current != NULL) {
			// printf("C_curr:%c\n", i + 'a');
			trie_free(current);
		}
	}

	if (node->children) {
		free(node->children);
		node->children = NULL;
	}
	free(node);
	node = NULL;
	return NULL;
}

/* Needed for Lambda tests, ignore :) */
void cleanup_example_string(char *str)
{
	int len = strlen(str);

	if (str[len - 2] == '\\') {
		str[len - 2] = '\0';
	}
}
