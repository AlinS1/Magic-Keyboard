
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands_knn.h"
#include "kd_tree.h"

int main(void)
{
	char line[MAX_LINE];
	char delim[] = "\n ";

	while (1) {
		fgets(line, MAX_LINE, stdin);
		char *command = strtok(line, delim);
		int command_type = command_string_to_number(command);
		kdt_tree_t *kdt;

		switch (command_type) {
		case 1:	 // LOAD
			break;
		case 2:	 // NEAREST NEIGHBOUR
			break;
		case 3:	 // RANGE SEARCH
			break;
		case 4:	 // EXIT
			break;
		default:
			printf("Incorrect command\n");
		}
	}
	return 0;
}
