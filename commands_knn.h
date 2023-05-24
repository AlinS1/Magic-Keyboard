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

int command_string_to_number(char *command);

kdt_tree_t *load_command(char *delim);

void nearest_neighbour_helper(kdt_node_t *root, int k, int *point,
							  nearest_neighbour_t *nn);

void nearest_neighbour_command(kdt_tree_t *kdt, char *delim);

#endif	// COMMANDS_KNN_H