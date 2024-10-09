// Copyright Diaconescu Stefania Clara 313CA 2023-2024

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "memory_allocator.h"
#include "doubly_list.h"

#define MAX_SIZE 650
#define MAX_SIZE_INP 600

int main(void)
{
	char command[MAX_SIZE] = "";
	unsigned long address;
	unsigned int nr_bytes;
	heap_t *heap;
	doubly_linked_list_t *allocated;
	index_t *index = malloc(1 * sizeof(index_t));
	int seg = 0;
	// sunt initializate campurile specifice structurii index
	index->index_free = 0;
	index->index_malloc = 0;
	index->nr_fragmentations = 0;
	index->mem_alloc = 0;

	scanf("%s", command);

	// sunt apelate functiile corespunzatoare comenzilor primite
	while (strcmp(command, "DESTROY_HEAP") != 0) {
		if (strcmp(command, "INIT_HEAP") == 0) {
			unsigned long start_addr;
			unsigned int nr_lists, type;
			scanf("%lx %d %d %d", &start_addr, &nr_lists, &nr_bytes, &type);
			heap = alloc_heap(start_addr, nr_lists, nr_bytes, type);
			init_heap(&heap, nr_lists, nr_bytes);
			allocated = dll_create(sizeof(char *), nr_bytes);
		} else if (strcmp(command, "MALLOC") == 0) {
			scanf("%d", &nr_bytes);
			malloc_bytes(&heap, &allocated, nr_bytes, index);
		} else if (strcmp(command, "FREE") == 0) {
			scanf("%lx", &address);
			free_memory(&heap, &allocated, index, address);
		} else if (strcmp(command, "READ") == 0) {
			scanf("%lx %d", &address, &nr_bytes);
			read(&allocated, address, &seg, nr_bytes);
		} else if (strcmp(command, "WRITE") == 0) {
			scanf("%lx", &address);
			char *date = malloc(MAX_SIZE_INP * sizeof(char)), *p;
			fgets(command, MAX_SIZE_INP, stdin);
			p = strtok(command, "\"");
			p = strtok(NULL, "\"");
			strcpy(date, p);
			date[strlen(date)] = '\0';
			p = strtok(NULL, "\n");
			strcpy(command, p + 1);
			int nr_bytes = atoi(command);
			write(&allocated, address, &seg, date, nr_bytes);
			free(date);
		} else if (strcmp(command, "DUMP_MEMORY") == 0) {
			dump_memory(heap, allocated, index);
		} else {
			printf("INVALID COMMAND\n");
		}
		if (seg == 1) {
			strcpy(command, "DESTROY_HEAP");
			dump_memory(heap, allocated, index);
		} else {
			scanf("%s", command);
		}
	}

	// la final este eliberata toata memoria alocata
	destroy_heap(&heap, &allocated);
	free(index);

	return 0;
}
