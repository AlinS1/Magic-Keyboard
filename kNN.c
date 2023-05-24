
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
			kdt = load_command(delim);
			break;
		case 2:	 // NEAREST NEIGHBOUR
			nearest_neighbour_command(kdt, delim);
			break;
		case 3:	 // RANGE SEARCH
			range_search_command(kdt, delim);
			break;
		case 4:	 // EXIT
			kdt_tree_free(kdt);
			exit(0);
			break;
		default:
			printf("Incorrect command\n");
		}
	}
	return 0;
}
