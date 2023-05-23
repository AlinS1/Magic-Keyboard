#include "commands_mk.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

#define INF 2147483647

void insert_command(trie_t *trie, char *delim)
{
	char *word = strtok(NULL, delim);
	// printf("word:%s\n", word);
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
		// printf("word:%s\n", word);
		trie_insert(trie, word);
	}
	// printf("loaded file\n");
	fclose(in);
}

void remove_command(trie_t *trie, char *delim)
{
	char *word = strtok(NULL, delim);

	trie_remove(trie, word);
}

void autocomplete_smallest_lexico(trie_node_t *node, char *prefix)
{
	if (node->end_of_word == 1) {
		printf("%s\n", prefix);
		return;
	}

	int nr_chars = 0;
	int len = strlen(prefix);
	nr_chars += len;
	char *smallest_word = malloc(sizeof(char) * MAX_LINE);
	strcpy(smallest_word, prefix);

	trie_node_t *current_node = node;
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
	if (node->end_of_word == 1) {
		if (*nr_chars < *shortest_len) {
			*shortest_len = *nr_chars;
			strcpy(shortest_word, aux_word);
			// printf("shortest_ap:%d word:%s\n", *shortest_len,
			//	   shortest_word);
		}
	}

	// If it's a leaf node, it's surely an end of word
	if (!node->children) {
		*nr_chars = *nr_chars - 1;
		// printf("word_found:%s\n", aux_word);
		return;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			aux_word[*nr_chars] = i + 'a';
			*nr_chars = *nr_chars + 1;
			aux_word[*nr_chars] = '\0';

			autocomplete_shortest_helper(node->children[i], aux_word, nr_chars,
										 shortest_len, shortest_word);
		}
	}
	aux_word[*nr_chars] = '\0';
	*nr_chars = *nr_chars - 1;
}

void autocomplete_shortest(trie_node_t *node, char *prefix)
{
	if (node->end_of_word == 1) {
		printf("%s\n", prefix);
		return;
	}
	// Create an auxiliary word that contains the prefix from which we will
	// start.
	int nr_chars = 0;
	int len = strlen(prefix);
	nr_chars += len;
	char *aux_word = malloc(sizeof(char) * MAX_LINE);
	strcpy(aux_word, prefix);

	// Initialize the minimum length and its corresponding word.
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

void autocomplete_most_frequent_helper(trie_node_t *node, char *final_word,
									   int *nr_chars, int *max_ap,
									   char *max_ap_word)
{
	// If it's a leaf node, it's surely an end of word
	if (!node->children) {
		final_word[*nr_chars] = '\0';
		*nr_chars = *nr_chars - 1;
		if (node->appearances > *max_ap) {
			*max_ap = node->appearances;
			strcpy(max_ap_word, final_word);
			// printf("max_ap:%d word:%s\n", *max_ap, max_ap_word);
		}
		// printf("word_found:%s\n", final_word);
		return;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			// printf("car:%c\n", i + 'a');
			final_word[*nr_chars] = i + 'a';
			*nr_chars = *nr_chars + 1;
			final_word[*nr_chars] = '\0';
			// If it's an end of word, verify the number of apperances
			if (node->children[i]->end_of_word == 1) {
				if (node->children[i]->appearances > *max_ap) {
					*max_ap = node->children[i]->appearances;
					strcpy(max_ap_word, final_word);
					// printf("max_ap:%d word:%s\n", *max_ap, max_ap_word);
				}
			}
			autocomplete_most_frequent_helper(node->children[i], final_word,
											  nr_chars, max_ap, max_ap_word);
		}
	}

	// If there is no child left to continue on, we go back a character.
	final_word[*nr_chars] = '\0';
	*nr_chars = *nr_chars - 1;
}

void autocomplete_most_frequent(trie_node_t *node, char *prefix)
{
	// Create an auxiliary word that contains the prefix from which we will
	// start.
	int nr_chars = 0;
	int len = strlen(prefix);
	nr_chars += len;
	char *aux_word = malloc(sizeof(char) * MAX_LINE);
	strcpy(aux_word, prefix);

	// Initialize the maximum number of appearances and its corresponding word.
	int max_ap = 0;
	char max_ap_word[MAX_LINE];
	strcpy(max_ap_word, prefix);

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
	// printf("prefix:%s|; nr:%d|\n", prefix, number_crt);

	trie_node_t *node_prefix = trie_search_path(trie, prefix);
	if (!node_prefix) {
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

// Returns 1 if the words have a maximum of "nr_max_diff" different letters
int compare_words(char *initial_word, char *current_word, int nr_max_diff)
{
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

// to be modified: current_len, current_word
// when current_len == max_len && word.end  verificam
void autocorrect_helper(trie_node_t *node, char *initial_word, int k,
						int max_len, int *current_len, char *current_word,
						int *words_found)
{
	if (*current_len == max_len) {
		if (node->end_of_word == 1) {
			if (compare_words(initial_word, current_word, k) == 1) {
				current_word[*current_len] = '\0';
				*words_found = 1;
				printf("%s\n", current_word);
			}
		}
		*current_len = *current_len - 1;
		// current_word[*current_len] = '\0';

		return;	 // go back if we reach the maximum length
	}

	if (!node->children) {
		*current_len = *current_len - 1;
		// current_word[*current_len] = '\0';

		return;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			current_word[*current_len] = i + 'a';
			*current_len = *current_len + 1;
			// current_word[*current_len] = '\0';
			autocorrect_helper(node->children[i], initial_word, k, max_len,
							   current_len, current_word, words_found);
		}
	}

	// current_word[*current_len] = '\0';
	*current_len = *current_len - 1;
}

void autocorrect_command(trie_t *trie, char *delim)
{
	char *initial_word = strtok(NULL, delim);
	char *k_ascii = strtok(NULL, delim);
	int k = atoi(k_ascii);
	int max_len = strlen(initial_word);

	int current_len = 0;
	char *current_word = malloc(sizeof(char) * (max_len + 1));
	current_word[0] = '\0';
	int found_words = 0;
	autocorrect_helper(trie->root, initial_word, k, max_len, &current_len,
					   current_word, &found_words);
	if (found_words == 0)
		printf("No words found\n");
	free(current_word);
}