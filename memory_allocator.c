// Copyright Diaconescu Stefania Clara 313CA 2023-2024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "memory_allocator.h"
#include "doubly_list.h"

#define MAX_SIZE 650
#define MAX_SIZE_INP 600

// Functia aloca si initializeaza o structura de tip heap_t
heap_t *
alloc_heap(unsigned long start_addr, unsigned int nr_lists,
		   unsigned int nr_bytes, unsigned int type)
{
	heap_t *heap = malloc(1 * sizeof(heap_t));
	DIE(!heap, "malloc failed");

	heap->start_addr = start_addr;
	heap->nr_lists = nr_lists;
	heap->capacity = nr_lists;
	heap->nr_bytes = nr_bytes;
	heap->type = type;
	heap->memory = nr_lists * nr_bytes;

	return heap;
}

// Functia construieste structura de date pentru un heap care incepe de la
// adresa citita in comanda
void init_heap(heap_t **heap, unsigned int nr_lists, unsigned int nr_bytes)
{
	unsigned long index = 0;
	(*heap)->segr_lists = malloc(nr_lists * sizeof(doubly_linked_list_t *));
	DIE(!(*heap)->segr_lists, "malloc failed");

	unsigned long first_addr = (*heap)->start_addr;
	unsigned int bytes_per_node = 8;
	// este creat un numar de liste egal cu nr_lists, pentru fiecare nod fiind
	// calculata adresa initiala
	for (unsigned int i = 0; i < nr_lists; i++) {
		(*heap)->segr_lists[i] = dll_create(sizeof(info_t), bytes_per_node);

		unsigned int size = nr_bytes / bytes_per_node;
		for (unsigned int j = 0; j < size; j++) {
			dll_add_nth_node((*heap)->segr_lists[i], j, first_addr,
							 bytes_per_node, index);
			first_addr += bytes_per_node;
			index++;
		}
		bytes_per_node *= 2;
	}
}

// Functia realoca vectorul de liste, iar capacitatea este dublata
doubly_linked_list_t **
realloc_mat(doubly_linked_list_t **list, unsigned int *capacity)
{
	doubly_linked_list_t **aux;
	*capacity *= 2;
	aux = realloc(list, *capacity * sizeof(doubly_linked_list_t *));
	DIE(!aux, "realloc failed");

	list = aux;
	return list;
}

// Functia verifica daca exista in vector lista cu numarul de bytes al fiecarui
// nod egal cu numarul de bytes al nodului de adaugat
int verify_heap(heap_t **heap, unsigned int nr_bytes, unsigned int *i,
				int *equal)
{
	int ok = 0;

	while (*i < (*heap)->nr_lists &&
		   ((*heap)->segr_lists[*i]->nr_bytes_per_node < nr_bytes ||
			!(*heap)->segr_lists[(*i)]->head))
		(*i)++;

	if ((*i) == (*heap)->nr_lists)
		return ok;

	if ((*heap)->segr_lists[(*i)]->nr_bytes_per_node == nr_bytes)
		(*equal) = (*i);

	ok = 1;

	// daca ok = 1, dupa ce se iese din while(), i va avea valoarea indicelui
	// din vector unde se afla lista cu spatiul necesar
	// daca exista o lista ce contine noduri de dimensiune egala cu cea ceruta,
	// se va salva indicele acesteia in variabila equal

	return ok;
}

// Functia returneaza pozitia la care trebuie adaugat nodul astfel incat
// adresele din lista sa fie in ordine crescatoare
unsigned int
pos_in_list(doubly_linked_list_t *list, unsigned long address)
{
	unsigned int l = 0;
	dll_node_t *current = list->head;
	while (address > ((info_t *)current->data)->start_addr && current->next) {
		current = current->next;
		l++;
	}
	if (address > ((info_t *)current->data)->start_addr)
		l++;
	return l;
}

// Functia insereaza o lista in vectorul de liste
void insert_lists(heap_t **heap, unsigned int nr_bytes_residue,
				  unsigned int *i)
{
	*i = 0;
	while (*i < (*heap)->nr_lists &&
		   (*heap)->segr_lists[*i]->nr_bytes_per_node < nr_bytes_residue)
		(*i)++;

	// sunt mutate cu o pozitie la dreapta toate listele de la finalul
	// vectorului pana la pozitia i
	if (*i < (*heap)->nr_lists) {
		for (unsigned int j = (*heap)->nr_lists; j > *i; j--)
			(*heap)->segr_lists[j] = (*heap)->segr_lists[j - 1];
	}
	(*heap)->segr_lists[*i] = dll_create(sizeof(info_t), nr_bytes_residue);
	(*heap)->segr_lists[*i]->nr_bytes_per_node = nr_bytes_residue;
	(*heap)->nr_lists++;
}

