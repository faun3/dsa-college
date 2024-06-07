// https://github.com/snaake20/ase_info_an_2_sem_2/blob/main/SDD/exercitii_examen/host%20-%20LA%202xV%20-%201/Host.pdf

// ------README--------
// the value "666" in the data.txt file separates the end of the graph nodes
// and the start of the edges and their weights
// 
// why did I do this?
// lazyness
// 
// fscanf will read the 5 values that make up a Host but it doesn't know how many it just read
// until it reads then (duh)
// when fscanf runs out of Host lines, it will read the first value (which should go in "to")
// and then exit the loop
// 
// adding a dummy integer value will still break the loop and it will allow me
// to keep using fscanf instead of reading into a buffer and parsing tokens manually
// 
// because this is an exam and the only thing that matters is the file being parsed
// this is actually okay
// but the code that reads the file is truly horrible in any other case
// -------README END-------
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int id;
    char* name;
    char* ip;
    int port;
    int uptime;
} Host;

Host makeHost(int id, char* name, char* ip, int port, int uptime) {
    Host h = {
        id,
        _strdup(name),
        _strdup(ip),
        port, uptime
    };
    return h;
}

Host cloneHost(Host h) {
    return makeHost(h.id, h.name, h.ip, h.port, h.uptime);
}

void printHost(Host h) {
    printf("Host: (id: %d, name: %s, ip: %s, port: %d, uptime: %d)\n", h.id, h.name, h.ip, h.port, h.uptime);
}

// keeps a list of graph nodes in the order they were appended
// also hold the struct data about the node
typedef struct PrimaryNode {
    Host data;
    struct PrimaryNode* next;
    struct SecondaryNode* adjacents;
} PrimaryNode;


// keeps a reference to the next secondary node (builds a list of neighbors for the current node)
typedef struct SecondaryNode {
    struct PrimaryNode* info; // graph node we are referencing
    struct SecondaryNode* next; // next adjacent node
    int weight;
} SecondaryNode;

void appendToPrimaryList(PrimaryNode** head, Host h) {
    PrimaryNode* newNode = (PrimaryNode*)malloc(sizeof(PrimaryNode));
    if (newNode == NULL) {
        exit(1);
    }

    newNode->next = NULL;
    newNode->adjacents = NULL;

    // shallow copy
    newNode->data = h;

    if (*head == NULL) {
        *head = newNode;
    } else {
        PrimaryNode* it = *head;
        while (it != NULL && it->next != NULL) {
            it = it->next;
        }
        it->next = newNode;
    }
}

void appendToSecondaryList(SecondaryNode** head, PrimaryNode* info, int weight) {
    SecondaryNode* newNode = (SecondaryNode*)malloc(sizeof(SecondaryNode));
    if (newNode == NULL) {
        exit(1);
    }

    newNode->next = NULL;
    newNode->info = info;
    newNode->weight = weight;

    if (*head == NULL) {
        *head = newNode;
    } else {
        SecondaryNode* it = *head;
        while (it != NULL && it->next != NULL) {
            it = it->next;
        }
        it->next = newNode;
    }
}

void printPrimaryList(const PrimaryNode* head) {
    PrimaryNode* it = head;

    while (it != NULL) {
        printHost(it->data);
        printf("Adjacent nodes: \n");
        SecondaryNode* sit = it->adjacents;
        while (sit != NULL) {
            printf("%d - weight: %d\n", sit->info->data.id, sit->weight);
            sit = sit->next;
        }
        printf("\n");
        it = it->next;
    }
}

PrimaryNode* findGraphNodeById(const PrimaryNode* head, int filterId) {
    if (head == NULL) {
        return NULL;
    }

    PrimaryNode* it = head;
    while (it != NULL) {
        if (it->data.id == filterId) {
            return it;
        }
        it = it->next;
    }
}

