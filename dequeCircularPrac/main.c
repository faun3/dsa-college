#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    int id;
    char* name;
    float gpa;
} Student;

Student makeStudent(int id, char* name, float gpa) {
    Student s = { id, _strdup(name), gpa };
    return s;
}

void printStudent(Student s) {
    printf("Student: (id: %d, name: %s, gpa: %.1f)\n", s.id, s.name, s.gpa);
}

typedef struct Node {
    struct Node* prev;
    struct Node* next;
    Student data;
} Node;

typedef struct Deque {
    Node* head;
    Node* tail;
} Deque;

void printDeq(const Deque* d) {
    Node* it = d->head;
    while (it != NULL) {
        printStudent(it->data);
        it = it->next;
    }
}

void printDeqReverse(const Deque* d) {
    Node* it = d->tail;
    while (it != NULL) {
        printStudent(it->data);
        it = it->prev;
    }
}

void freeDeq(Deque* d) {
    Node* it = NULL;
    while (d->head) {
        it = d->head;
        d->head = d->head->next;
        free(it->data.name);
        free(it);
    }
}

void pushLeft(Deque* deque, Student s) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        exit(1);
    }

    newNode->next = deque->head;
    newNode->prev = NULL;
    newNode->data = s;

    if (deque->head == NULL && deque->tail == NULL) {
        deque->head = newNode;
        deque->tail = newNode;
    } else {
        deque->head->prev = newNode;
        deque->head = newNode;
    }
}

void pushRight(Deque* deque, Student s) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        exit(1);
    }

    newNode->prev = deque->tail;
    newNode->next = NULL;
    newNode->data = s;

    if (deque->head == NULL && deque->tail == NULL) {
        deque->head = newNode;
        deque->tail = newNode;
    } else {
        deque->tail->next = newNode;
        deque->tail = newNode;
    }
}

Student popLeftByValue(Deque* deque) {
    Student tmp = { -1, NULL, -1 };
    if (deque->head == NULL) {
        return tmp;
    }

    tmp = deque->head->data;
    Node* freeMe = deque->head;
    deque->head = deque->head->next;
    deque->head->prev = NULL;
    if (deque->head == NULL) {
        deque->tail = NULL;
    }
    free(freeMe);
    return tmp;
}

Student popRightByValue(Deque* deque) {
    Student tmp = { -1, NULL, -1 };
    if (deque->tail == NULL) {
        return tmp;
    }

    tmp = deque->tail->data;
    Node* freeMe = deque->tail;
    deque->tail = deque->tail->prev;
    deque->tail->next = NULL;
    if (deque->tail == NULL) {
        deque->head = NULL;
    }
    free(freeMe);
    return tmp;
}

Student* popLeft(Deque* d) {
    if (d->head == NULL) {
        return NULL;
    }

    Node* tmp = d->head;
    Student* s = &(tmp->data);
    d->head = d->head->next;

    if (d->head == NULL) {
        d->tail = NULL;
    } else {
        d->head->prev = NULL;
    }

    return s;
}

Student* popRight(Deque* d) {
    if (d->head == NULL) {
        return NULL;
    }

    Student* s = &d->tail->data;
    Node* tmp = d->tail;

    if (d->head == d->tail) {
        d->head = NULL;
        d->tail = NULL;
    } else {
        d->tail = d->tail->prev;
        d->tail->next = NULL;
    }

    return s;
}

void parseFile(Deque* deque, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return;
    }

    int id;
    char name[256];
    float gpa;
    while (fscanf(file, "%d %s %f", &id, name, &gpa) == 3) {
        pushRight(deque, makeStudent(id, name, gpa));
    }

    fclose(file);
}

void printCircular(const Deque* l) {
    Node* it = l->head;
    do {
        printStudent(it->data);
        it = it->next;
    } while (it != l->head);
}

void freeCircular(Deque* l) {
    if (l->head == NULL) {
        return;
    }

    Node* it = l->head;
    Node* headRef = it;
    do {
        Node* next = it->next;
        free(it->data.name);
        free(it);
        it = next;
    } while (it != headRef);
    l->head = NULL;
    l->tail = NULL;
}

int main() {
    Deque deq = { NULL, NULL };

    parseFile(&deq, "data.txt");
    printf("\nNatural order print:\n");
    printDeq(&deq);
    printf("\nReverse order print:\n");
    printDeqReverse(&deq);

    Student* head = popLeft(&deq);
    Student* tail = popRight(&deq);

    printf("\nHead and tail values:\n");
    printStudent(*head);
    printStudent(*tail);

    printf("\nNatural order print after popLeft and popRight:\n");
    printDeq(&deq);

    freeDeq(&deq);
    free(head->name);
    free(tail->name);

    // rewind file
    FILE* file = fopen("data.txt", "r");
    if (file == NULL) {
        exit(666);
    }

    fseek(file, 0, 0);
    fclose(file);

    Deque circular = { NULL, NULL };
    parseFile(&circular, "data.txt");
    circular.tail->next = circular.head;
    printf("\nCircular list:\n");
    printCircular(&circular);
    freeCircular(&circular);
}