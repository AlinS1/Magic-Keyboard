#include "commands_mk.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

void insert_command(trie_t *trie, char *delim)
{
	char *word = strtok(NULL, delim);
	printf("word:%s\n", word);
	trie_insert(trie, word);
}

void load_command(trie_t *trie, char *delim)
{
	char *filename = strtok(NULL, delim);

	FILE *in = fopen(filename, "rt");
	DIE(!in, "cannot open file");

	char word[MAX_LINE];
	while (fscanf(in, "%s", word)) {
		trie_insert(trie, word);
	}

	fclose(in);
}

void remove_command(trie_t *trie, char *delim)
{
	char *word = strtok(NULL, delim);

	trie_remove(trie, word);
}

void autocomplete_smallest_lexico(trie_node_t *node, char *prefix)
{
}

void autocomplete_shortest_aux(trie_node_t *node, char *shortest_word,
							   int *nr_chars)
{
	while (node->children) {
		for (int i = 0; i < ALPHABET_SIZE; i++) {
			if (node->children[i]) {
				shortest_word[*nr_chars] = i + 'a';
				*nr_chars = *nr_chars + 1;
				autocomplete_shortest_aux(node->children[i], shortest_word,
										  nr_chars);
			}
		}
	}
}

void autocomplete_shortest(trie_node_t *node, char *prefix)
{
	char *shortest_word = malloc(sizeof(char) * MAX_LINE);
	int nr_chars = 0;
	int len = strlen(prefix);
	for (int i = 0; i < len; i++) {
		shortest_word[i] = prefix[i];
	}
	shortest_word[len] = '\0';
	nr_chars += len;

	while (node->children) {
		for (int i = 0; i < ALPHABET_SIZE; i++) {
			if (node->children[i]) {
				shortest_word[nr_chars] = i + 'a';
				nr_chars = nr_chars + 1;
				if (node->children[i]->end_of_word == 1) {
					// ar trebui bfs cu stiva/coada
					return;
				}
				autocomplete_shortest_aux(node->children[i], shortest_word,
										  &nr_chars);
			}
		}
	}
}

void autocomplete_most_frequent(trie_node_t *node, char *prefix)
{
}

void autocomplete_command(trie_t *trie, char *delim)
{
	char *prefix = strtok(NULL, delim);
	char *number_crt_ascii = strtok(NULL, delim);
	int number_crt = atoi(number_crt_ascii);

	trie_node_t *node_prefix = trie_search(trie, prefix);
	if (!node_prefix) {
		printf("Prefix doesn't exist");
		return;
	}

	switch (number_crt) {
	case 1:	 // Lexicographically smallest
		autocomplete_smallest_lexico(trie, prefix);
		break;
	case 2:	 // Shortest
		autocomplete_shortest(trie, prefix);
		break;
	case 3:	 // Most frequent
		autocomplete_most_frequent(trie, prefix);
		break;
	case 0:	 // All
		autocomplete_smallest_lexico(trie, prefix);
		autocomplete_shortest(trie, prefix);
		autocomplete_most_frequent(trie, prefix);
	}
}