// Functia cauta si aloca un bloc de memorie de dimensiuea ceruta
void malloc_bytes(heap_t **heap, doubly_linked_list_t **allocated,
				  unsigned int nr_bytes, index_t *index)
{
	unsigned int ok = 0, i = 0, j = 0, ok2 = 0;
	int equal = -1;
	dll_node_t *node;

	// se verifica daca exista blocuri de dimensiune mai mare sau egala cu cea
	// necesara
	// daca exista, acesta va fi scos din lista
	ok = verify_heap(heap, nr_bytes, &i, &equal);

	if (ok == 0) {
		printf("Out of memory\n");
		return;
	}

	(index->index_malloc)++;
	node = dll_remove_nth_node((*heap)->segr_lists[i], 0);

	if (equal == -1) {
		// daca nu exista o lista ce contine noduri de dimensiune egala cu cea
		// ceruta, blocul este fragmentat, iar ceea ce ramane se adauga in
		// lista corespunzatoare dimensiunii
		unsigned int bytes_residue = ((info_t *)node->data)->nr_bytes -
									 nr_bytes;
		unsigned int addr_residue = ((info_t *)node->data)->start_addr +
									nr_bytes;
		index->nr_fragmentations++;
		while (j < (*heap)->nr_lists && ok2 == 0) {
			if ((*heap)->segr_lists[j]->nr_bytes_per_node == bytes_residue &&
				(*heap)->segr_lists[j]->head)
				ok2 = 1;
			else
				j++;
		}

		if (ok2 == 1) {
			unsigned int l = pos_in_list((*heap)->segr_lists[j],
										 ((info_t *)node->data)->start_addr);
			dll_add_nth_node((*heap)->segr_lists[j], l, addr_residue,
							 bytes_residue, ((info_t *)node->data)->index);
		} else {
			// daca nu exista lista in care nr_bytes_per_node este egal cu
			// dimensiunea din nod ramasa nealocata, aceasta va fi inserata in
			// vector
			if ((*heap)->nr_lists + 1 > (*heap)->capacity)
				(*heap)->segr_lists = realloc_mat((*heap)->segr_lists,
												  &((*heap)->capacity));
			insert_lists(heap, bytes_residue, &i);
			dll_add_nth_node((*heap)->segr_lists[i], 0, addr_residue,
							 bytes_residue, ((info_t *)node->data)->index);
		}
	}

	((info_t *)node->data)->nr_bytes = nr_bytes;
	index->mem_alloc += nr_bytes;

	// nodul extras este adaugat in lista cu noduri alocate
	if (!(*allocated)->head) {
		dll_add_nth_node((*allocated), 0, ((info_t *)node->data)->start_addr,
						 nr_bytes, ((info_t *)node->data)->index);
	} else {
		unsigned int l = pos_in_list((*allocated),
						 ((info_t *)node->data)->start_addr);
		dll_add_nth_node((*allocated), l, ((info_t *)node->data)->start_addr,
						 nr_bytes, ((info_t *)node->data)->index);
	}
	if (node->data)
		free(node->data);
	free(node);
}

// Functia cauta o adresa in nodurile din lista data ca parametru si returneaza
// pozitia
int find_address(doubly_linked_list_t *list, unsigned long address)
{
	unsigned int l = 0;
	if (!list->head)
		return -1;

	dll_node_t *current = list->head;
	while (current->next && address != ((info_t *)current->data)->start_addr) {
		current = current->next;
		l++;
	}
	if (address != ((info_t *)current->data)->start_addr)
		l++;
	return l;
}

