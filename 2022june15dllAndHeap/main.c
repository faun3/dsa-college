// https://github.com/snaake20/ase_info_an_2_sem_2/blob/main/SDD/exercitii_examen/vagon%20-%20LD%20Heap%20-%201/Vagon.jpg

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

typedef struct TrainCar {
    int id;
    char* transporter;
    int tickets;
    int capacity;
} TrainCar;

TrainCar makeTrainCar(int id, char* transporter, int tickets, int capacity) {
    TrainCar t = {
        id, _strdup(transporter), tickets, capacity
    };
    return t;
}

TrainCar cloneTrainCar(TrainCar t) {
    return makeTrainCar(t.id, t.transporter, t.tickets, t.capacity);
}

void printTrainCar(TrainCar t) {
    printf("TrainCar: (id: %d, transporter: %s, tickets: %d, capacity: %d)\n", 
        t.id, t.transporter, t.tickets, t.capacity);
}

typedef struct Node {
    struct Node* next;
    struct Node* prev;
    TrainCar data;
} Node;

typedef struct List {
    Node* head;
    Node* tail;
} List;

void printList(const List* list) {
    printf("Print in natural order:\n");
    Node* it = list->head;
    while (it) {
        printTrainCar(it->data);
        it = it->next;
    }
}

void printListReverse(const List* list) {
    printf("Print in reverse order:\n");
    Node* it = list->tail;
    while (it) {
        printTrainCar(it->data);
        it = it->prev;
    }
}

void freeList(List* list) {
    if (!list->head) {
        return;
    }

    Node* it;
    while (list->head) {
        it = list->head;
        list->head = list->head->next;
        free(it->data.transporter);
        free(it);
    }
    list->head = NULL;
    list->tail = NULL;
}

void pushLeft(List* list, TrainCar t) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        exit(1);
    }

    newNode->prev = NULL;
    newNode->data = t;

    if (!list->head && !list->tail) {
        newNode->next = NULL;
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->head->prev = newNode;
        newNode->next = list->head;
        list->head = newNode;
    }
}

void parseFile(List* list, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        return;
    }

    char transporter[256];
    int id, tickets, capacity;
    while (fscanf(f, "%d %s %d %d", &id, transporter, &tickets, &capacity) == 4) {
        pushLeft(list, makeTrainCar(id, transporter, tickets, capacity));
    }

    fclose(f);
}

int getMinTickets(const List* list) {
    int min = INT_MAX;

    Node* it = list->head;
    while (it) {
        if (it->data.tickets < min) {
            min = it->data.tickets;
        }
        it = it->next;
    }

    return min;
}

void deleteMinTickets(List* list) {
    int min = getMinTickets(list);

    if (list->head->data.tickets == min) {
        Node* deleteMe = list->head;

        if (list->head == list->tail) {
            list->head = NULL;
            list->tail = NULL;
        } else {
            list->head = list->head->next;
        }

        free(deleteMe->data.transporter);
        free(deleteMe);
    } else {
        Node* it = list->head;
        while (it && it->next) {
            if (it->next->data.tickets == min) {
                Node* deleteMe = it->next;
                Node* afterDelete = deleteMe->next;
                it->next = afterDelete;
                free(deleteMe->data.transporter);
                free(deleteMe);
                return;
            }
        }
    }
}

TrainCar* listToArray(const List* list, int* sz) {
    TrainCar* arr = NULL;
    *sz = 0;

    Node* it = list->head;
    while (it) {
        (*sz)++;
        TrainCar* temp = realloc(arr, sizeof(TrainCar) * (*sz));
        if (!temp) {
            exit(1);
        }
        temp[(*sz) - 1] = cloneTrainCar(it->data);
        arr = temp;
        it = it->next;
    }

    return arr;
}

void heapify(TrainCar* arr, int sz, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < sz && arr[left].capacity < arr[smallest].capacity) {
        smallest = left;
    }

    if (right < sz && arr[right].capacity < arr[smallest].capacity) {
        smallest = right;
    }

    if (smallest != idx) {
        TrainCar t = arr[smallest];
        arr[smallest] = arr[idx];
        arr[idx] = t;

        heapify(arr, sz, smallest);
    }
}

void buildHeap(TrainCar* arr, int sz) {
    for (int i = (sz / 2) - 1; i >= 0; i--) {
        heapify(arr, sz, i);
    }
}

void updateTickets(List* list, int filterId, int newTickets) {
    Node* it = list->head;
    while (it) {
        if (it->data.id == filterId) {
            it->data.tickets = newTickets;
            return;
        }
        it = it->next;
    }
}

TrainCar pop(TrainCar* heap, int* sz) {
    if (*sz == 0) {
        TrainCar t = { 0, NULL, 0, 0 };
        return t;
    }

    TrainCar aux = heap[0];
    heap[0] = heap[(*sz) - 1];
    heap[(*sz) - 1] = aux;

    TrainCar value = heap[(*sz) - 1];

    (*sz)--;

    heapify(heap, *sz, 0);

    return value;
}

int main() {
    List list = { NULL, NULL };
    parseFile(&list, "data.txt");

    printList(&list);
    printListReverse(&list);

    deleteMinTickets(&list);
    printf("\nAfter deleting the TrainCar with the minimum amount of tickets: \n");
    printList(&list);

    TrainCar* arr = NULL;
    int sz = 0;
    arr = listToArray(&list, &sz);

    printf("\nArray:\n");
    for (int i = 0; i < sz; i++) {
        printTrainCar(arr[i]);
    }

    buildHeap(arr, sz);
    printf("\nHeap:\n");
    for (int i = 0; i < sz; i++) {
        printTrainCar(arr[i]);
    }

    TrainCar top = pop(arr, &sz);
    printf("\nPopped head:\n");
    printTrainCar(top);

    printf("\nHeap after pop:\n");
    for (int i = 0; i < sz; i++) {
        printTrainCar(arr[i]);
    }

    top = pop(arr, &sz);
    printf("\nPopped head:\n");
    printTrainCar(top);

    printf("\nHeap after pop:\n");
    for (int i = 0; i < sz; i++) {
        printTrainCar(arr[i]);
    }

    top = pop(arr, &sz);
    printf("\nPopped head:\n");
    printTrainCar(top);

    printf("\nHeap after pop:\n");
    for (int i = 0; i < sz; i++) {
        printTrainCar(arr[i]);
    }

    top = pop(arr, &sz);
    printf("\nPopped head:\n");
    printTrainCar(top);

    printf("\nHeap after pop:\n");
    for (int i = 0; i < sz; i++) {
        printTrainCar(arr[i]);
    }

    top = pop(arr, &sz);
    printf("\nPopped head:\n");
    printTrainCar(top);

    printf("\nHeap after pop:\n");
    for (int i = 0; i < sz; i++) {
        printTrainCar(arr[i]);
    }

    int filterId = 1;
    int newTickets = 1337;
    updateTickets(&list, filterId, newTickets);
    printf("\nUpdating tickets for TrainCar with id %d to %d:\n", filterId, newTickets);
    printList(&list);

    freeList(&list);
    free(arr);
}