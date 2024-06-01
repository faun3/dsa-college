#define _CRT_SECURE_NO_WARNINGS
// https://github.com/snaake20/ase_info_an_2_sem_2/tree/main/SDD/exercitii_examen/comanda%20-%20BST%20Heap%202xV%20-%201
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Comanda 
{
	int id_comanda;
	int cod_client;
	char* nume_client;
	int timp_livrare;
	int nr_produse;
	double valoare;
	double reducere_aplicata;
} Comanda;

typedef struct Node
{
	Comanda data;
	struct Node* left;
	struct Node* right;
} Node;

Comanda make_comanda(int id_comanda, int cod_client, char* nume_client, int timp_livrare, int nr_produse, double valoare, double reducere_aplicata)
{
	Comanda comanda;

	comanda.id_comanda = id_comanda;
	comanda.cod_client = cod_client;
	
	if (nume_client != NULL) 
	{
		comanda.nume_client = _strdup(nume_client);
	}
	else comanda.nume_client = NULL;

	comanda.timp_livrare = timp_livrare;
	comanda.nr_produse = nr_produse;
	comanda.valoare = valoare;
	comanda.reducere_aplicata = reducere_aplicata;

	return comanda;
}

Comanda make_clone(Comanda other) 
{
	return make_comanda(other.id_comanda, other.cod_client, other.nume_client, other.timp_livrare, other.nr_produse, other.valoare, other.reducere_aplicata);
}

void print_comanda(Comanda c)
{
	printf("Comanda: (id_comanda: %d, cod_client: %d, nume_client: %s, timp_livrare: %d, nr_produse: %d, valoare: %.2lf, reducere_aplicata: %.2lf)\n", c.id_comanda, c.cod_client, c.nume_client, c.timp_livrare, c.nr_produse, c.valoare, c.reducere_aplicata);
}

void free_comanda(Comanda c)
{
	free(c.nume_client);
}

// read a string with spaces
// % -> start matching something
// [] -> scanset AKA what stuff to read
// ^ -> not
// ^\" -> not a " character
// 
// this whole scanset NEEDS to be wrapped in "" that we escape with \" \"
// why? because scanf reads literally what we provide it, so it will consume the first quote "M A" -> M A"
//		and then read anything until it touches another quote -> we read 'M A' in a buffer somewhere
//		then consume the second quote " -> nothing, quotes were consumed

void tree_insert(Node** root, Comanda c)
{
	Node* new_node = (Node*)malloc(sizeof(Node));
	if (new_node == NULL) exit(1);

	new_node->left = NULL;
	new_node->right = NULL;

	if (*root == NULL)
	{
		new_node->data = make_clone(c);
		*root = new_node;
	}
	else
	{
		if (c.id_comanda < (*root)->data.id_comanda)
		{
			tree_insert(&(*root)->left, c);
		}
		else
		{
			tree_insert(&(*root)->right, c);
		}
	}
}

// preorder
void print_tree(Node* root)
{
	if (root == NULL) return;
	print_comanda(root->data);
	print_tree(root->left);
	print_tree(root->right);
}

void preorder_print(Node* root)
{
	print_tree(root);
}

void inorder_print(Node* root)
{
	if (root == NULL) return;

	inorder_print(root->left);
	print_comanda(root->data);
	inorder_print(root->right);
}

void postorder_print(Node* root)
{
	if (root == NULL) return;

	postorder_print(root->left);
	postorder_print(root->right);
	print_comanda(root->data);
}

void free_tree(Node** root)
{
	if (*root == NULL) return;

	free_tree(&(*root)->left);
	free_tree(&(*root)->right);
	free_comanda((*root)->data);
	free(*root);
	*root = NULL;
}

void load_data(const char* file_name, Node** root)
{
	FILE* file = fopen(file_name, "r");

	int id_comanda, cod_client, timp_livrare, nr_produse;
	double valoare, reducere_aplicata;
	char name_buffer[256];

	while (fscanf(file, "%d %d \"%[^\"]\" %d %d %lf %lf", &id_comanda, &cod_client, name_buffer, &timp_livrare, &nr_produse, &valoare, &reducere_aplicata) == 7)
	{
		Comanda c = make_comanda(id_comanda, cod_client, name_buffer, timp_livrare, nr_produse, valoare, reducere_aplicata);
		tree_insert(root, c);
	}

	fclose(file);
}

void free_comanda_arr(Comanda* arr, int sz)
{
	for (int i = 0; i < sz; i++)
	{
		free_comanda(arr[i]);
	}
}

void find_orders_with_longer_delivery_than(Node* root, int time_filter, Comanda** found_arr, int* sz)
{
	if (root == NULL) return;

	if (root->data.timp_livrare > time_filter)
	{
		(*sz)++;
		Comanda* tmp = (Comanda*)realloc((*found_arr), sizeof(Comanda) * (*sz));
		if (tmp == NULL) exit(1); // failed to realloc

		tmp[(*sz) - 1] = make_clone(root->data);
		*found_arr = tmp;
	}
	find_orders_with_longer_delivery_than(root->left, time_filter, found_arr, sz);
	find_orders_with_longer_delivery_than(root->right, time_filter, found_arr, sz);
}

