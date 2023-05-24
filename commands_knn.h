#ifndef COMMANDS_KNN_H
#define COMMANDS_KNN_H

#define MAX_LINE 100

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kd_tree.h"

typedef struct nearest_neighbour_t nearest_neighbour_t;
struct nearest_neighbour_t {
	int size;
	double norm;
	int **neighbours;
};

typedef struct range_t range_t;
struct range_t {
	int *start;
	int *end;
	int **points;
	int nr_elem;
	int capacity;
};

int command_string_to_number(char *command);

kdt_tree_t *load_command(char *delim);

int cmp_node(int *node_1, int *node_2, int k);

void swap(int **node_1, int **node_2);

void print_array_of_nodes(int **nodes, int k, int arr_size);

// =============================
// ===== NEAREST NEIGHBOUR =====
// =============================

double euclidean_norm(int *elem_1, int *elem_2, int k);

nearest_neighbour_t *init_nn(void);

void free_nn(nearest_neighbour_t **nn);

void update_nn(nearest_neighbour_t *nn, double norm, kdt_node_t *root, int k);

void nearest_neighbour_helper(kdt_node_t *root, int k, int *point,
							  nearest_neighbour_t *nn);

void nearest_neighbour_command(kdt_tree_t *kdt, char *delim);

// ==============================
// ======== SEARCH RANGE ========
// ==============================

range_t *init_range(kdt_tree_t *kdt, char *delim);

void free_range(range_t **range);

void update_range(range_t *range, kdt_node_t *root, int k);

int is_in_range(kdt_node_t *node, int k, range_t *range);

void range_search_helper(kdt_node_t *root, int k, range_t *range);

void range_search_command(kdt_tree_t *kdt, char *delim);

#endif	// COMMANDS_KNN_H
