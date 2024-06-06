#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Project {
    unsigned int id;
    char* title;
    char* contractor;
    unsigned char daysToImplement;
    float budget;
    float percentDone;
} Project;

Project makeProject(unsigned int id, char* title, char* contractor, unsigned char daysToImplement, float budget, float percentDone) {
    Project p = {
        id,
        _strdup(title),
        _strdup(contractor),
        daysToImplement,
        budget,
        percentDone
    };
    return p;
}

Project cloneProject(Project p) {
    return makeProject(p.id, p.title, p.contractor, p.daysToImplement, p.budget, p.percentDone);
}

void freeProject(Project p) {
    free(p.title);
    free(p.contractor);
}

void printProject(Project p) {
    printf("Project: (id: %u, title: %s, contractor: %s, duration: %hhu, budget: %.2f, progress: %.0f", 
        p.id, p.title, p.contractor, p.daysToImplement, p.budget, p.percentDone * 100);

    // used only so we can print "%"
    fwrite("%\n", 1, 2, stdout);
}

typedef struct Node {
    struct Node* left;
    struct Node* right;
    Project data;
} Node;

void bstInsert(Node** root, Project p) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) exit(1);

    newNode->left = NULL;
    newNode->right = NULL;

    // shallow copy
    newNode->data = p;

    if (*root == NULL) {
        *root = newNode;
    } else {
        if (p.id < (*root)->data.id) {
            bstInsert(&(*root)->left, p);
        } else {
            bstInsert(&(*root)->right, p);
        }
    }
}

void freeTree(Node** root) {
    if ((*root) == NULL) {
        return;
    }

    freeTree(&(*root)->left);
    freeTree(&(*root)->right);
    freeProject((*root)->data);
    free(*root);
    *root = NULL;
}

void preorderPrint(const Node* root) {
    if (root == NULL) {
        return;
    }

    printProject((*root).data);
    preorderPrint((*root).left);
    preorderPrint((*root).right);
}

void inorderPrint(const Node* root) {
    if (root == NULL) {
        return;
    }

    inorderPrint(root->left);
    printProject(root->data);
    inorderPrint(root->right);
}

void parseFile(Node** root, const char* fileName) {
    FILE* f = fopen(fileName, "r");

    if (f == NULL) {
        return;
    }

    unsigned int id;
    char title[256];
    char contractor[256];
    unsigned char days;
    float budget, completion;
    while (fscanf(f, "%u \"%[^\"]\" \"%[^\"]\" %hhu %f %f", &id, title, contractor, &days, &budget, &completion) == 6) {
        Project p = makeProject(id, title, contractor, days, budget, completion);
        bstInsert(root, p);
    }

    fclose(f);
}

int countByContractorName(const Node* root, char* contractorNameFilter) {
    if (root == NULL) {
        return 0;
    }

    if (strcmp(root->data.contractor, contractorNameFilter) == 0) {
        return 1 + countByContractorName(root->left, contractorNameFilter) + countByContractorName(root->right, contractorNameFilter);
    } else {
        return countByContractorName(root->left, contractorNameFilter) + countByContractorName(root->right, contractorNameFilter);
    }
}

void modifyDoneness(Node* root, unsigned int filterId, float newDonePercent) {
    if (root == NULL) {
        return;
    }

    if (root->data.id == filterId) {
        root->data.percentDone = newDonePercent;
    } else {
        if (filterId < root->data.id) {
            modifyDoneness(root->left, filterId, newDonePercent);
        } else {
            modifyDoneness(root->right, filterId, newDonePercent);
        }
    }

}

void removeLeaves(Node** root) {
    if (*root == NULL) {
        return;
    }

    if ((*root)->left == NULL && (*root)->right == NULL) {
        freeProject((*root)->data);
        free(*root);
        *root = NULL;
    } else {
        removeLeaves(&(*root)->left);
        removeLeaves(&(*root)->right);
    }
}

void getProjectArrWithBudgetsHigherThan(const Node* root, float filterBudget, Project** arr, int* sz) {
    if (root == NULL) {
        return;
    }

    // no idea why the realloc solution doesn't work lol
    if (root->data.budget >= filterBudget) {
        if (*arr == NULL) {
            *arr = (Project*)malloc(sizeof(Project));
            if (*arr == NULL) {
                exit(1);
            }

            *sz = 1;
            (*arr)[*sz - 1] = cloneProject(root->data);
        } else {

            Project* tmp = (Project*)malloc(sizeof(Project) * (*sz));
            // Project* tmp = realloc(*arr, (*sz) + 1);
            (*sz) = (*sz) + 1;
            if (tmp == NULL) {
                exit(1);
            }

            for (int i = 0; i < (*sz) - 1; i++) {
               tmp[i] = cloneProject((*arr)[i]);
            }
            tmp[(*sz) - 1] = cloneProject(root->data);

            for (int i = 0; i < (*sz) - 1; i++) {
               freeProject((*arr)[i]);
            }
            *arr = tmp;
        }
        getProjectArrWithBudgetsHigherThan(root->left, filterBudget, arr, sz);
        getProjectArrWithBudgetsHigherThan(root->right, filterBudget, arr, sz);
    } else {
        getProjectArrWithBudgetsHigherThan(root->left, filterBudget, arr, sz);
        getProjectArrWithBudgetsHigherThan(root->right, filterBudget, arr, sz);
    }
}

int main()
{
    Node* root = NULL;
    parseFile(&root, "data.txt");

    printf("Tree:\n");
    preorderPrint(root);

    printf("Found: %d\n", countByContractorName(root, "OpenAI"));

    modifyDoneness(root, 5, 0.99);

    preorderPrint(root);

    printf("\n\n\n");
    printf("Full tree (printed in Inorder):\n");
    inorderPrint(root);
    removeLeaves(&root);
    printf("\nTree after leaves were removed (printed in Inorder):\n");
    inorderPrint(root);

    Project* arr = NULL;
    int sz = 0;
    int filter = 667;
    getProjectArrWithBudgetsHigherThan(root, filter, &arr, &sz);

    printf("\nArray (projects with budget higher than %d): \n", filter);
    for (int i = 0; i < sz; i++) {
        printProject(arr[i]);
    }

    freeTree(&root);
}