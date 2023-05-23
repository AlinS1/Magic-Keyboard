#ifndef COMMANDS_MK_H
#define COMMANDS_MK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

#define MAX_LINE 100

// Tranforms a command string into a representative number in order to use
// switch case
int command_type_to_number(char *command);

// Inserts the word from the previously read line into the trie
void insert_command(trie_t *trie, char *delim);

// Loads the file from the previously read line and inserts all its words into
// the trie
void load_command(trie_t *trie, char *delim);

// Removes the word from the previously read line from the trie
void remove_command(trie_t *trie, char *delim);

// ================================
// ==== AUTOCOMPLETE FUNCTIONS ====
// ================================

// Finds the smallest lexicographically word that starts with "prefix" and
// prints it
void autocomplete_smallest_lexico(trie_node_t *end_prefix_node, char *prefix);

// Recursively checks all the words from our trie that contain the previously
// given "prefix" If the found word's length is shorter than the previous
// shortest we found, we update the shortest length and the shortest word.
// aux_word -> current set of characters
// nr_chars -> current number of characters in "aux_word"
void autocomplete_shortest_helper(trie_node_t *node, char *aux_word,
								  int *nr_chars, int *shortest_len,
								  char *shortest_word);

// Finds the shortest word that starts with "prefix" and prints it.
void autocomplete_shortest(trie_node_t *node, char *prefix);

// Recursively checks all the words starting with the prefix. If the number of
// appearances is higher than the previous maximum we found, we update the
// maximum number and the word with maximum appearances.
// aux_word -> current set of characters
// nr_chars -> current number of characters in "aux_word"
void autocomplete_most_frequent_helper(trie_node_t *node, char *aux_word,
									   int *nr_chars, int *max_ap,
									   char *max_ap_word);

// Finds the word with the greatest number of appearances that starts with
// "prefix" and prints it.
void autocomplete_most_frequent(trie_node_t *node, char *prefix);

// Deals with the autocomplete commands' cases.
void autocomplete_command(trie_t *trie, char *delim);

// ===============================
// ==== AUTOCORRECT FUNCTIONS ====
// ===============================

// Returns 1 if the words have a maximum of "nr_max_diff" different letters
int compare_words(char *initial_word, char *current_word, int nr_max_diff);

// Recursively checks all the words starting with the trie's root and have the
// size of "max_len". When we reach max_len, we compare "current_word" with the
// "initial_word" and print it if it is a valid autocorrection.
void autocorrect_helper(trie_node_t *node, char *initial_word, int nr_max_diffk,
						int max_len, int *current_len, char *current_word,
						int *found_words);

// Deals with the autocorrect command.
void autocorrect_command(trie_t *trie, char *delim);

#endif	// COMMANDS_MK_H
