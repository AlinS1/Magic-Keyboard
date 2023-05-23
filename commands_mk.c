#include "commands_mk.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

#define INF 2147483647

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

void insert_command(trie_t *trie, char *delim)
{
	char *word = strtok(NULL, delim);
	trie_insert(trie, word);
}

void load_command(trie_t *trie, char *delim)
{
	char *filename = strtok(NULL, delim);

	FILE *in = fopen(filename, "rt");
	DIE(!in, "cannot open file");

	char word[MAX_LINE];
	while (!feof(in)) {
		fscanf(in, "%s", word);
		trie_insert(trie, word);
	}
	fclose(in);
}

void remove_command(trie_t *trie, char *delim)
{
	char *word = strtok(NULL, delim);
	trie_remove(trie, word);
}

void autocomplete_smallest_lexico(trie_node_t *end_prefix_node, char *prefix)
{
	// If the prefix is a word, then it is the smallest.
	if (end_prefix_node->end_of_word == 1) {
		printf("%s\n", prefix);
		return;
	}

	// Put the prefix at the beginning of a new initialized word
	// ("smallest_word") that is going to become the word we are looking for.
	int len = strlen(prefix);
	char *smallest_word = malloc(sizeof(char) * MAX_LINE);
	DIE(!smallest_word, "malloc failed");
	strcpy(smallest_word, prefix);
	int nr_chars = len;

	// We iterate through the trie elements by always continuing with the first
	// letter we find from "a" to "z" until we find a word ending.
	// This way, we will surely find the shortest and smallest lexicographical
	// word.
	// Each new letter is added at the end of the word we previously
	// initialized ("smallest_word").
	trie_node_t *current_node = end_prefix_node;
	while (current_node->end_of_word == 0) {
		if (current_node->children) {
			for (int i = 0; i < ALPHABET_SIZE; i++) {
				if (current_node->children[i]) {
					current_node = current_node->children[i];
					smallest_word[nr_chars] = i + 'a';
					nr_chars = nr_chars + 1;
					smallest_word[nr_chars] = '\0';
					break;
				}
			}
		}
	}

	// If the "smallest_word" hasn't changed at all, that means there was no
	// word to find.
	if (strcmp(prefix, smallest_word) == 0)
		printf("No words found\n");
	else
		printf("%s\n", smallest_word);
	free(smallest_word);
}

void autocomplete_shortest_helper(trie_node_t *node, char *aux_word,
								  int *nr_chars, int *shortest_len,
								  char *shortest_word)
{
	// For each word found, we verify its length. If it's shorter than the
	// previously found "shortest_word", we update the "shortest_len" and
	// "shortest_word".
	if (node->end_of_word == 1) {
		if (*nr_chars < *shortest_len) {
			*shortest_len = *nr_chars;
			strcpy(shortest_word, aux_word);
		}
	}

	// If it's a leaf node, we go back a level
	if (!node->children)
		return;

	// Go through all node's children that exist and add the character at the
	// end of the current word ("aux_word").
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			aux_word[*nr_chars] = i + 'a';
			*nr_chars = *nr_chars + 1;
			aux_word[*nr_chars] = '\0';

			autocomplete_shortest_helper(node->children[i], aux_word, nr_chars,
										 shortest_len, shortest_word);
			*nr_chars = *nr_chars - 1;
		}
	}
	aux_word[*nr_chars] = '\0';
}

void autocomplete_shortest(trie_node_t *node, char *prefix)
{
	if (node->end_of_word == 1) {
		printf("%s\n", prefix);
		return;
	}

	// Create an auxiliary word that starts as the prefix from which we will
	// start and is updated for each char that we go through in trie. It helps
	// us to keep track of where we are in the trie.
	int len = strlen(prefix);
	int nr_chars = len;
	char *aux_word = malloc(sizeof(char) * MAX_LINE);
	DIE(!aux_word, "malloc failed");
	strcpy(aux_word, prefix);

	// Initialize the minimum length with the greatest number that an integer
	// can hold and a word that is going to finally correspond to the shortest
	// word found.
	int shortest_len = INF;
	char shortest_word[MAX_LINE];
	strcpy(shortest_word, prefix);

	autocomplete_shortest_helper(node, aux_word, &nr_chars, &shortest_len,
								 shortest_word);

	// If the final word is not different from the prefix, there were no words
	// found.
	if (strcmp(prefix, shortest_word) == 0)
		printf("No words found\n");
	else
		printf("%s\n", shortest_word);
	free(aux_word);
}

void autocomplete_most_frequent_helper(trie_node_t *node, char *aux_word,
									   int *nr_chars, int *max_ap,
									   char *max_ap_word)
{
	// If it's a leaf node, it's surely an end of word.
	if (!node->children) {
		aux_word[*nr_chars] = '\0';
		// Verify the number of apperances
		if (node->appearances > *max_ap) {
			*max_ap = node->appearances;
			strcpy(max_ap_word, aux_word);
		}
		return;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			aux_word[*nr_chars] = i + 'a';
			*nr_chars = *nr_chars + 1;
			aux_word[*nr_chars] = '\0';
			// If it's an end of word, verify the number of apperances
			if (node->children[i]->end_of_word == 1) {
				if (node->children[i]->appearances > *max_ap) {
					*max_ap = node->children[i]->appearances;
					strcpy(max_ap_word, aux_word);
				}
			}
			autocomplete_most_frequent_helper(node->children[i], aux_word,
											  nr_chars, max_ap, max_ap_word);
			*nr_chars = *nr_chars - 1;
		}
	}
}

