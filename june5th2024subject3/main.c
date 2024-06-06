#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Appointment {
    char* date;
    char* clientName;
    float lengthInHours;
    char* services;
    int cost;
} Appointment;

Appointment makeAppointment(char* date, char* clientName, float lengthInHours, char* services, int cost) {
    Appointment a = {
        _strdup(date),
        _strdup(clientName),
        lengthInHours,
        _strdup(services),
        cost
    };
    return a;
}

Appointment cloneAppointment(Appointment a) {
    return makeAppointment(a.date, a.clientName, a.lengthInHours, a.services, a.cost);
}

void printAppointment(Appointment a) {
    printf("Appointment: (date: %s, clientName: %s, length: %.2f, services: %s, cost: %d)\n",
        a.date, a.clientName, a.lengthInHours, a.services, a.cost);
}

void freeAppointment(Appointment a) {
    free(a.clientName);
    free(a.date);
    free(a.services);
}

typedef struct SecondaryNode {
    Appointment data;
    struct SecondaryNode* next;
} SecondaryNode;

typedef struct MainNode {
    struct MainNode* next;
    SecondaryNode* listHead;
} MainNode;

void appendToTailSecondaryNode(SecondaryNode** head, Appointment a) {
    SecondaryNode* newNode = (SecondaryNode*)malloc(sizeof(SecondaryNode));
    if (newNode == NULL) {
        exit(1);
    }
    newNode->next = NULL;
    newNode->data = a;

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

void appendToTailMainNode(MainNode** head) {
    MainNode* newNode = (MainNode*)malloc(sizeof(MainNode));
    if (newNode == NULL) {
        exit(1);
    }
    newNode->next = NULL;
    newNode->listHead = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        MainNode* it = *head;
        while (it != NULL && it->next != NULL) {
            it = it->next;
        }

        it->next = newNode;
    }
}

void registerAppointment(MainNode** head, Appointment a) {
    if (*head == NULL) {
        MainNode* newNode = (MainNode*)malloc(sizeof(MainNode));
        if (newNode == NULL) {
            exit(1);
        }

        // make a new bucket for out appointment
        newNode->next = NULL;
        newNode->listHead = NULL;

        *head = newNode;

        appendToTailSecondaryNode(&(*head)->listHead, a);
    } else {
        // walk the main list and try to find the proper bucket for our Appointment
        MainNode* it = *head;
        while (it != NULL) {
            if (it->listHead != NULL) {
                if (strcmp(it->listHead->data.date, a.date) == 0) {
                    appendToTailSecondaryNode(&(it->listHead), a);
                    return;
                }
            }
            it = it->next;
        }

        it = *head;
        while (it != NULL && it->next != NULL) {
            it = it->next;
        }
        // if there is no proper bucket, add another MainNode to the list and set its listHead to the appointment
        MainNode* newNode = (MainNode*)malloc(sizeof(MainNode));
        if (newNode == NULL) {
            exit(1);
        }

        newNode->next = NULL;
        newNode->listHead = NULL;
        it->next = newNode;
        appendToTailSecondaryNode(&(newNode->listHead), a);
    }
}

void printSecondaryList(const SecondaryNode* head) {
    if (head == NULL) {
        return;
    }

    SecondaryNode* it = head;
    while (it != NULL) {
        printAppointment(it->data);
        it = it->next;
    }
}

void printMainList(const MainNode* head) {
    if (head == NULL) {
        exit(1);
    }

    int sz = 0;

    MainNode* it = head;
    while (it != NULL) {
        sz++;
        // printf("For bucket: %s\n", it->listHead->data.date);
        printSecondaryList(it->listHead);
        it = it->next;
    }
    printf("Main list has %d buckets\n", sz);
}

void parseFile(MainNode** head, const char* fileName) {
    FILE* f = fopen(fileName, "r");
    if (f == NULL) {
        return;
    }

    char date[256], name[256], service[256];
    float duration;
    int price;
    while (fscanf(f, "%s %s %f \"%[^\"]\" %d",
        date, name, &duration, service, &price) == 5) {
        Appointment a = makeAppointment(date, name, duration, service, price);
        registerAppointment(head, a);
    }

    fclose(f);
}

int totalValueAtCertainDate(const MainNode* head, char* filterDate) {
    int sum = 0;

    MainNode* it = head;
    while (it != NULL) {
        if (strcmp(it->listHead->data.date, filterDate) == 0) {
            SecondaryNode* w = it->listHead;
            while (w != NULL) {
                sum += w->data.cost;
                w = w->next;
            }
        }
        it = it->next;
    }

    return sum;
}

