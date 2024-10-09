// Copyright Diaconescu Stefania Clara 313CA 2023-2024

#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "doubly_list.h"

typedef struct heap_t {
	unsigned long start_addr;
	unsigned int nr_lists, nr_bytes, type, capacity, memory;
	doubly_linked_list_t **segr_lists;
} heap_t;

typedef struct index_t {
	int index_malloc;
	int index_free;
	int nr_fragmentations;
	int mem_alloc;
} index_t;

heap_t *
alloc_heap(unsigned long start_addr, unsigned int nr_lists,
		   unsigned int nr_bytes, unsigned int type);

void
init_heap(heap_t **heap, unsigned int nr_lists, unsigned int nr_bytes);

doubly_linked_list_t **
realloc_mat(doubly_linked_list_t **list, unsigned int *capacity);

int
verify_heap(heap_t **heap, unsigned int nr_bytes, unsigned int *i, int *equal);

unsigned int
pos_in_list(doubly_linked_list_t *list, unsigned long address);

void
insert_lists(heap_t **heap, unsigned int nr_bytes_residue, unsigned int *i);

int
find_address(doubly_linked_list_t *list, unsigned long address);

void
free_memory(heap_t **heap, doubly_linked_list_t **allocated, index_t *index,
			unsigned long address);

void
malloc_bytes(heap_t **heap, doubly_linked_list_t **allocated,
			 unsigned int nr_bytes, index_t *index);
int
find_address(doubly_linked_list_t *list, unsigned long address);

void
free_memory_type0(heap_t **heap, dll_node_t *node);

int
find_index(doubly_linked_list_t *list, unsigned long address,
		   unsigned int index, unsigned int nr_bytes);

void
free_memory_type1(heap_t ***heap, dll_node_t *node, unsigned long address);

void
free_memory(heap_t **heap, doubly_linked_list_t **allocated, index_t *index,
			unsigned long address);

unsigned int
find_end(unsigned int *bytes, unsigned int l, doubly_linked_list_t **allocated,
		 unsigned int nr_bytes);

void
write(doubly_linked_list_t **allocated, unsigned long address, int *seg,
	  char *date, unsigned int  nr_bytes);

void
read(doubly_linked_list_t **allocated, unsigned long address, int *seg,
	 unsigned int nr_bytes);

unsigned int
free_mem(heap_t *heap);

unsigned int
free_blocks(heap_t *heap);

void
dump_memory(heap_t *heap, doubly_linked_list_t *allocated, index_t *index);

void
destroy_heap(heap_t **heap, doubly_linked_list_t **allocated);

#endif
