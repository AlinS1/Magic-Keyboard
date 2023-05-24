#include "commands_knn.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kd_tree.h"

#define INF 2147483647

int command_string_to_number(char *command)
{
	if (strcmp(command, "LOAD") == 0)
		return 1;
	if (strcmp(command, "NN") == 0)
		return 2;
	if (strcmp(command, "RS") == 0)
		return 3;
	if (strcmp(command, "EXIT") == 0)
		return 4;
	return -1;
}

kdt_tree_t *load_command(char *delim)
{
	char *filename = strtok(NULL, delim);

	FILE *in = fopen(filename, "rt");
	DIE(!in, "cannot open file");
	int n, k;
	fscanf(in, "%d %d", &n, &k);
	kdt_tree_t *kdt = kdt_tree_create(k);

	for (int i = 0; i < n; i++) {
		int *element = malloc(k * sizeof(int));
		for (int j = 0; j < k; j++)
			fscanf(in, "%d", &element[j]);
		kdt_tree_insert(kdt, element);
		free(element);
	}

	fclose(in);
	return kdt;
}

double euclidean_norm(int *elem_1, int *elem_2, int k)
{
	double sum = 0;
	for (int i = 0; i < k; i++) {
		int sum_elem = elem_2[i] - elem_1[i];
		sum_elem *= sum_elem;
		sum += sum_elem;
	}
	return sqrt((double)sum);
}

nearest_neighbour_t *init_nn(void)
{
	nearest_neighbour_t *nn = malloc(sizeof(nearest_neighbour_t));
	nn->size = 0;
	nn->norm = INF;
	nn->neighbours = NULL;

	return nn;
}

int cmp_node(int *node_1, int *node_2, int k)
{
	for (int i = 0; i < k; i++) {
		if (node_1[i] == node_2[i])
			continue;
		if (node_1[i] < node_2[i])
			return 0;  // no need to swap
		if (node_1[i] > node_2[i])
			return 1;  // we need to swap them
	}
	return 0;
}

void swap(int **node_1, int **node_2)
{
	int *aux = *node_1;
	*node_1 = *node_2;
	*node_2 = aux;
}

void print_array_of_nodes(int **nodes, int k, int arr_size)
{
	if (arr_size > 1) {
		for (int i = 0; i < arr_size - 1; i++) {
			for (int j = i + 1; j < arr_size; j++)
				if (cmp_node(nodes[i], nodes[j], k) == 1)
					swap(&nodes[i], &nodes[j]);
		}
	}

	for (int i = 0; i < arr_size; i++)
		print_data(nodes[i], k);
}

void free_nn(nearest_neighbour_t **nn)
{
	nearest_neighbour_t *aux = *nn;

	for (int i = 0; i < aux->size; i++) {
		free(aux->neighbours[i]);
		aux->neighbours[i] = NULL;
	}
	free(aux->neighbours);
	aux->neighbours = NULL;
	free(aux);
	*nn = NULL;
}

void update_nn(nearest_neighbour_t *nn, double norm, kdt_node_t *root, int k)
{
	if (nn->size == 0) {
		nn->neighbours = malloc(sizeof(int *));
		nn->neighbours[0] = malloc(sizeof(int) * k);
		memcpy(nn->neighbours[0], root->data, sizeof(int) * k);
		nn->norm = norm;
		nn->size++;
		return;
	}
	if (nn->size != 0) {
		if (norm < nn->norm) {
			for (int i = 0; i < nn->size; i++)
				free(nn->neighbours[i]);
			free(nn->neighbours);
			nn->neighbours = malloc(sizeof(int *));
			nn->size = 1;
			nn->neighbours[0] = malloc(sizeof(int) * k);
			memcpy(nn->neighbours[0], root->data, sizeof(int) * k);
			nn->norm = norm;
			return;
		}

		if (norm == nn->norm) {	 // Mai multe puncte cu aceeasi norma
			nn->size++;
			int **aux = realloc(nn->neighbours, sizeof(int *) * nn->size);
			if (aux) {
				nn->neighbours = aux;
				nn->neighbours[nn->size - 1] = malloc(sizeof(int) * k);
				memcpy(nn->neighbours[nn->size - 1], root->data,
					   sizeof(int) * k);
			}
			return;
		}
	}
}

void nearest_neighbour_helper(kdt_node_t *root, int k, int *point,
							  nearest_neighbour_t *nn)
{
	if (!root)
		return;

	nearest_neighbour_helper(root->left, k, point, nn);
	nearest_neighbour_helper(root->right, k, point, nn);

	double norm = euclidean_norm(root->data, point, k);

	update_nn(nn, norm, root, k);
}

void nearest_neighbour_command(kdt_tree_t *kdt, char *delim)
{
	int *point = malloc(kdt->k * sizeof(int));
	for (int i = 0; i < kdt->k; i++) {
		char *current_element = strtok(NULL, delim);
		int current_nr = atoi(current_element);
		point[i] = current_nr;
	}

	nearest_neighbour_t *nn = init_nn();
	nearest_neighbour_helper(kdt->root, kdt->k, point, nn);
	print_array_of_nodes(nn->neighbours, kdt->k, nn->size);
	free_nn(&nn);

	free(point);
}

range_t *init_range(kdt_tree_t *kdt, char *delim)
{
	range_t *range = malloc(sizeof(range_t));
	range->start = malloc(kdt->k * sizeof(int));
	range->end = malloc(kdt->k * sizeof(int));

	range->nr_elem = 0;
	range->capacity = 2;
	range->points = malloc(range->capacity * sizeof(int *));
	for (int i = 0; i < range->capacity; i++)
		range->points[i] = malloc(kdt->k * sizeof(int));

	for (int i = 0; i < kdt->k; i++) {
		char *current_element = strtok(NULL, delim);
		int current_nr = atoi(current_element);
		range->start[i] = current_nr;

		current_element = strtok(NULL, delim);
		current_nr = atoi(current_element);
		range->end[i] = current_nr;
	}

	return range;
}

void free_range(range_t **range)
{
	range_t *aux_range = *range;
	for (int i = 0; i < aux_range->capacity; i++)
		free(aux_range->points[i]);
	free(aux_range->points);
	free(aux_range->start);
	free(aux_range->end);
	free(*range);
	range = NULL;
}

void update_range(range_t *range, kdt_node_t *root, int k)
{
	range->nr_elem++;
	if (range->nr_elem == range->capacity + 1) {
		range->capacity *= 2;
		int **aux = realloc(range->points, sizeof(int *) * range->capacity);
		if (aux)
			range->points = aux;

		for (int i = range->nr_elem; i < range->capacity; i++)
			range->points[i] = malloc(sizeof(int) * k);
	}

	memcpy(range->points[range->nr_elem - 1], root->data, sizeof(int) * k);
}

int is_in_range(kdt_node_t *node, int k, range_t *range)
{
	for (int i = 0; i < k; i++)
		if (range->start[i] > node->data[i] || node->data[i] > range->end[i])
			return 0;
	return 1;
}

void range_search_helper(kdt_node_t *root, int k, range_t *range)
{
	if (!root)
		return;

	range_search_helper(root->left, k, range);
	range_search_helper(root->right, k, range);

	if (is_in_range(root, k, range))
		update_range(range, root, k);
}

void range_search_command(kdt_tree_t *kdt, char *delim)
{
	range_t *range = init_range(kdt, delim);

	range_search_helper(kdt->root, kdt->k, range);

	print_array_of_nodes(range->points, kdt->k, range->nr_elem);
	free_range(&range);
}