void freeSecondaryList(SecondaryNode** head) {
    if (*head == NULL) {
        return;
    }

    SecondaryNode* it = NULL;
    while (*head != NULL) {
        it = *head;
        (*head) = (*head)->next;
        freeAppointment(it->data);
        free(it);
    }
    *head = NULL;
}

void freeMainList(MainNode** head) {
    if (*head == NULL) {
        return;
    }

    MainNode* it = NULL;
    while (*head != NULL) {
        it = *head;
        (*head) = (*head)->next;
        freeSecondaryList(&(it->listHead));
        free(it);
    }
    *head = NULL;
}

void updateCostByClientName(const MainNode* head, char* clientNameFilter, int newCost) {
    if (head == NULL) {
        return;
    }

    MainNode* mit = head;
    while (mit != NULL) {
        SecondaryNode* sit = mit->listHead;
        while (sit != NULL) {
            if (strcmp(sit->data.clientName, clientNameFilter) == 0) {
                sit->data.cost = newCost;
                return;
            }
            sit = sit->next;
        }
        mit = mit->next;
    }
}

bool deleteOneSecondaryNodeWithDurationLessThan(SecondaryNode** head, float durationFilter) {
    if (*head == NULL) {
        return false;
    }

    // check if the head is the first element
    if ((*head)->data.lengthInHours < durationFilter) {
        SecondaryNode* newHead = (*head)->next;
        freeAppointment((*head)->data);
        free((*head));
        *head = newHead;
        return true;
    } else {
        SecondaryNode* it = *head;
        while (it != NULL && it->next != NULL) {
            if (it->next->data.lengthInHours < durationFilter) {
                SecondaryNode* deleteMe = it->next;
                SecondaryNode* afterDelete = deleteMe->next;
                it->next = afterDelete;
                freeAppointment(deleteMe->data);
                free(deleteMe);
                return true;
            }
            it = it->next;
        }
    }

    return false;
}

void filterWhereDurationLessThan(MainNode* head, float durationFilter) {
    MainNode* it = head;
    while (it != NULL) {
        bool isFound = false;
        do {
            isFound = deleteOneSecondaryNodeWithDurationLessThan(&(it->listHead), durationFilter);
        } while (isFound == true);
        it = it->next;
    }
}

typedef struct DoublyNode {
    struct DoublyNode* prev;
    struct DoublyNode* next;
    Appointment data;
} DoublyNode;

typedef struct List {
    DoublyNode* head;
    DoublyNode* tail;
} List;

void push(List* list, Appointment a) {
    DoublyNode* newNode = (DoublyNode*)malloc(sizeof(DoublyNode));
    if (newNode == NULL) {
        exit(1);
    }

    newNode->data = cloneAppointment(a);
    newNode->prev = NULL;

    if (list->head == NULL && list->tail == NULL) {
        newNode->next = NULL;
        list->tail = newNode;
        list->head = newNode;
    } else {
        newNode->next = list->head;
        list->head = newNode;
    }
}

void stackFromSameServices(const MainNode* head, List* stack, char* serviceFilter) {
    if (head == NULL) {
        return;
    }

    MainNode* it = head;
    while (it != NULL) {
        SecondaryNode* sit = it->listHead;
        while (sit != NULL) {
            if (strcmp(sit->data.services, serviceFilter) == 0) {
                push(stack, sit->data);
            }
            sit = sit->next;
        }
        it = it->next;
    }
}

void printStack(const List* stack) {
    DoublyNode* it = stack->head;
    while (it != NULL) {
        printAppointment(it->data);
        it = it->next;
    }
}

void freeStack(List* stack) {
    if (stack->head == NULL) {
        return;
    }

    DoublyNode* it = NULL;
    while (stack->head != NULL) {
        it = stack->head;
        stack->head = stack->head->next;
        freeAppointment(it->data);
        free(it);
    }
    stack->tail = NULL;
    stack->head = NULL;
}

int main() {
    MainNode* list = NULL;
    parseFile(&list, "data.txt");
    printf("\nAfter reading from file:\n");
    printMainList(list);

    const char* filterDate = "31-10-2024";
    printf("Total value of appointments on %s is: %d\n", filterDate, totalValueAtCertainDate(list, filterDate));

    updateCostByClientName(list, "Markus", 1337);

    printf("\nAfter updating cost:\n");
    printMainList(list);

    filterWhereDurationLessThan(list, 1.49);

    printf("\nAfter filtering:\n");
    printMainList(list);

    List stack = {
        NULL,
        NULL
    };

    stackFromSameServices(list, &stack, "Teeth Whitening");

    printf("\nStack:\n");
    printStack(&stack);

    freeMainList(&list);
    freeStack(&stack);
}