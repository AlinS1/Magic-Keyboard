# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -Wshadow -Wpedantic -std=c99 -O0 -g

# define targets
TARGETS=kNN mk
TRIE=trie
COMMANDS_MK=commands_mk
MK=mk
KNN=kNN

#define object-files
OBJ=mk.o kNN.o commands_mk.o trie.o

build: $(TARGETS)

# TASK_1
mk: mk.o $(TRIE).o $(COMMANDS_MK).o
	$(CC) $(CFLAGS) $^ -o $@

mk.o: mk.c
	$(CC) $(CFLAGS) $^ -c

trie.o: $(TRIE).c $(TRIE).h
	$(CC) $(CFLAGS) $^ -c

commands_mk.o: $(COMMANDS_MK).c $(COMMANDS_MK).h
	$(CC) $(CFLAGS) $^ -c


# TASK_2
kNN: kNN.o
	$(CC) $(CFLAGS) $^ -o $@

kNN.o: kNN.c
	$(CC) $(CFLAGS) $^ -c



pack:
	zip -FSr 314CA_SimileaAlin-Andrei_Tema3.zip README.md Makefile *.c *.h

clean:
	rm -f $(TARGETS) $(OBJ)

.PHONY: pack clean
