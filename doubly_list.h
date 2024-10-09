// Copyright Diaconescu Stefania Clara 313CA 2023-2024

#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>

#define DIE(assertion, call_description)				\
	do {								\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",			\
					__FILE__, __LINE__);		\
			perror(call_description);			\
			exit(errno);				        \
		}							\
	} while (0)

typedef struct info_t {
	unsigned long start_addr, index;
	unsigned int nr_bytes;
	char *input;
} info_t;

typedef struct dll_node_t {
	void *data; /* Pentru ca datele stocate sa poata avea orice tip, folosim un
				   pointer la void. */
	struct dll_node_t *prev, *next;
} dll_node_t;

typedef struct doubly_linked_list_t {
	dll_node_t *head;
	unsigned int data_size;
	unsigned int size;
	unsigned int nr_bytes_per_node;
} doubly_linked_list_t;

doubly_linked_list_t*
dll_create(unsigned int data_size, unsigned int nr_bytes_per_node);

void
dll_add_nth_node(doubly_linked_list_t *list, unsigned int n,
				 unsigned long start_addr, unsigned int bytes_per_node,
				 unsigned long index);

dll_node_t*
dll_remove_nth_node(doubly_linked_list_t *list, unsigned int n);

void
dll_free(doubly_linked_list_t **pp_list);

void
dll_print_addr_list(doubly_linked_list_t *list);

void
dll_print_alloc_list(doubly_linked_list_t *list);

#endif