void addEdge(PrimaryNode* graph, int leftId, int rightId, int weight) {
    // find both nodes
    PrimaryNode* leftNode = findGraphNodeById(graph, leftId);
    PrimaryNode* rightNode = findGraphNodeById(graph, rightId);

    // only if both nodes were found
    if (leftNode != NULL && rightId != NULL) {
        // only one edge is appended because the graph is directend AND weighted
        appendToSecondaryList(&(leftNode->adjacents), rightNode, weight);
    }
}

void parseFile(PrimaryNode** head, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        return;
    }

    int id, port, uptime;
    char name[256], ip[256];

    int reads;
    while (fscanf(f, "%d \"%[^\"]\" \"%[^\"]\" %d %d", &id, name, ip, &port, &uptime) == 5) {
        Host h = makeHost(id, name, ip, port, uptime);
        appendToPrimaryList(head, h);
    }

    int from, to, weight;
    while (fscanf(f, "%d %d %d", &from, &to, &weight) == 3) {
        addEdge(*head, from, to, weight);
    }

    fclose(f);
}

typedef struct ArrEl {
    PrimaryNode* source;
    PrimaryNode* dest;
} ArrEl;

ArrEl* buildArrWithEdgesWithWeightOver(const PrimaryNode* graph, int weigthFilter, int* sz) {
    ArrEl* arr = NULL;
    *sz = 0;

    PrimaryNode* it = graph;
    while (it != NULL) {
        SecondaryNode* sit = it->adjacents;
        while (sit != NULL) {
            if (sit->weight > weigthFilter) {
                (*sz)++;
                ArrEl* tmp = realloc(arr, sizeof(ArrEl) * (*sz));
                if (tmp == NULL) {
                    exit(1);
                }

                ArrEl newVal = {
                    it,
                    sit->info
                };
                tmp[(*sz) - 1] = newVal;

                arr = tmp;
            }
            sit = sit->next;
        }
        it = it->next;
    }

    return arr;
}

Host* getHostsWithUptimeLessOrEqualTo(const PrimaryNode* graph, int uptimeFilter, int* sz) {
    Host* arr = NULL;
    *sz = 0;

    PrimaryNode* it = graph;
    while (it != NULL) {
        if (it->data.uptime <= uptimeFilter) {
            (*sz)++;
            Host* tmp = realloc(arr, sizeof(Host) * (*sz));
            if (tmp == NULL) {
                exit(1);
            }

            tmp[(*sz) - 1] = cloneHost(it->data);

            arr = tmp;
        }
        it = it->next;
    }

    return arr;
}

freeGraph(PrimaryNode** graph) {
    if (*graph == NULL) return;

    PrimaryNode* it = NULL;
    while (*graph != NULL) {
        it = (*graph);
        SecondaryNode* sit;
        while (it->adjacents != NULL) {
            sit = it->adjacents;
            it->adjacents = it->adjacents->next;
            free(sit);
        }
        (*graph) = (*graph)->next;
        free(it->data.ip);
        free(it->data.name);
        free(it);
    }
}

// 4.
// I actually don't understand what they want me to filter the array by

int main() {
    PrimaryNode* graph = NULL;
    parseFile(&graph, "data.txt");
    printPrimaryList(graph);

    ArrEl* arr = NULL;
    int sz = 0;

    arr = buildArrWithEdgesWithWeightOver(graph, 3, &sz);

    printf("\nArray:\n");
    for (int i = 0; i < sz; i++) {
        printf("%d - %d\n", arr[i].source->data.id, arr[i].dest->data.id);
    }

    free(arr);

    Host* uptimesArr = NULL;
    int uptimesLen = 0;
    int uptimeFilter = 201;

    uptimesArr = getHostsWithUptimeLessOrEqualTo(graph, uptimeFilter, &uptimesLen);
    printf("\nArray of hosts with uptime less than or equal to %d:\n", uptimeFilter);
    for (int i = 0; i < uptimesLen; i++) {
        printHost(uptimesArr[i]);
    }
    free(uptimesArr);

    freeGraph(&graph);
}