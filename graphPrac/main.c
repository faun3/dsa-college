// https://csacademy.com/app/graph_editor/ -> use this for generating a data.txt file faster
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person {
    int id;
    char* name;
    int age;
} Person;

Person makePerson(int id, char* name, int age) {
    Person p = { id, _strdup(name), age };
    return p;
}

void printPerson(Person p) {
    printf("Person: (id: %d, name: %s, age: %d)\n", p.id, p.name, p.age);
}

typedef struct PrimaryNode {
    Person data;
    struct PrimaryNode* next;
    struct SecondaryNode* adjacents;
} PrimaryNode;

void pushPrimary(PrimaryNode** head, Person p) {
    PrimaryNode* newNode = (PrimaryNode*)malloc(sizeof(PrimaryNode));
    if (newNode == NULL) {
        exit(1);
    }

    newNode->adjacents = NULL;
    newNode->next = NULL;
    newNode->data = p;

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

PrimaryNode* findGraphNodeById(const PrimaryNode* graph, int id) {
    if (graph == NULL) {
        return NULL;
    }

    PrimaryNode* it = graph;
    while (it != NULL) {
        if (it->data.id == id) {
            return it;
        }
        it = it->next;
    }
}

typedef struct SecondaryNode {
    PrimaryNode* info;
    struct SecondaryNode* next;
} SecondaryNode;

void pushAdjacent(SecondaryNode** head, PrimaryNode* info) {
    SecondaryNode* newNode = (SecondaryNode*)malloc(sizeof(SecondaryNode));
    if (newNode == NULL) {
        exit(1);
    }
    newNode->next = NULL;
    newNode->info = info;

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

void parseFile(PrimaryNode** graph, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return;
    }

    int id, age;
    char name[256];
    while (fscanf(file, "%d %s %d", &id, name, &age) == 3) {
        Person p = makePerson(id, name, age);
        if (p.id != -1) {
            pushPrimary(graph, p);
        } else {
            break;
        }
    }

    int from, to;
    while (fscanf(file, "%d %d", &from, &to) == 2) {
        PrimaryNode* leftNode = findGraphNodeById(*graph, from);
        PrimaryNode* rightNode = findGraphNodeById(*graph, to);

        if (leftNode != NULL && rightNode != NULL) {
            pushAdjacent(&(leftNode->adjacents), rightNode);
            pushAdjacent(&(rightNode->adjacents), leftNode);
        }
    }

    fclose(file);
}

void printGraph(const PrimaryNode* graph) {
    PrimaryNode* it = graph;
    while (it != NULL) {
        printPerson(it->data);
        printf("Adjacents:\n");
        SecondaryNode* sit = it->adjacents;
        while (sit != NULL) {
            printf("%d ", sit->info->data.id);
            sit = sit->next;
        }
        printf("\n\n");
        it = it->next;
    }
}

void freeGraph(PrimaryNode** graph) {
    PrimaryNode* it;
    while (*graph) {
        it = *graph;
        *graph = (*graph)->next;
        free(it->data.name);
        SecondaryNode* sit;
        while (it->adjacents) {
            sit = it->adjacents;
            it->adjacents = it->adjacents->next;
            free(sit);
        }
        free(it);
    }
    *graph = NULL;
}

typedef struct ListNode {
    PrimaryNode* data;
    struct ListNode* next;
    struct ListNode* prev;
} ListNode;

typedef struct Deque {
    ListNode* head;
    ListNode* tail;
} Deque;

void pushLeft(Deque* deq, PrimaryNode* p) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (newNode == NULL) {
        exit(1);
    }
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->data = p;

    if (deq->head == NULL && deq->tail == NULL) {
        deq->head = newNode;
        deq->tail = newNode;
    } else {
        newNode->next = deq->head;
        deq->head->prev = newNode;
        deq->head = newNode;
    }
}

PrimaryNode* popLeft(Deque* deq) {
    if (deq->head == NULL) {
        return NULL;
    }

    ListNode* tmp = deq->head;
    PrimaryNode* val = tmp->data;

    deq->head = tmp->next;
    if (deq->head == NULL) {
        deq->tail = NULL;
    } else {
        deq->head->prev = NULL;
    }

    free(tmp);
    return val;
}

PrimaryNode* popRight(Deque* deq) {
    if (deq->tail == NULL) {
        return NULL;
    }

    ListNode* tmp = deq->tail;
    PrimaryNode* val = tmp->data;
    if (deq->head == deq->tail) {
        deq->head = NULL;
        deq->tail = NULL;
    } else {
        deq->tail = deq->tail->prev;
    }
    free(tmp);

    return val;
}

void freeDeque(Deque* deq) {
    ListNode* it;
    while (deq->head) {
        it = deq->head;
        deq->head = deq->head->next;

        // we don't free the name because it's shallow copied
        free(it);
    }
}

void graphDfs(const PrimaryNode* graph) {
    // hacky
    bool visited[8] = { 0 };

    Deque d = { NULL, NULL };

    pushLeft(&d, graph);
    while (d.head) {
        PrimaryNode* currentNode = popLeft(&d);
        if (visited[currentNode->data.id - 1] == false) {
            printf("%d ", currentNode->data.id);
            visited[currentNode->data.id - 1] = true;
        }

        SecondaryNode* it = currentNode->adjacents;
        while (it != NULL) {
            if (visited[it->info->data.id - 1] == false) {
                pushLeft(&d, it->info);
            }
            it = it->next;
        }
    }

    freeDeque(&d);
}

void graphBfs(const PrimaryNode* graph) {
    bool visited[8] = { false };
    Deque d = { NULL, NULL };

    pushLeft(&d, graph);
    while (d.head) {
        PrimaryNode* currentNode = popRight(&d);
        if (visited[currentNode->data.id - 1] == false) {
            printf("%d ", currentNode->data.id);
            visited[currentNode->data.id - 1] = true;
        }

        SecondaryNode* it = currentNode->adjacents;
        while (it != NULL) {
            if (visited[it->info->data.id - 1] == false) {
                pushLeft(&d, it->info);
            }
            it = it->next;
        }
    }

    freeDeque(&d);
}

int main() {
    PrimaryNode* graph = NULL;
    parseFile(&graph, "data.txt");
    printf("\nGraph:\n");
    printGraph(graph);

    printf("\nDFS:\n");
    graphDfs(graph);
    printf("\nBFS:\n");
    graphBfs(graph);
    freeGraph(&graph);
}