int tree_count_nodes(Node* tree)
{
	if (tree == NULL) return 0;
	else return 1 + tree_count_nodes(tree->left) + tree_count_nodes(tree->right);
}

void heapify(Comanda* heap, int sz, int idx)
{
	int smallest = idx;
	int left = (idx * 2) + 1;
	int right = (idx * 2) + 2;

	// find smallest and swap 
	if (left < sz && heap[left].timp_livrare < heap[smallest].timp_livrare)
	{
		smallest = left;
	}

	if (right < sz && heap[right].timp_livrare < heap[smallest].timp_livrare)
	{
		smallest = right;
	}

	if (smallest != idx)
	{
		Comanda aux = heap[smallest];
		heap[smallest] = heap[idx];
		heap[idx] = aux;

		// heapify on the subtree where 
		heapify(heap, sz, smallest);
	}
}

void mutate_into_min_heap(Comanda* heap, int sz)
{
	for (int i = (sz / 2) - 1; i >= 0; i--)
	{
		printf("i: %d\n", i);
		heapify(heap, sz, i);
	}
}

int tree_to_arr_recursive_inorder(const Node* root, Comanda* arr, int idx)
{
	if (root == NULL) return idx;

	idx = tree_to_arr_recursive_inorder(root->left, arr, idx);
	arr[idx++] = make_clone(root->data);
	idx = tree_to_arr_recursive_inorder(root->right, arr, idx);

	return idx;
}

// build a heap out of the nodes using the delivery_time
Comanda* new_heap(Node* root, int* sz)
{
	*sz = tree_count_nodes(root);
	Comanda* heap = (Comanda*)malloc(sizeof(Comanda) * (*sz));
	(void)tree_to_arr_recursive_inorder(root, heap, 0);

	return heap;
}

typedef struct ClientPair 
{
	int cod_client;
	double suma_totala;
} ClientPair;

int find_by_client_id(int client_id, const ClientPair* arr, int sz)
{
	if (arr == NULL) return -1;
	int idx = -1;

	for (int i = 0; i < sz; i++)
	{
		if (arr[i].cod_client == client_id) return i;
	}

	return idx;
}

ClientPair* order_total_group_by_client(const Node* root, int* sz_clients)
{
	ClientPair* clients = NULL;
	(*sz_clients )= 0;

	// iterative inorder traversal
	Node* current = root;
	int sz = tree_count_nodes(root);
	// don't need to free this, we're shallow copying
	Node** stack = (Node**)calloc(sz, sizeof(Node*)); if (stack == NULL) exit(1);
	int top = -1;

	while (current != NULL || top != -1)
	{
		while (current != NULL)
		{
			stack[++top] = current;
			current = current->left;
		}

		current = stack[top--];
		if (find_by_client_id(current->data.cod_client, clients, *sz_clients) == -1)
		{
			ClientPair* tmp = (ClientPair*)realloc(clients, sizeof(ClientPair) * ++(*sz_clients));
			if (tmp == NULL) exit(1);

			clients = tmp;
			ClientPair pair;
			pair.cod_client = current->data.cod_client;
			pair.suma_totala = current->data.valoare;
			clients[*sz_clients - 1] = pair;
		}
		else
		{
			clients[find_by_client_id(current->data.cod_client, clients, *sz_clients)].suma_totala += current->data.valoare;
		}

		current = current->right;
	}

	return clients;
}

int main()
{
	Comanda c1 = make_comanda(1, 1, "Mihail", 10, 4, 666.666, 0.23);
	Comanda c2 = make_clone(c1);
	c2.nume_client = _strdup("George");
	print_comanda(c1);
	print_comanda(c2);

	printf("\n\n\n");

	Node* root = NULL;
	load_data("data.txt", &root);

	preorder_print(root);
	// inorder_print(root);
	// postorder_print(root);

	// Comanda* found_arr = NULL;
	// int sz = 0;

	// find_orders_with_longer_delivery_than(root, 26, &found_arr, &sz);

	// printf("\n\nFound: \n");
	// for (int i = 0; i < sz; i++)
	// {
	// 	print_comanda(found_arr[i]);
	// }

	// free_comanda_arr(found_arr, sz);
	// found_arr = NULL;

	// printf("%d", tree_count_nodes(root));


	// heap, heapfiy
	Comanda* heap = NULL;
	int sz = 0;

	heap = new_heap(root, &sz);
	mutate_into_min_heap(heap, sz);

	printf("\n\n\nHeap: \n");
	for (int i = 0; i < sz; i++)
	{
		print_comanda(heap[i]);
	}

	ClientPair* clients = NULL;
	int sz_clients = 0;

	clients = order_total_group_by_client(root, &sz_clients);

	for (int i = 0; i < sz_clients; i++)
	{
		printf("Id: %d, Total: %.2lf\n", clients[i].cod_client, clients[i].suma_totala);
	}

	free_tree(&root);
	free_comanda_arr(heap, sz);
}