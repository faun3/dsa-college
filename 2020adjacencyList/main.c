// https://github.com/snaake20/ase_info_an_2_sem_2/blob/main/SDD/exercitii_examen/host%20-%20LA%202xV%20-%201/Host.pdf
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

    while (fscanf(f, "%d \"%[^\"]\" \"%[^\"]\" %d %d\n", &id, name, ip, &port, &uptime) == 5) {
        Host h = makeHost(id, name, ip, port, uptime);
        appendToPrimaryList(head, h);
    }
    int from, to, weight;
    while (fscanf(f, "%d %d %d", &from, &to, &weight) == 3) {
        printf("%d %d %d\n", to, from, weight);
        addEdge(*head, from, to, weight);
    }

    fclose(f);
}

int main() {
    PrimaryNode* graph = NULL;
    parseFile(&graph, "data.txt");
    printPrimaryList(graph);
}