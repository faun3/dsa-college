// https://github.com/snaake20/ase_info_an_2_sem_2/blob/main/SDD/exercitii_examen/spectacol%20-%202xBST%202xV%20-%201/Bilet7.pdf

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

typedef struct Show {
    int id;
    float price;
    char* name;
    char date[10];
    int seats;
    bool isRecorded;
} Show;

Show makeShow(int id, float price, char* name, char date[10], int seats, bool isRecorded) {
    Show s;
    s.id = id;
    s.price = price;
    s.name = _strdup(name);
    strcpy(s.date, date);
    s.seats = seats;
    s.isRecorded = isRecorded;
    return s;
}

Show cloneShow(Show s) {
    return makeShow(s.id, s.price, s.name, s.date, s.seats, s.isRecorded);
}

void printShow(Show s) {
    printf("Show: (id: %d, price: %.2f, name: %s, date: %s, seats: %d", s.id, s.price, s.name, s.date, s.seats);
    if (s.isRecorded) {
        printf(", isRecorded: true)\n");
    } else {
        printf(", isRecorded: false)\n");
    }
}

typedef struct Node {
    struct Node* left;
    struct Node* right;
    Show data;
} Node;

void bstInsert(Node** root, Show s) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        exit(1);
    }

    newNode->left = NULL;
    newNode->right = NULL;
    newNode->data = s;

    if (*root == NULL) {
        *root = newNode;
    } else {
        if (s.id < (*root)->data.id) {
            bstInsert(&(*root)->left, s);
        } else {
            bstInsert(&(*root)->right, s);
        }
    }
}

void parseFile(Node** root, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        exit(1);
    }

    int id, seats;
    char name[256], date[11];
    float price;
    bool isRecorded;
    while (fscanf(f, "%d %f %s %s %d %hhu", &id, &price, name, date, &seats, &isRecorded) == 6) {
        bstInsert(root, makeShow(id, price, name, date, seats, isRecorded));
    }

    fclose(f);
}

void inorderPrint(const Node* root) {
    if (!root) {
        return;
    }

    inorderPrint(root->left);
    printShow(root->data);
    inorderPrint(root->right);
}

void freeTree(Node** root) {
    if (*root == NULL) {
        return;
    }

    freeTree(&(*root)->left);
    freeTree(&(*root)->right);
    free((*root)->data.name);
    free(*root);
}

int main() {
    Node* root = NULL;
    parseFile(&root, "data.txt");

    inorderPrint(root);

    freeTree(&root);
}