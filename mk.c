#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commands_mk.h"
#include "trie.h"

int command_type_to_number(char *command)
{
	if (!command)
		return -1;

	if (strcmp(command, "INSERT") == 0)
		return 1;
	if (strcmp(command, "LOAD") == 0)
		return 2;
	if (strcmp(command, "REMOVE") == 0)
		return 3;
	if (strcmp(command, "AUTOCORRECT") == 0)
		return 4;
	if (strcmp(command, "AUTOCOMPLETE") == 0)
		return 5;
	if (strcmp(command, "EXIT") == 0)
		return 6;

	return -1;
}

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
