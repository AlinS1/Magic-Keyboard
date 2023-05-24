# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -Wshadow -Wpedantic -std=c99 -O0 -g

# define targets
TARGETS=kNN mk
TRIE=trie
COMMANDS_MK=commands_mk
MK=mk
KNN=kNN
KDTREE=kd_tree
COMMANDS_KNN=commands_knn

#define object-files
OBJ=mk.o commands_mk.o trie.o kNN.o kd_tree.o commands_knn.o

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
kNN: kNN.o $(KDTREE).o $(COMMANDS_KNN).o
	$(CC) $(CFLAGS) $^ -o $@ -lm

kNN.o: kNN.c
	$(CC) $(CFLAGS) $^ -c

kd_tree.o: $(KDTREE).c $(KDTREE).h
	$(CC) $(CFLAGS) $^ -c

commands_knn.o: $(COMMANDS_KNN).c $(COMMANDS_KNN).h
	$(CC) $(CFLAGS) $^ -c


# OTHERS
pack:
	zip -FSr 314CA_SimileaAlin-Andrei_Tema3.zip README.md Makefile *.c *.h

clean:
	rm -f $(TARGETS) $(OBJ)

.PHONY: pack clean