// Functia da free() la o memorie alocata anterior, adaugand in heap nodul
// extras din lista cu noduri alocate
void free_memory_type0(heap_t **heap, dll_node_t *node)
{
	unsigned int j = 0, l = 0, i = 0;
	int ok = 0;

	while (j < (*heap)->nr_lists && ok == 0) {
		if ((*heap)->segr_lists[j]->nr_bytes_per_node ==
			((info_t *)node->data)->nr_bytes && (*heap)->segr_lists[j]->head)
			ok = 1;
		else
			j++;
	}
	if (ok == 1) {
		l = pos_in_list((*heap)->segr_lists[j],
						((info_t *)node->data)->start_addr);
		dll_add_nth_node((*heap)->segr_lists[j], l,
						 ((info_t *)node->data)->start_addr, ((info_t *)
						 node->data)->nr_bytes, ((info_t *)node->data)->index);
	} else {
		// daca nu exista lista in care nr_bytes_per_node este egal cu
		// dimensiunea nodului caruia i se da free(), aceasta va fi inserata in
		// vector
		if ((*heap)->nr_lists + 1 > (*heap)->capacity)
			(*heap)->segr_lists = realloc_mat((*heap)->segr_lists,
											  &((*heap)->capacity));
		insert_lists(heap, ((info_t *)node->data)->nr_bytes, &i);
		dll_add_nth_node((*heap)->segr_lists[i], 0, ((info_t *)node->data)->
						 start_addr, ((info_t *)node->data)->nr_bytes,
						 ((info_t *)node->data)->index);
	}

	if (((info_t *)node->data)->input)
		free(((info_t *)node->data)->input);

	if (node->data)
		free(node->data);

	free(node);
}

// Functia cauta in lista data ca parametru nodurile cu acelasi index si
// adresele una in continuarea celeilalte
int
find_index(doubly_linked_list_t *list, unsigned long address,
		   unsigned int index, unsigned int nr_bytes)
{
	unsigned int i = 0, ok = 0, pos = -1;
	if (!list->head)
		return -1;

	dll_node_t *current = list->head;
		while (current->next && ok == 0) {
			if (((info_t *)current->data)->index == index && (((info_t *)
				current->data)->start_addr == address + nr_bytes || ((info_t *)
				current->data)->start_addr == address -
				((info_t *)current->data)->nr_bytes)) {
				pos = i;
				ok = 1;
			}
			current = current->next;
			i++;
		}
		if (((info_t *)current->data)->index == index && (((info_t *)
			current->data)->start_addr == address + nr_bytes || ((info_t *)
			current->data)->start_addr == address - ((info_t *)current->data)
			->nr_bytes))
			pos = i;

	return pos;
}

// Functia da free() la o memorie alocata anterior, adaugand in heap nodul
// extras din lista cu noduri alocate, totodata reconstituind blocurile
// initiale
void
free_memory_type1(heap_t ***heap, dll_node_t *node, unsigned long address)
{
	unsigned int i = 0;
	int pos = 0;

	while (i < (**heap)->nr_lists) {
		if (!(**heap)->segr_lists[i]->head) {
			i++;
		} else {
			// verifica daca exista noduri ce pot fi lipite de cel caruia i s-a
			// dat free() (au acelasi index, iar adresele sunt continue)
			// daca exista, se modifica numarul de bytes si adresa daca este
			// cazul, indexul i fiind pozitionat din nou la inceputul
			// vectorului
			pos = find_index((**heap)->segr_lists[i], address,
							 ((info_t *)node->data)->index,
							 ((info_t *)node->data)->nr_bytes);
			if (pos != -1) {
				dll_node_t *current = (**heap)->segr_lists[i]->head;
				if (pos > 1) {
					for (int j = 0; j < pos - 1; j++)
						current = current->next;
				} else if (pos == 1) {
					current = current->next;
				}

				if (((info_t *)node->data)->start_addr < ((info_t *)
					current->data)->start_addr)
					((info_t *)current->data)->start_addr =
											((info_t *)node->data)->start_addr;
				((info_t *)current->data)->nr_bytes +=
											((info_t *)node->data)->nr_bytes;

				if (node->data)
					free(node->data);
				free(node);
				node = dll_remove_nth_node((**heap)->segr_lists[i], pos);
				i = -1;
			}
			i++;
		}
	}
	free_memory_type0(*heap, node);
}

