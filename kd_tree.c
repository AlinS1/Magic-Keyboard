#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kd_tree.h"

/**
 * Helper function to create a node
 * @data: the data to be added in the node
 * @data_size: data's size
 */
kdt_node_t *__kdt_node_create(int *data, int k)
{
	kdt_node_t *kdt_node = malloc(sizeof(*kdt_node));

	DIE(!kdt_node, "kdt_node malloc");

	kdt_node->left = kdt_node->right = NULL;

	kdt_node->data = malloc(k * sizeof(int));
	DIE(!kdt_node->data, "kdt_node->data malloc");
	memcpy(kdt_node->data, data, k * sizeof(int));

	return kdt_node;
}

/**
 * Alloc memory for a new kdt
 * @data_size: size of the data contained by the kdt's nodes
 * @return: pointer to the newly created kdt
 */
kdt_tree_t *kdt_tree_create(int k)
{
	kdt_tree_t *kdt_tree = malloc(sizeof(*kdt_tree));
	DIE(!kdt_tree, "kdt_tree malloc");

	kdt_tree->root = NULL;
	kdt_tree->k = k;

	return kdt_tree;
}

/**
 * Insert a new element in a kdt
 * @kdt_tree: the kdt where to insert the new element
 * @data: the data to be inserted in kdt
 */
void kdt_tree_insert(kdt_tree_t *kdt_tree, int *data)
{
	int rc;
	kdt_node_t *root = kdt_tree->root;
	kdt_node_t *parent = NULL;
	kdt_node_t *node = __kdt_node_create(data, kdt_tree->k);
	int current_dim = 0;

	if (kdt_tree->root == NULL) {
		kdt_tree->root = node;
		printf("Inserted element:");
		print_data(data, kdt_tree->k);
		return;
	}

	while (root) {
		rc = cmp_dim(root->data, data, current_dim, kdt_tree->k);
		if (rc > 0) {
			parent = root;
			root = root->left;
		} else if (rc < 0) {
			parent = root;
			root = root->right;
		}
		current_dim++;
	}

	rc = cmp_dim(parent->data, data, current_dim - 1, kdt_tree->k);
	if (rc > 0) {
		parent->left = node;
	} else {
		parent->right = node;
	}

	printf("Inserted element:");
	print_data(data, kdt_tree->k);
}

/**
 * Free the left and the right subtree of a node, its data and itself
 * @b_node: the node which has to free its children and itself
 * @free_data: function used to free the data contained by a node
 */
void __kdt_tree_free(kdt_node_t *kdt_node)
{
	if (!kdt_node)
		return;

	if (kdt_node->left)
		__kdt_tree_free(kdt_node->left);
	if (kdt_node->right)
		__kdt_tree_free(kdt_node->right);

	free(kdt_node->data);
	free(kdt_node);
}

/**
 * Free a kdt
 * @kdt_tree: the kdt to be freed
 * @free_data: function used to free the data contained by a node
 */
void kdt_tree_free(kdt_tree_t *kdt_tree)
{
	__kdt_tree_free(kdt_tree->root);
	free(kdt_tree);
}

int cmp_dim(int *current, int *to_add, int current_dim, int k)
{
	int idx = current_dim % k;	// prima dimensiune -> 0
	if (current[idx] <= to_add[idx])
		return -1;	// dreapta
	return 1;		// stanga
}

void print_data(int *data, int k)
{
	for (int i = 0; i < k; i++)
		printf("%d ", data[i]);
	printf("\n");
}

// static void __kdt_tree_print_inorder(kdt_node_t *kdt_node,
// 									 void (*print_data)(int *, int), int k)
// {
// 	if (!kdt_node)
// 		return;

// 	__kdt_tree_print_inorder(kdt_node->left, print_data, k);
// 	print_data(kdt_node->data, k);
// 	__kdt_tree_print_inorder(kdt_node->right, print_data, k);
// }

// /**
//  * Print inorder a kdt
//  * @kdt_tree: the kdt to be printed
//  * @print_data: function used to print the data contained by a node
//  */
// void kdt_tree_print_inorder(kdt_tree_t *kdt_tree,
// 							void (*print_data)(int *, int))
// {
// 	__kdt_tree_print_inorder(kdt_tree->root, print_data, kdt_tree->k);
// }

// int main(void)
// {
// 	int n, k;
// 	printf("Enter n and k(nr_dim):");
// 	scanf("%d %d", &n, &k);
// 	kdt_tree_t *kdt = kdt_tree_create(k);

// 	for (int i = 0; i < n; i++) {
// 		int *element = malloc(k * sizeof(int));
// 		for (int i = 0; i < k; i++) {
// 			scanf("%d", &element[i]);
// 		}
// 		kdt_tree_insert(kdt, element);
// 		free(element);
// 	}
// 	printf("Root:\n");
// 	print_data(kdt->root->data, k);
// 	printf("L_son:\n");
// 	print_data(kdt->root->left->data, k);
// 	printf("R_son:\n");
// 	print_data(kdt->root->right->data, k);
// 	printf("Inordine:\n");
// 	kdt_tree_print_inorder(kdt, print_data);
// 	kdt_tree_free(kdt);

// 	return 0;
// }
