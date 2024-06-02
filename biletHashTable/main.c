// https://github.com/snaake20/ase_info_an_2_sem_2/blob/main/SDD/exercitii_examen/comanda%20-%20HTL%20LS%20-%201/ComandaEcom.jpeg
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct Comanda
{
    unsigned int idComanda;
    char* dataLansare;
    char* dataLivrare;
    char* numeClient;
    float suma;
} Comanda;

Comanda makeComanda(unsigned int idComanda, char* dataLansare, char* dataLivrare, char* numeClient, float suma)
{
    Comanda comanda = {
        idComanda,
        _strdup(dataLansare),
        _strdup(dataLivrare),
        _strdup(numeClient),
        suma
    };
    return comanda;
}

void printComanda(Comanda c)
{
    printf("Comanda: (id: %u, lansare: %s, livrare: %s, client: %s, suma: %.2f)\n", c.idComanda, c.dataLansare, c.dataLivrare, c.numeClient, c.suma);
}

typedef struct Node
{
    Comanda data;
    struct Node* next;
} Node;

typedef struct HashTable
{
    int sz;
    Node** data;
} HashTable;

unsigned int hash(char* name, int tableSize)
{
    unsigned int val = 17;

    val = val + strlen(name) * 31;

    return val % tableSize;
}

HashTable makeHashTable(int sz)
{
    HashTable ht = {
        sz,
        (Node**)calloc(sz, sizeof(Node*))
    };
    return ht;
}

void printHashTable(const HashTable* h)
{
    for (int i = 0; i < h->sz; i++)
    {
        printf("[%d]:\n", i);
        Node* it = h->data[i];
        while (it != NULL)
        {
            printComanda(it->data);
            it = it->next;
        }
    }
}

void addToHashTable(HashTable* ht, Comanda c)
{
    int idx = hash(c.numeClient, ht->sz);

    Node* newNode = (Node*)calloc(1, sizeof(Node)); if (!newNode) exit(1);

    newNode->next = NULL;
    newNode->data = c;
    if (ht->data[idx] == NULL)
    {
        ht->data[idx] = newNode;
    }
    else
    {
        Node* it = ht->data[idx];
        while (it != NULL && it->next != NULL)
        {
            it = it->next;
        }
        it->next = newNode;
    }
}

void parseFile(HashTable* ht, char* fileName)
{
    FILE* f = fopen(fileName, "r");
    if (f == NULL) return;

    unsigned int clientId = 0;
    char placed[256], arrival[256], name[256];
    float value;

    while (fscanf(f, "%u %s %s %s %f", &clientId, placed, arrival, name, &value) == 5)
    {
        Comanda c = makeComanda(clientId, placed, arrival, name, value);
        addToHashTable(ht, c);
    }

    fclose(f);
}

void freeList(Node** head)
{
    if (*head == NULL) return;

    while (*head != NULL)
    {
        Node* it = *head;
        (*head) = (*head)->next;
        free(it->data.dataLansare);
        free(it->data.dataLivrare);
        free(it->data.numeClient);
        free(it);
    }
}

void freeHashTable(HashTable* ht)
{
    for (int i = 0; i < ht->sz; i++)
    {
        freeList(&ht->data[i]);
    }
}

unsigned int getNrComenzi(const HashTable* ht, const char* name)
{
    unsigned int idx = hash(name, ht->sz);

    unsigned int count = 0;
    Node* list = ht->data[idx];
    while (list != NULL)
    {
        if (strcmp(list->data.numeClient, name) == 0)
        {
            count++;
        }
        list = list->next;
    }
    return count;
}

int updateDeliveryDate(HashTable* ht, unsigned int orderId, const char* newDate)
{
    for (int i = 0; i < ht->sz; i++)
    {
        Node* it = ht->data[i];
        while (it != NULL)
        {
            if (it->data.idComanda == orderId)
            {
                free(it->data.dataLivrare);
                it->data.dataLivrare = _strdup(newDate);
                return 1;
            }
            it = it->next;
        }
    }
    return -1;
}

