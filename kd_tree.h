#ifndef KD_TREE_H
#define KD_TREE_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIE(assertion, call_description)                       \
	do {                                                       \
		if (assertion) {                                       \
			fprintf(stderr, "(%s, %d): ", __FILE__, __LINE__); \
			perror(call_description);                          \
			exit(errno);                                       \
		}                                                      \
	} while (0)

typedef struct kdt_node_t kdt_node_t;
struct kdt_node_t {
	/* left child */
	kdt_node_t *left;

	/* right child */
	kdt_node_t *right;

	/* data contained by the node */
	int *data;
};

typedef struct kdt_tree_t kdt_tree_t;
struct kdt_tree_t {
	/* number of dimensions */
	int k;

	/* root of the tree */
	kdt_node_t *root;
};

// ================================
// ==== KD Tree Implementation ====
// ================================

kdt_node_t *__kdt_node_create(int *data, int k);

kdt_tree_t *kdt_tree_create(int k);

void kdt_tree_insert(kdt_tree_t *kdt_tree, int *data);

void __kdt_tree_free(kdt_node_t *kdt_node);

void kdt_tree_free(kdt_tree_t *kdt_tree);

// ===============================
// ===== Auxiliary Functions =====
// ===============================

int cmp_dim(int *current, int *to_add, int current_dim, int k);

void print_data(int *data, int k);

#endif	// KD_TREE_H