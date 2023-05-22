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

#endif	// COMMANDS_MK_H