int compareDateString(char* dateLhs, char* dateRhs)
{
    char* l = _strdup(dateLhs);
    char* r = _strdup(dateRhs);

    int dayLhs, monthLhs, yearLhs, dayRhs, monthRhs, yearRhs;
    char* step = strtok(l, "-");
    dayLhs = atoi(step);
    step = strtok(NULL, "-");
    monthLhs = atoi(step);
    step = strtok(NULL, "-");
    yearLhs = atoi(step);

    step = strtok(r, "-");
    dayRhs = atoi(step);
    step = strtok(NULL, "-");
    monthRhs = atoi(step);
    step = strtok(NULL, "-");
    yearRhs = atoi(step);

    free(l);
    free(r);

    if (yearLhs == yearRhs)
    {
        if (monthLhs == monthRhs)
        {
            if (dayLhs == dayRhs) return 0;
            else
            {
                if (dayLhs < dayRhs) return -1;
                else return 1;
            }
        }
        else
        {
            if (monthLhs < monthRhs) return -1;
            else return 1;
        }
    }
    else
    {
        if (yearLhs < yearRhs) return -1;
        else return 1;
    }

    // lol
    return 666;
}

Node* tableToList(const HashTable* ht, float sum)
{
    Node* head = NULL;

    for (int i = 0; i < ht->sz; i++)
    {
        Node* it = ht->data[i];
        while (it != NULL)
        {
            if (it->data.suma >= sum)
            {
                Node* newNode = (Node*)calloc(1, sizeof(Node)); if (newNode == NULL) exit(1);
                newNode->next = NULL;
                newNode->data = makeComanda(it->data.idComanda, it->data.dataLansare, it->data.dataLivrare, it->data.numeClient, it->data.suma);
                if (head == NULL)
                {
                    head = newNode;
                }
                else
                {
                    newNode->next = head;
                    head = newNode;
                }
            }
            it = it->next;
        }
    }

    return head;
}

void deleteWhereDeliveryDateIsBefore(HashTable* ht, char* filterDate)
{
    for (int i = 0; i < ht->sz; i++)
    {
        if (ht->data[i] != NULL)
        {
            if (ht->data[i]->next == NULL)
            {
                if (compareDateString(ht->data[i]->data.dataLivrare, filterDate) == -1)
                {
                    Node* it = ht->data[i];
                    free(it->data.dataLansare);
                    free(it->data.dataLivrare);
                    free(it->data.numeClient);
                    free(it);
                    ht->data[i] = NULL;
                }
            }
            else
            {
                Node* it = ht->data[i];
                while (it && it->next)
                {
                    if (compareDateString(it->data.dataLivrare, filterDate) == -1)
                    {
                        Node* deleteMe = it->next;
                        Node* right = it->next->next;
                        it->next = right;
                        free(deleteMe->data.dataLansare);
                        free(deleteMe->data.dataLivrare);
                        free(deleteMe->data.numeClient);
                        free(deleteMe);
                    }
                    it = it->next;
                }
            }
        }
    }
}

int main()
{
    Comanda c1 = makeComanda(100, "24-10-2024", "31-10-2024", "George", 666.66);
    Comanda c2 = makeComanda(110, "24-09-2023", "21-10-2023", "Anne", 666.66);
    printComanda(c1);

    HashTable ht = makeHashTable(10);
    printHashTable(&ht, 10);
    parseFile(&ht, "date.txt");
    printf("\n\n\n");
    printHashTable(&ht, 10);

    printf("\n\n%u\n", getNrComenzi(&ht, "Nothere"));

    updateDeliveryDate(&ht, 100, "01-01-2023");

    printHashTable(&ht, 10);

    printf("\n\n%d\n\n", compareDateString(c1.dataLivrare, c2.dataLivrare));
    printf("\n\n%d\n\n", compareDateString(c1.dataLivrare, c2.dataLivrare));

    Node* list = tableToList(&ht, 666);
    Node* it = list;
    printf("\n\n\nList: \n");
    while (it != NULL)
    {
        printComanda(it->data);
        it = it->next;
    }

    printf("\n\n\n");
    printHashTable(&ht);

    deleteWhereDeliveryDateIsBefore(&ht, "01-01-2024");
    printf("\n\n\n");
    printHashTable(&ht);

    freeHashTable(&ht);
}