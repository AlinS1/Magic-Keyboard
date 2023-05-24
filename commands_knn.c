#include "commands_knn.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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