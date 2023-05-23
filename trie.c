#include "trie.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

trie_node_t *trie_create_node(trie_t *trie)
{
	if (!trie)
		return NULL;
	trie_node_t *trie_node = malloc(sizeof(trie_node_t));
	DIE(!trie_node, "malloc failed");
	trie_node->children = NULL;
	trie_node->end_of_word = 0;
	trie_node->n_children = 0;
	trie_node->appearances = 0;
	trie->nr_nodes++;

	return trie_node;
}

trie_t *trie_create(int data_size, int alphabet_size, char *alphabet)
{
	trie_t *trie = malloc(sizeof(trie_t));
	DIE(!trie, "malloc failed");
	trie->root = trie_create_node(trie);
	trie->root->appearances = -1;
	trie->size = 0;
	trie->data_size = data_size;
	trie->alphabet_size = alphabet_size;
	trie->alphabet = alphabet;
	trie->nr_nodes = 1;
	return trie;
}

void trie_insert(trie_t *trie, char *key)
{
	if (!trie)
		return;

	// If the word already exists, we just increment its number of appearances
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

		if (!current_node->children) {
			current_node->children =
				malloc(sizeof(trie_node_t *) * trie->alphabet_size);
			DIE(!current_node->children, "malloc failed");

			for (int k = 0; k < trie->alphabet_size; k++)
				current_node->children[k] = NULL;
		}
		if (!current_node->children[idx])
			current_node->children[idx] = trie_create_node(trie);

		current_node = current_node->children[idx];
	}
	// For the last node
	current_node->end_of_word = 1;
	current_node->appearances = 1;
}

trie_node_t *trie_search_path(trie_t *trie, char *key)
{
	if (!trie)
		return NULL;
	int len = strlen(key);
	if (len == 0)
		return trie->root;

	trie_node_t *current_node = trie->root;
	for (int i = 0; i < len; i++) {
		int idx = key[i] - 'a';
		if (i != len - 1 &&
			(!current_node->children || !current_node->children[idx]))
			return NULL;
		current_node = current_node->children[idx];
	}
	return current_node;
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
		if (!current_node->children || !current_node->children[idx])
			return NULL;
		current_node = current_node->children[idx];
	}

	// For the last
	if (current_node->end_of_word == 1)
		return current_node;
	else
		return NULL;
}

void aux_trie_remove(trie_t *trie, char *key, int *found_other_word)
{
	if (*found_other_word == 1)	 // Stop if we found another word
		return;

	// Go to the node that corresponds to the last char in "key".
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
		return;	 // Stop if we reached the root
	}

	// We have reached a character that is contained in another word, so we have
	// to stop.
	if (current_node->end_of_word == 1 || current_node->n_children != 0) {
		*found_other_word = 1;
		return;
	}

	// Free the memory only if the node doesn't have other children
	if (current_node->n_children == 0) {
		char *new_word = malloc(sizeof(char) * len);
		DIE(!new_word, "malloc failed");

		for (int i = 0; i < len - 1; i++)
			new_word[i] = key[i];
		new_word[len - 1] = '\0';

		trie->nr_nodes--;

		free(current_node->children);
		current_node->children = NULL;
		free(current_node);
		current_node = NULL;

		// Eliminate the connection from the node's parent towards the node
		int idx = key[len - 1] - 'a';
		prev_node->children[idx] = NULL;

		aux_trie_remove(trie, new_word, found_other_word);
		free(new_word);
	}
}

void trie_remove(trie_t *trie, char *key)
{
	if (!trie)
		return;

	int len = strlen(key);

	// Verify if the word we want to remove exists.
	trie_node_t *node = trie_search(trie, key);
	if (!node)
		return;

	node->end_of_word = 0;
	node->appearances = 0;

	// Decrement the number of children that the word's nodes will have.
	node = trie->root;
	for (int i = 0; i < len; i++) {
		node->n_children--;
		int idx = key[i] - 'a';
		node = node->children[idx];
	}

	if (node->n_children == 0) {
		int found_other_word = 0;
		aux_trie_remove(trie, key, &found_other_word);
		trie->nr_nodes--;
	}
}

trie_node_t *trie_free(trie_node_t *node)
{
	if (!node)
		return NULL;

	if (!node->children) {
		free(node);
		node = NULL;
		return NULL;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		trie_node_t *current = node->children[i];
		if (current)
			trie_free(current);
	}

	if (node->children) {
		free(node->children);
		node->children = NULL;
	}
	free(node);
	node = NULL;
	return NULL;
}
