#ifndef COMMANDS_MK_H
#define COMMANDS_MK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

#define MAX_LINE 100

void insert_command(trie_t *trie, char *delim);
void load_command(trie_t *trie, char *delim);
void remove_command(trie_t *trie, char *delim);


// AUTOCOMPLETE FUNCTIONS
void autocomplete_smallest_lexico(trie_node_t *node, char *prefix);

void autocomplete_shortest_helper(trie_node_t *node, char *aux_word,
								  int *nr_chars, int *shortest_len,
								  char *shortest_word);
void autocomplete_shortest(trie_node_t *node, char *prefix);

// Recursively checks all the words' appearances starting with the prefix.
// If the number of appearances is higher than the previous
// maximum("max_ap") we found, we update the maximum number and the word
// with maximum appearances("max_ap_word").
void autocomplete_most_frequent_helper(trie_node_t *node, char *final_word,
									   int *nr_chars, int *max_ap,
									   char *max_ap_word);
void autocomplete_most_frequent(trie_node_t *node, char *prefix);

void autocomplete_command(trie_t *trie, char *delim);

// AUTOCORRECT FUNCTIONS

void autocorrect_command(trie_t *trie, char *delim);




#endif	// COMMANDS_MK_H