// Functia da free() nodului cu adresa de inceput data
void free_memory(heap_t **heap, doubly_linked_list_t **allocated,
				 index_t *index, unsigned long address)
{
	if (address == 0)
		return;
	if (!((*allocated)->head)) {
		printf("Invalid free\n");
		return;
	}

	// verifica daca nodul de la adresa respectiva a fost alocat
	unsigned int l = find_address((*allocated), address);
	if (l == (*allocated)->size) {
		printf("Invalid free\n");
		return;
	}

	dll_node_t *node = dll_remove_nth_node((*allocated), l);
	index->mem_alloc -= ((info_t *)node->data)->nr_bytes;

	// in functie de tip, apeleaza functia de free corespunzatoare
	if ((*heap)->type == 0)
		free_memory_type0(heap, node);
	else
		free_memory_type1(&heap, node, address);

	index->index_free++;
}

// Functia cauta pozitia ultimului nod necesar pentru a citi/scrie, pana
// spatiul este suficient si verifica daca toti octetii din interval au fost
// alocati
unsigned int
find_end(unsigned int *bytes, unsigned int l, doubly_linked_list_t **allocated,
		 unsigned int nr_bytes)
{
	dll_node_t *current = (*allocated)->head;
	unsigned int end = l;
	if (l >= 1) {
		for (unsigned int i = 0; i < l; i++)
			current = current->next;
	}

	while (current->next && ((info_t *)current->next->data)->start_addr ==
		   ((info_t *)current->data)->start_addr + ((info_t *)current->data)->
		   nr_bytes && nr_bytes > *bytes) {
		end++;
		*bytes += ((info_t *)current->data)->nr_bytes;
		current = current->next;
	}

	if (nr_bytes > *bytes && current->prev &&
		((info_t *)current->data)->start_addr == ((info_t *)
		 current->prev->data)->start_addr + ((info_t *)current->data)
		 ->nr_bytes) {
		*bytes += ((info_t *)current->data)->nr_bytes;
		end++;
		current = current->next;
	}

	if (l == end)
		*bytes = ((info_t *)current->data)->nr_bytes;

	return end;
}

// Functia scrie sirul de caractere dat la adresa precizata
void write(doubly_linked_list_t **allocated, unsigned long address, int *seg,
		   char *data, unsigned int nr_bytes)
{
	// verifica daca la adresa data este alocat un numar suficient de bytes
	unsigned int l = find_address((*allocated), address), bytes = 0;
	if (l == (*allocated)->size) {
		printf("Segmentation fault (core dumped)\n");
		*seg = 1;
		return;
	}
	dll_node_t *current = (*allocated)->head;
	if (nr_bytes > strlen(data))
		nr_bytes = strlen(data);
	unsigned int end = find_end(&bytes, l, allocated, nr_bytes), i = l;
	if (nr_bytes > bytes) {
		printf("Segmentation fault (core dumped)\n");
		*seg = 1;
		return;
	}

	if (l >= 1)
		for (unsigned int j = 0; j < l; j++)
			current = current->next;
	// se copiaza fragmente din sirul de caractere in functie de dimensiunea
	// nodurilor
	info_t *aux = (info_t *)(current->data);
	if (l == end) {
		if (!aux->input) {
			aux->input = malloc(aux->nr_bytes + 1);
			DIE(!aux->input, "malloc failed");
			memcpy(aux->input, data, strlen(data) + 1);
			(aux->input)[strlen(data)] = '\0';
		} else {
			strncpy(aux->input, data, strlen(data));
		}
		return;
	}
	while (i < (end - 1)) {
		if (!aux->input) {
			aux->input = malloc(aux->nr_bytes + 1);
			DIE(!aux->input, "malloc failed");
			char *p = malloc(MAX_SIZE * sizeof(char));
			DIE(!p, "malloc failed");
			strncpy(p, data, aux->nr_bytes);
			memcpy(aux->input, p, aux->nr_bytes + 1);
			(aux->input)[aux->nr_bytes] = '\0';
			strcpy(p, data + aux->nr_bytes);
			strcpy(data, p);
			free(p);
			current = current->next;
			aux = (info_t *)(current->data);
			bytes -= aux->nr_bytes;
		} else {
			// daca la acea adresa se afla ceva scris, se suprascrie informatia
			strncpy(aux->input, data, strlen(data));
			char *p = malloc(MAX_SIZE * sizeof(char));
			DIE(!p, "malloc failed");
			strcpy(p, data + bytes);
			strcpy(data, p);
			free(p);
			current = current->next;
			aux = (info_t *)(current->data);
		}
		i++;
	}

	if (!aux->input) {
		aux->input = malloc(nr_bytes + 1);
		DIE(!aux->input, "malloc failed");
		memcpy(aux->input, data, nr_bytes + 1);
		(aux->input)[nr_bytes] = '\0';
	} else {
		if (nr_bytes > strlen(aux->input)) {
			free(aux->input);
			aux->input = malloc((nr_bytes + 1) * sizeof(char));
			DIE(!aux->input, "malloc failed");
			(aux->input)[nr_bytes] = '\0';
		}
		strncpy(aux->input, data, nr_bytes);
	}
}

