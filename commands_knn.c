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
	// printf("n:%d, k:%d", n, k);
	kdt_tree_t *kdt = kdt_tree_create(k);

	for (int i = 0; i < n; i++) {
		int *element = malloc(k * sizeof(int));
		for (int j = 0; j < k; j++) {
			fscanf(in, "%d", &element[j]);
		}
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

nearest_neighbour_t *init_nn()
{
	nearest_neighbour_t *nn = malloc(sizeof(nearest_neighbour_t));
	nn->size = 0;
	nn->norm = INF;
	nn->neighbours = NULL;

	return nn;
}

void print_nn(nearest_neighbour_t *nn, int k)
{
	for (int i = 0; i < nn->size; i++) {
		print_data(nn->neighbours[i], k);
	}
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

void update_nn(nearest_neighbour_t *nn, double norm, kdt_node_t *root)
{
	if (nn->size == 0) {
		nn->neighbours = malloc(sizeof(int *));
		nn->neighbours[0] = root->data;
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
			nn->neighbours[0] = root->data;
			nn->norm = norm;
			return;
		}

		if (norm == nn->norm) {	 // Mai multe puncte cu aceeasi norma
			nn->size++;
			int **aux = realloc(nn->neighbours, sizeof(int *) * nn->size);
			if (aux) {
				nn->neighbours = aux;
				nn->neighbours[nn->size - 1] = root->data;
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

	int rc = cmp_dim(root->data, point, root->level, k);
	if (rc > 0) {
		nearest_neighbour_helper(root->left, k, point, nn);
	} else if (rc < 0) {
		nearest_neighbour_helper(root->right, k, point, nn);
	}

	double norm = euclidean_norm(root->data, point, k);
	printf("norm %f for:", norm);
	print_data(root->data, k);

	update_nn(nn, norm, root);

	int* height;
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

	printf("INORDER:\n");
	kdt_tree_print_inorder(kdt, print_data);
	nearest_neighbour_helper(kdt->root, kdt->k, point, nn);

	print_nn(nn, kdt->k);
	free_nn(&nn);

	free(point);
}
