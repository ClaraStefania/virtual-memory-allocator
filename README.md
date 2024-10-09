Diaconescu Stefania Clara 313CA 2023-2024

## Segregated Free Lists - Tema 1

### Descriere:

   Problema are ca scop realizarea unui alocator de memorie virtual, simuland
functiile de malloc() si free() cu ajutorul unui vector de liste dublu
inlantuite.

   Functiile de baza pentru prelucrarea listelor utilizate in program sunt:
* dll_create() - aloca o lista si initializeaza campurile acesteia
* dll_add_nth_node() - adauga un nod in lista pe pozitia n. Se aloca memorie
si sunt create legaturile in functie de locul in care este adaugat nodul.
* dll_remove_nth_node() - intoarce un pointer la nodul de pe pozitia n,
legaturile fiind desfacute
* dll_free() - dezaloca memoria ocupata de lista

   In 'main()' este citita prima comanda, urmand sa intre in while() cat timp
aceasta este diferita de 'DESTROY_HEAP'.

#### INIT_HEAP

   Este alocata memorie pentru vectorul de liste. Sunt create listele si sunt
stabilite adresele nodurilor, primul nod avand adresa data in comanda. De
asemenea, fiecare nod va avea un index, util in cazul free-ului specific
tipului 1. Sunt apelate functiile 'dll_create()' si 'dll_add_nth_node()'.

#### MALLOC

   Se verifica daca exista blocuri cu o dimensiune mai mare sau egala cu cea
ceruta, cu ajutorul functiei 'verify_heap()'. Aceasta returneaza 0 daca nu
exista suficienta memorie si 1 in caz contrar. Indicele listei ce contine
noduri de dimensiune necesara este salvat in variabla i, iar daca numarul de
bytes este egal, variabila equal preia valoarea lui i. Altfel, ramane -1;
   Daca nu este gasita o lista ce contine noduri cu dimensiunea egala cu cea
ceruta, blocul este fragmentat. Sunt modificate numarul de bytes si adresa,
adaugandu-se in lista corespunzatoare fragmentul ramas (daca lista nu exista
este inserata in vector cu ajutorul functiei 'insert_lists()'). Functia
'pos_in_list()' returneaza pozitia la care trebuie adaugat, astfel incat
adresele din lista sa fie in ordine crescatoare.
   La final, nodul extras din heap este adaugat in lista cu cele alocate.

#### FREE

   Se verifica daca exista un bloc ce incepe de la adresa data, prin apelarea
functiei 'find_address()' ce returneaza pozitia blocului.
   Daca tipul este 0, se cauta lista corespunzatoare dimensiunii, iar daca
nu exista, este inserata.
   Daca tipul este 1, se doreste reconstituirea blocurilor initiale. Se
verifica daca exista noduri ce pot fi lipite de cel caruia i s-a dat free(),
fiind apelata functia 'find_index()'. Aceasta cauta noduri care au acelasi
index si adresele continue. Daca exista, blocurile sunt lipite. Se modifica
numarul de bytes si adresa daca este cazul, indexul i fiind pozitionat din nou
la inceputul vectorului.
   La final, se realizeaza acelasi procedeu ca in cazul tipului 0.

### WRITE SI READ

   Se verifica daca la adresa data este alocata memorie si daca aceasta este
suficienta. Functia 'find_end()' cauta la ce bloc trebuie oprita scrierea,
vazand astfel daca alocarile au fost succesive si daca toti octetii din
interval au fost alocati. Daca dimensiunea sirului ce trebuie scris este mai
mica decat numarul de bytes din comanda, acesta preia lungimea sirului. La
final, este copiata informatia. Daca la acea adresa se afla deja ceva scris,
caracterele se suprascriu.
   In cazul read-ului, procedeul este acelasi, doar ca in loc sa fie scrisa
informatia, este afisata.

#### DUMP_MEMORY

   Functia afiseaza zonele libere si zonele alocate. Sunt apelate 'free_mem()'
si 'free_blocks()', care calculeaza cata memorie, respectiv cate blocuri
libere sunt.


#### DESTROY_HEAP

   Este dezalocata toata memoria ocupata de heap si de lista de blocuri
alocate.

   In cazul fiecarei comenzi sunt tratate cazurile de eroare, precum 'Invalid
free', 'Segmentation fault (core dumped)' sau 'Out of memory'.