void autocomplete_most_frequent(trie_node_t *node, char *prefix)
{
	// Create an auxiliary word that starts as the prefix from which we will
	// start and is updated for each char that we go through in trie. It helps
	// us to keep track of where we are in the trie.
	int len = strlen(prefix);
	int nr_chars = len;
	char *aux_word = malloc(sizeof(char) * MAX_LINE);
	DIE(!aux_word, "malloc failed");
	strcpy(aux_word, prefix);

	// Initialize the maximum number of appearances with 0 because it's the
	// smallest size a length can be and a word that is going to finally
	// correspond to the word found with the most appearances.
	int max_ap = 0;
	char max_ap_word[MAX_LINE];
	strcpy(max_ap_word, prefix);

	// If the prefix is a word in itself, we update the initial maximum
	// appearances.
	if (node->end_of_word == 1)
		max_ap = node->appearances;

	autocomplete_most_frequent_helper(node, aux_word, &nr_chars, &max_ap,
									  max_ap_word);

	// If the final word is not different from the prefix, there were no words
	// found.
	if (max_ap == 0)
		printf("No words found\n");
	else
		printf("%s\n", max_ap_word);
	free(aux_word);
}

void autocomplete_command(trie_t *trie, char *delim)
{
	char *prefix = strtok(NULL, delim);
	char *number_crt_ascii = strtok(NULL, delim);
	int number_crt = atoi(number_crt_ascii);

	// Find the node corresponding to the last character in "prefix". From this
	// node, we will start searching for other words for autocomplete.
	trie_node_t *node_prefix = trie_search_path(trie, prefix);

	// If there is no such node, there will not be any words to find.
	if (!node_prefix) {
		// In the case "0", we will print the error three times because we had
		// to print three words.
		if (number_crt == 0)
			for (int i = 0; i < 3; i++)
				printf("No words found\n");
		else
			printf("No words found\n");
		return;
	}

	switch (number_crt) {
	case 1:	 // Lexicographically smallest
		autocomplete_smallest_lexico(node_prefix, prefix);
		break;

	case 2:	 // Shortest
		autocomplete_shortest(node_prefix, prefix);
		break;

	case 3:	 // Most frequent
		autocomplete_most_frequent(node_prefix, prefix);
		break;

	case 0:	 // All
		autocomplete_smallest_lexico(node_prefix, prefix);
		autocomplete_shortest(node_prefix, prefix);
		autocomplete_most_frequent(node_prefix, prefix);
	}
}

int compare_words(char *initial_word, char *current_word, int nr_max_diff)
{
	// Compare each letter from "initial_word" to its counterpart in
	// "current_word".
	int len = strlen(initial_word);
	int kon = 0;
	for (int i = 0; i < len; i++) {
		if (initial_word[i] != current_word[i])
			kon++;
	}
	if (kon <= nr_max_diff)
		return 1;
	return 0;
}

void autocorrect_helper(trie_node_t *node, char *initial_word, int k,
						int max_len, int *current_len, char *current_word,
						int *found_words)
{
	if (*current_len == max_len) {
		if (node->end_of_word == 1) {
			if (compare_words(initial_word, current_word, k) == 1) {
				current_word[*current_len] = '\0';
				*found_words = 1;
				printf("%s\n", current_word);
			}
		}
		return;	 // Go back a level if we reach the "initial_word"'s length
	}

	// Go back a level if we find ends of words that are shorter than
	// "initial_word"
	if (!node->children)
		return;

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			current_word[*current_len] = i + 'a';
			*current_len = *current_len + 1;
			autocorrect_helper(node->children[i], initial_word, k, max_len,
							   current_len, current_word, found_words);
			*current_len = *current_len - 1;
		}
	}
}

void autocorrect_command(trie_t *trie, char *delim)
{
	// Extracts the word we have to autocorrect from the previously read line
	// and the maximum number of characters that can differ.
	char *initial_word = strtok(NULL, delim);
	char *nr_max_diff_ascii = strtok(NULL, delim);
	int nr_max_diff = atoi(nr_max_diff_ascii);

	// We will recursively check for words just until we reach "initial_word"'s
	// length, because the found words need to have the same length as it.
	int max_len = strlen(initial_word);

	// Create an auxiliary word that we will update for each char that we go
	// through in trie. It helps us to keep track of where we are in the trie.
	int current_len = 0;
	char *current_word = malloc(sizeof(char) * (max_len + 1));
	DIE(!current_word, "malloc failed");
	current_word[0] = '\0';

	// Keeps track if we found any words.
	int found_words = 0;

	autocorrect_helper(trie->root, initial_word, nr_max_diff, max_len,
					   &current_len, current_word, &found_words);

	if (found_words == 0)
		printf("No words found\n");
	free(current_word);
}
