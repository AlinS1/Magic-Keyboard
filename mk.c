#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands_mk.h"
#include "trie.h"

int main(void)
{
	char line[MAX_LINE];
	char delim[] = "\n ";
	trie_t *trie = trie_create(sizeof(int), ALPHABET_SIZE, ALPHABET);

	while (1) {
		fgets(line, MAX_LINE, stdin);
		char *command = strtok(line, delim);
		int command_type = command_type_to_number(command);

		switch (command_type) {
		case 1:	 // INSERT
			insert_command(trie, delim);
			break;
		case 2:	 // LOAD
			load_command(trie, delim);
			break;
		case 3:	 // REMOVE
			remove_command(trie, delim);
			break;
		case 4:	 // AUTOCORRECT
			autocorrect_command(trie, delim);
			break;
		case 5:	 // AUTOCOMPLETE
			autocomplete_command(trie, delim);
			break;
		case 6:	 // EXIT
			trie_free(trie->root);
			free(trie);
			exit(0);
		}
	}
	return 0;
}
