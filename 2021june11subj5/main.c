#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Angajat {
    unsigned int departmentId;
    char* name;
    int salary;
} Angajat;

Angajat makeAngajat(unsigned int departmentId, char* name, int salary) {
    Angajat a = { departmentId, _strdup(name), salary };
    return a;
}

Angajat cloneAngajat(Angajat a) {
    return makeAngajat(a.departmentId, a.name, a.salary);
}

void printAngajat(Angajat a) {
    printf("Angajat: (depId: %u, name: %s, salary: %d)\n", a.departmentId, a.name, a.salary);
}

typedef struct PrimaryNode {
    unsigned int depId;
    struct PrimaryNode* next;
    struct SecondaryNode* employees;
} PrimaryNode;

typedef struct SecondaryNode {
    Angajat data;
    struct SecondaryNode* next;
} SecondaryNode;

void addDepartment(PrimaryNode** head, unsigned int depId) {
    PrimaryNode* newNode = (PrimaryNode*)malloc(sizeof(PrimaryNode));
    newNode->depId = depId;
    newNode->employees = NULL;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        newNode->next = *head;
        *head = newNode;
    }
}

void addAngajat(SecondaryNode** head, Angajat a) {
    SecondaryNode* newNode = (SecondaryNode*)malloc(sizeof(SecondaryNode));
    if (newNode == NULL) {
        return;
    }

    newNode->next = NULL;
    newNode->data = a;

    if (*head == NULL) {
        *head = newNode;
    } else {
        newNode->next = *head;
        *head = newNode;
    }
}

void parseFile(PrimaryNode** head, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return;
    }

    unsigned int depId;
    char name[256];
    int salary;
    while (fscanf(file, "%u \"%[^\"]\" %d", &depId, name, &salary) == 3) {
        Angajat a = makeAngajat(depId, name, salary);

        bool found = false;
        PrimaryNode* it = *head;
        while (it != NULL) {
            if (it->depId == a.departmentId) {
                found = true;
                break;
            }
            it = it->next;
        }

        if (found) {
            addAngajat(&(it->employees), a);
        } else {
            addDepartment(head, a.departmentId);
            addAngajat(&((*head)->employees), a);
        }
    }

    fclose(file);
}

void printListOfLists(PrimaryNode* head) {
    PrimaryNode* it = head;
    while (it) {
        printf("Department ID: %u\n", it->depId);
        printf("Employees:\n");
        SecondaryNode* sit = it->employees;
        while (sit) {
            printAngajat(sit->data);
            sit = sit->next;
        }
        it = it->next;
    }
}

void freeListOfLists(PrimaryNode** head) {
    PrimaryNode* it = NULL;
    while (*head) {
        it = *head;
        SecondaryNode* sit = NULL;
        while (it->employees) {
            sit = it->employees;
            it->employees = it->employees->next;
            free(sit->data.name);
            free(sit);
        }
        *head = (*head)->next;
        free(it);
    }
}

Angajat* filterBySalary(PrimaryNode* head, int salaryFilter, int* sz) {
    *sz = 0;
    Angajat* arr = NULL;

    PrimaryNode* it = head;
    while (it) {
        SecondaryNode* sit = it->employees;
        while (sit) {
            if (sit->data.salary > salaryFilter) {
                (*sz)++;
                Angajat* tmp = realloc(arr, sizeof(Angajat) * (*sz));
                tmp[(*sz) - 1] = cloneAngajat(sit->data);
                arr = tmp;
            }
            sit = sit->next;
        }
        it = it->next;
    }

    return arr;
}

typedef struct ListNode {
    struct ListNode* prev;
    struct ListNode* next;
    Angajat data;
} ListNode;

typedef struct Deque {
    ListNode* head;
    ListNode* tail;
} Deque;

void printDeque(Deque* d) {
    ListNode* it = d->head;
    while (it) {
        printAngajat(it->data);
        it = it->next;
    }
}

void freeDeque(Deque* d) {
    ListNode* it = NULL;
    while (d->head) {
        it = d->head;
        d->head = d->head->next;
        free(it->data.name);
        free(it);
    }
}

void pushLeft(Deque* d, Angajat a) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (newNode == NULL) {
        return;
    }

    newNode->prev = NULL;
    newNode->data = cloneAngajat(a);
    newNode->next = NULL;

    if (d->head == NULL) {
        d->head = newNode;
        d->tail = newNode;
    } else {
        newNode->next = d->head;
        d->head->prev = newNode;
        d->head = newNode;
    }
}

Angajat* popRight(Deque* d) {
    if (d->tail == NULL) {
        return NULL;
    }

    ListNode* tmp = d->tail;
    Angajat returnMe = tmp->data;

    if (d->head == d->tail) {
        d->head = NULL;
        d->tail = NULL;
    } else {
        d->tail = d->tail->prev;
        d->tail->next = NULL;
    }

    free(tmp);
    return &returnMe;
}

Deque empsFromDep(PrimaryNode* list, int depIdFilter) {
    Deque d = { NULL, NULL };

    PrimaryNode* it = list;
    while (it) {
        if (it->depId == depIdFilter) {
            SecondaryNode* sit = it->employees;
            while (sit) {
                pushLeft(&d, cloneAngajat(sit->data));
                sit = sit->next;
            }
            return d;
        }
        it = it->next;
    }

    return d;
}

// annoying to do
Angajat* lowestThreeSalaries(Deque* d) {
    
}

int main() {
    PrimaryNode* list = NULL;
    parseFile(&list, "data.txt");
    printListOfLists(list);

    Angajat* emps = NULL;
    int sz = 0;
    emps = filterBySalary(list, 40000, &sz);

    printf("\nEmployees filtered:\n");
    for (int i = 0; i < sz; i++) {
        printAngajat(emps[i]);
    }

    Deque dep100 = empsFromDep(list, 100);
    printf("\nQueued emps in dep 100:\n");
    printDeque(&dep100);

    printf("\nPopping all queue elements...\n");
    Angajat* aa = popRight(&dep100);
    printAngajat(*aa);
    aa = popRight(&dep100);
    printAngajat(*aa);
    aa = popRight(&dep100);
    printAngajat(*aa);
    aa = popRight(&dep100);
    printAngajat(*aa);
    aa = popRight(&dep100);
    if (aa == NULL) {
        printf("Can't pop anymore!\n");
    }

    freeListOfLists(&list);
    free(emps);
    freeDeque(&dep100);
}