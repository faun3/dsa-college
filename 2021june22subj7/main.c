// https://github.com/snaake20/ase_info_an_2_sem_2/blob/main/SDD/exercitii_examen/spectacol%20-%202xBST%202xV%20-%201/Bilet7.pdf

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#include <io.h>
#include <fcntl.h>

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

int subtreeHeight(const Node* root) {
    if (!root) {
        return 0;
    }

    int lHeight = subtreeHeight(root->left);
    int rHeight = subtreeHeight(root->right);
    if (lHeight > rHeight) {
        return 1 + lHeight;
    } else {
        return 1 + rHeight;
    }
    exit(666);
}

int balanceDiff(const Node* root) {
    if (!root) {
        return 0;
    } else {
        return subtreeHeight(root->left) - subtreeHeight(root->right);
    }
}

void rotateLeft(Node** root) {
    Node* aux = (*root)->right;
    (*root)->right = aux->left;
    aux->left = *root;
    *root = aux;
}

void rotateRight(Node** root) {
    Node* aux = (*root)->left;
    (*root)->left = aux->right;
    aux->right = *root;
    *root = aux;
}

void avlInsert(Node** root, Show s) {
    bstInsert(root, s);
    
    if (balanceDiff(*root) == 2) {
        if (balanceDiff((*root)->left) == -1) {
            rotateLeft(&(*root)->left);
        }
        rotateRight(root);
    }

    if (balanceDiff(*root) == -2) {
        if (balanceDiff((*root)->left) == 1) {
            rotateRight(&(*root)->left);
        }
        rotateLeft(root);
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
        avlInsert(root, makeShow(id, price, name, date, seats, isRecorded));
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

void preorderPrint(const Node* root) {
    if (!root) {
        printf("NULL\n");
        return;
    }

    printShow(root->data);
    preorderPrint(root->left);
    preorderPrint(root->right);
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

typedef struct ListNode {
    struct ListNode* next;
    struct ListNode* prev;
    Node* data;
} ListNode;

typedef struct List {
    ListNode* head;
    ListNode* tail;
} List;

void pushLeft(List* list, Node* data) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (newNode == NULL) {
        exit(1);
    }

    newNode->prev = NULL;
    newNode->data = data;

    if (!list->head && !list->tail) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        newNode->next = list->head;
        list->head->prev = newNode;
        list->head = newNode;
    }
}

Node* popRight(List* list) {
    if (!list->tail && !list->head) {
        return NULL;
    } else if (list->head == list->tail) {
        Node* val = list->tail->data;
        list->tail = NULL;
        list->head = NULL;
        return val;
    } else {
        Node* val = list->tail->data;
        ListNode* deleteMe = list->tail;
        list->tail = list->tail->prev;
        free(deleteMe);
        return val;
    }
}

Node* popLeft(List* list) {
    if (!list->tail && !list->head) {
        return NULL;
    } else if (list->head == list->tail) {
        Node* val = list->head->data;
        list->tail = NULL;
        list->head = NULL;
        return val;
    } else {
        Node* val = list->head->data;
        ListNode* deleteMe = list->head;
        list->head = list->head->next;
        free(deleteMe);
        return val;
    }
}

Node* front(List queue) {
    return queue.head;
}

void levelOrderPrint(const Node* root) {
    List queue = {
        NULL, NULL
    };

    pushLeft(&queue, root);
    while (front(queue)) {
        Node* current = popRight(&queue);

        printShow(current->data);

        if (current->left) {
            pushLeft(&queue, current->left);
        }

        if (current->right) {
            pushLeft(&queue, current->right);
        }
    }
}

void dfs(const Node* root) {
    List stack = { NULL, NULL };
    pushLeft(&stack, root);

    while (stack.head != NULL) {
        Node* current = popLeft(&stack);

        printShow(current->data);

        if (current->left) {
            pushLeft(&stack, current->left);
        }

        if (current->right) {
            pushLeft(&stack, current->right);
        }
    }
}

// the inorder successor is the SMALLEST node that is still LARGER than the root
// it's very easy to check this for correctness:
//
// after an inorderPrint, the root's inorder successor should be printed right after the root
// note: in a BST, the inorderPrint will print the id's in increasing order
Node* inorderSuccessor(Node* root) {
    if (root == NULL) return NULL;

    Node* current = root->right;
    while (current != NULL && current->left != NULL) {
        current = current->left;
    }

    return current;
}

void deleteNode(Node** root, int deleteId) {
    if (*root == NULL) return;

    if (deleteId < (*root)->data.id) {
        deleteNode(&(*root)->left, deleteId);
    } else if (deleteId > (*root)->data.id) {
        deleteNode(&(*root)->right, deleteId);
    } else {
        // deletion happens on this branch

        if ((*root)->left == NULL || (*root)->right == NULL) {
            // 0 or 1 child case

            // get the replacement child
            Node* replace = (*root)->left != NULL ? (*root)->left : (*root)->right;

            // only used for freeing
            Node* tmp = *root;

            // replace the root
            *root = replace;

            free(tmp->data.name);
            free(tmp);
        } else {
            // both children case
            // we need to get the inorded successor and replace the current root with that
            Node* replacement = inorderSuccessor(*root);
            (*root)->data = cloneShow(replacement->data);
            deleteNode(&(*root)->right, replacement->data.id);
        }
    }

    // add this code if deleting from an AVL tree
    // if (*root == NULL) return;
    // rebalance(root);
}

int main() {

    Node* root = NULL;
    parseFile(&root, "data.txt");

    dfs(root);

    printf("Diff: %d\n", balanceDiff(root));

    printf("\nLevel order:\n");
    levelOrderPrint(root);

    deleteNode(&root, 5);

    printf("\nLevel order after node deletion:\n");
    levelOrderPrint(root);

    freeTree(&root);
}