// Functia citeste si afiseaza sirul de caractere scris la adresa precizata
void read(doubly_linked_list_t **allocated, unsigned long address, int *seg,
		  unsigned int nr_bytes)
{
	// verifica daca la adresa data este alocat un numar suficient de bytes
	unsigned int l = find_address((*allocated), address), bytes = 0;
	if (l == (*allocated)->size) {
		printf("Segmentation fault (core dumped)\n");
		*seg = 1;
		return;
	}

	dll_node_t *current = (*allocated)->head;
	unsigned int end = find_end(&bytes, l, allocated, nr_bytes);

	if (nr_bytes > bytes) {
		printf("Segmentation fault (core dumped)\n");
		*seg = 1;
		return;
	}

	if (l >= 1) {
		for (unsigned int i = 0; i < l; i++)
			current = current->next;
	}

	// se afiseaza pe rand informatiile din noduri
	unsigned int i = l;
	if (end > l) {
		while (i < (end - 1)) {
			printf("%s", ((info_t *)current->data)->input);
			current = current->next;
			nr_bytes -= ((info_t *)current->data)->nr_bytes;
			i++;
		}
	}

	char *p = malloc(MAX_SIZE_INP * sizeof(char));
	DIE(!p, "malloc failed");
	strncpy(p, ((info_t *)current->data)->input, nr_bytes + 1);
	p[nr_bytes] = '\0';
	printf("%s", p);
	printf("\n");
	free(p);
}

// Functia calculeaza memoria libera
unsigned int
free_mem(heap_t *heap)
{
	unsigned int mem = 0;
	for (unsigned int i = 0; i < heap->nr_lists; i++) {
		doubly_linked_list_t *list = heap->segr_lists[i];
		mem += list->nr_bytes_per_node * list->size;
	}

	return mem;
}

// Functia calculeaza numarul de blocuri libere
unsigned int
free_blocks(heap_t *heap)
{
	unsigned int nr = 0;
	for (unsigned int i = 0; i < heap->nr_lists; i++)
		nr += heap->segr_lists[i]->size;

	return nr;
}

// Functia afișează zonele libere și zonele alocate
void dump_memory(heap_t *heap, doubly_linked_list_t *allocated, index_t *index)
{
	printf("+++++DUMP+++++\n");
	printf("Total memory: %d bytes\n", heap->memory);
	printf("Total allocated memory: %d bytes\n", index->mem_alloc);
	printf("Total free memory: %d bytes\n", free_mem(heap));
	printf("Free blocks: %d\n", free_blocks(heap));
	printf("Number of allocated blocks: %d\n", allocated->size);
	printf("Number of malloc calls: %d\n", index->index_malloc);
	printf("Number of fragmentations: %d\n", index->nr_fragmentations);
	printf("Number of free calls: %d\n", index->index_free);

	unsigned int i;
	for (i = 0; i < heap->nr_lists; i++) {
		if (heap->segr_lists[i]->size > 0) {
			printf("Blocks with %d bytes - ",
				   heap->segr_lists[i]->nr_bytes_per_node);
			printf("%d free block(s) :", heap->segr_lists[i]->size);
			dll_print_addr_list(heap->segr_lists[i]);
		}
	}

	printf("Allocated blocks :");
	dll_print_alloc_list(allocated);
	printf("\n-----DUMP-----\n");
}

// Functia elibereaza memoria alocata
void destroy_heap(heap_t **heap, doubly_linked_list_t **allocated)
{
	for (unsigned int i = 0; i < (*heap)->nr_lists; i++)
		dll_free(&((*heap)->segr_lists[i]));
	free((*heap)->segr_lists);
	free(*heap);
	dll_free(&(*allocated));
}
