#ifndef TRIE_H
#define TRIE_H

#define ALPHABET_SIZE 26
#define ALPHABET "abcdefghijklmnopqrstuvwxyz"

typedef struct trie_node_t trie_node_t;
struct trie_node_t {
	/* Value associated with key (set if end_of_word = 1) */
	int appearances;

	/* 1 if current node marks the end of a word, 0 otherwise */
	int end_of_word;

	trie_node_t **children;
	int n_children;
};

typedef struct trie_t trie_t;
struct trie_t {
	trie_node_t *root;

	/* Number of keys */
	int size;

	/* Generic Data Structure */
	int data_size;

	/* Trie-Specific, alphabet properties */
	int alphabet_size;
	char *alphabet;

	/* Optional - number of nodes, useful to test correctness */
	int nNodes;
};

trie_node_t *trie_create_node(trie_t *trie);
trie_t *trie_create(int data_size, int alphabet_size, char *alphabet);
void trie_insert(trie_t *trie, char *key);
trie_node_t *trie_search(trie_t *trie, char *key);
// int aux_trie_remove(trie_t *trie, trie_node_t *node, char *key);

void aux_trie_remove(trie_t *trie, char *key);
void trie_remove(trie_t *trie, char *key);
trie_node_t *trie_free(trie_node_t *node);
void cleanup_example_string(char *str);

#endif	// TRIE_H