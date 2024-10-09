// Copyright Diaconescu Stefania Clara 313CA 2023-2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include "doubly_list.h"

// Functie care trebuie apelata pentru alocarea si initializarea unei liste.
// (Setare valori initiale pentru campurile specifice structurii list).
doubly_linked_list_t*
dll_create(unsigned int data_size, unsigned int nr_bytes_per_node)
{
	doubly_linked_list_t *list = malloc(1 * sizeof(doubly_linked_list_t));
	DIE(!list, "malloc failed");

	list->head = NULL;
	list->size = 0;
	list->data_size = data_size;
	list->nr_bytes_per_node = nr_bytes_per_node;

	return list;
}

//  Functia adauga un nod in lista pe pozitia n.
void
dll_add_nth_node(doubly_linked_list_t *list, unsigned int n,
				 unsigned long start_addr, unsigned int bytes_per_node,
				 unsigned long index)
{
	dll_node_t *node = malloc(1 * sizeof(dll_node_t));
	DIE(!node, "malloc failed");
	node->data = malloc(sizeof(info_t));
	DIE(!node->data, "malloc failed");
	((info_t *)node->data)->input = NULL;

	if (!list->head) {
		node->next = NULL;
		node->prev = NULL;
		list->head = node;
	} else if (n == 0) {
		node->next = list->head;
		node->prev = NULL;
		node->next->prev = node;
		list->head = node;
	} else if (n >= list->size) {
		dll_node_t *current = list->head;

		while (current->next)
			current = current->next;

		current->next = node;
		node->next = NULL;
		node->prev = current;
	} else {
		dll_node_t *current = list->head;
		unsigned int i = 0;

		while (i < n - 1 && current->next) {
			current = current->next;
			i++;
		}

		node->next = current->next;
		node->prev = current;
		current->next->prev = node;
		current->next = node;
	}
	((info_t *)node->data)->nr_bytes = bytes_per_node;
	((info_t *)node->data)->index = index;
	((info_t *)node->data)->start_addr = start_addr;
	list->size++;
}

// Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
// parametru. Functia intoarce un pointer spre acest nod proaspat eliminat din
// lista.
dll_node_t*
dll_remove_nth_node(doubly_linked_list_t *list, unsigned int n)
{
	if (!list || !list->head)
		return NULL;

	if (n == 0) {
		dll_node_t *current = list->head;

		if (list->head->next)
			list->head->next->prev = NULL;

		list->head = list->head->next;
		list->size--;
		return current;
	}
	if (n >= list->size - 1) {
		dll_node_t *current = list->head;

		while (current->next)
			current = current->next;

		current->prev->next = NULL;
		list->size--;
		return current;
	}

	dll_node_t *current = list->head;
	unsigned int i = 0;

	while (i < n) {
		current = current->next;
		i++;
	}

	current->prev->next = current->next;
	current->next->prev = current->prev;
	list->size--;
	current->next = NULL;
	return current;
}

// Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
// sfarsit elibereaza memoria folosita de structura lista.
void
dll_free(doubly_linked_list_t **pp_list)
{
	if (!(*pp_list))
		return;
	if (!(*pp_list)->head) {
		free(*pp_list);
		(*pp_list) = NULL;
		return;
	}

	dll_node_t *current = (*pp_list)->head;

	while (current) {
		dll_node_t *next = current->next;
		if (((info_t *)current->data)->input)
			free(((info_t *)current->data)->input);
		if (current->data)
			free(current->data);
		free(current);
		current = next;
	}
	free(*pp_list);
	(*pp_list) = NULL;
}

// Functia printeaza adresele blocurilor din lista de blocuri nealocate.
void
dll_print_addr_list(doubly_linked_list_t *list)
{
	dll_node_t *current;
	if (!list->head)
		return;
	current = list->head;

	while (current->next) {
		printf(" 0x%lx", ((info_t *)current->data)->start_addr);
		current = current->next;
	}
	printf(" 0x%lx", ((info_t *)current->data)->start_addr);

	printf("\n");
}

// Functia printeaza adresele blocurilor din lista de blocuri alocate.
void
dll_print_alloc_list(doubly_linked_list_t *list)
{
	dll_node_t *current;
	if (!list->head)
		return;
	current = list->head;

	while (current->next) {
		printf(" (0x%lx - %d)", ((info_t *)current->data)->start_addr,
			   ((info_t *)current->data)->nr_bytes);
		current = current->next;
	}
	printf(" (0x%lx - %d)", ((info_t *)current->data)->start_addr,
		   ((info_t *)current->data)->nr_bytes);
}
