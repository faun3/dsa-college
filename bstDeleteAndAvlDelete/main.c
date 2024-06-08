#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <crtdbg.h>

typedef struct Data {
    int id;
} Data;

Data makeData(int id) {
    Data d = { id };
    return d;
}

typedef struct Node {
    struct Node* left;
    struct Node* right;
    Data data;
} Node;

void bstInsert(Node** root, Data d) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        exit(1);
    }

    newNode->left = NULL;
    newNode->right = NULL;
    newNode->data = d;

    if (*root == NULL) {
        *root = newNode;
    } else {
        if (d.id < (*root)->data.id) {
            bstInsert(&(*root)->left, d);
        } else {
            bstInsert(&(*root)->right, d);
        }
    }
}

int subtreeHeight(const Node* root) {
    if (root == NULL) {
        return 0;
    }

    int lHeight = subtreeHeight(root->left);
    int rHeight = subtreeHeight(root->right);

    if (lHeight > rHeight) {
        return 1 + lHeight;
    } else {
        return 1 + rHeight;
    }
}

int heightDiff(const Node* root) {
    if (root == NULL) {
        return 0;
    }

    return subtreeHeight(root->left) - subtreeHeight(root->right);
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

void avlInsert(Node** root, Data d) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        exit(1);
    }

    newNode->left = NULL;
    newNode->right = NULL;
    newNode->data = d;

    if (*root == NULL) {
        *root = newNode;
    } else {
        if (newNode->data.id < (*root)->data.id) {
            avlInsert(&(*root)->left, d);
        } else {
            avlInsert(&(*root)->right, d);
        }
    }

    if (heightDiff(*root) == 2) {
        if (heightDiff((*root)->left) == -1) {
            rotateLeft(&(*root)->left);
        }
        rotateRight(root);
    }

    if (heightDiff(*root) == -2) {
        if (heightDiff((*root)->right == 1)) {
            rotateRight(&(*root)->right);
        }
        rotateLeft(root);
    }
}

void rebalance(Node** root) {
    if (*root == NULL) {
        return;
    }

    if (heightDiff(*root) == 2) {
        if (heightDiff((*root)->left) == -1) {
            rotateLeft(&(*root)->left);
        }
        rotateRight(root);
    }

    if (heightDiff(*root) == -2) {
        if (heightDiff((*root)->right == 1)) {
            rotateRight(&(*root)->right);
        }
        rotateLeft(root);
    }
}

void preorderPrint(const Node* root) {
    if (root == NULL) {
        return;
    }

    printf("%d ", root->data.id);
    preorderPrint(root->left);
    preorderPrint(root->right);
}

Node* bstDeleteNode(Node* root, int deleteId) {
    if (root == NULL) {
        return root;
    }

    if (deleteId < root->data.id) {
        root->left = bstDeleteNode(root->left, deleteId);
        return root;
    } else if (deleteId > root->data.id) {
        root->right = bstDeleteNode(root->right, deleteId);
        return root;
    }

    if (root->left == NULL) {
        Node* tmp = root->right;
        free(root);
        return tmp;
    } else if (root->right == NULL) {
        Node* tmp = root->left;
        free(root);
        return tmp;
    }

    Node* parent = root;
    Node* successor = root->right;
    while (successor->left != NULL) {
        parent = successor;
        successor = successor->left;
    }

    root->data.id = successor->data.id;

    if (parent->left == successor) {
        parent->left = successor->right;
    } else {
        parent->right = successor->right;
    }

    free(successor);
    return root;
}

Node* avlDeleteNode(Node* root, int deleteId) {
    if (root == NULL) {
        return root;
    }

    if (deleteId < root->data.id) {
        root->left = avlDeleteNode(root->left, deleteId);
        rebalance(&root->left);
        return root;
    } else if (deleteId > root->data.id) {
        root->right = avlDeleteNode(root->right, deleteId);
        rebalance(&root->right);
        return root;
    }

    if (root->left == NULL) {
        Node* tmp = root->right;
        free(root);
        rebalance(&tmp);
        return tmp;
    } else if (root->right == NULL) {
        Node* tmp = root->left;
        free(root);
        rebalance(&tmp);
        return tmp;
    }

    Node* parent = root;
    Node* successor = root->right;
    while (successor->left != NULL) {
        parent = successor;
        successor = successor->left;
    }

    root->data.id = successor->data.id;

    if (parent->left == successor) {
        parent->left = successor->right;
    } else {
        parent->right = successor->right;
    }

    free(successor);
    return root;
}

freeTree(Node** root) {
    if (*root != NULL) {
        freeTree(&(*root)->left);
        freeTree(&(*root)->right);
        free(*root);
        *root = NULL;
    }
}

void splitIntoOddsAndEvens(Node* root, Node** rootOdds, Node** rootEvens) {
    if (root == NULL) {
        return;
    }

    if (root->data.id % 2 == 0) {
        bstInsert(rootOdds, root->data);
    } else {
        bstInsert(rootEvens, root->data);
    }

    splitIntoOddsAndEvens(root->left, rootOdds, rootEvens);
    splitIntoOddsAndEvens(root->right, rootOdds, rootEvens);
}

int main() {
    Node* root = NULL;

    avlInsert(&root, makeData(4));
    avlInsert(&root, makeData(7));
    avlInsert(&root, makeData(2));
    avlInsert(&root, makeData(1));
    avlInsert(&root, makeData(3));
    avlInsert(&root, makeData(5));
    avlInsert(&root, makeData(6));

    printf("\nBefore BST delete:\n");
    preorderPrint(root);
    printf("\n");

    root = bstDeleteNode(root, 7);

    printf("\After BST delete:\n");
    preorderPrint(root);
    printf("\n");

    /*Node* rootEvens = NULL;
    Node* rootOdds = NULL;

    splitIntoOddsAndEvens(root, &rootOdds, &rootEvens);

    printf("\nOdds:\n");
    preorderPrint(rootOdds);

    printf("\nEvens:\n");
    preorderPrint(rootEvens);*/

    root = avlDeleteNode(root, 4);

    preorderPrint(root);

    freeTree(&root);
    //freeTree(&rootOdds);
    //freeTree(&rootEvens);

    _CrtDumpMemoryLeaks();
}