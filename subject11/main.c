// https://github.com/snaake20/ase_info_an_2_sem_2/blob/main/SDD/exercitii_examen/task%20-%20AVL%20LD%20V%20-%201/Task.pdf
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Task
{
    int id;
    char* name;
    char* deadline;
    int priority;
    int effort;
} Task;

Task makeTask(int id, char* name, char* deadline, int priority, int effort)
{
    Task task = {
        id,
        _strdup(name),
        _strdup(deadline),
        priority,
        effort
    };
    return task;
}

Task cloneTask(Task t)
{
    return makeTask(t.id, t.name, t.deadline, t.priority, t.effort);
}

void printTask(Task t)
{
    printf("Task: (id: %d, name: %s, deadline: %s, prio: %d, effort: %d)\n", t.id, t.name, t.deadline, t.priority, t.effort);
}

void freeTask(Task t)
{
    free(t.deadline);
    free(t.name);
}

typedef struct Node
{
    Task data;
    struct Node* left;
    struct Node* right;
} Node;

void inorderPrint(const Node* root)
{
    if (!root) return;

    inorderPrint(root->left);
    printTask(root->data);
    inorderPrint(root->right);
}

void freeTree(Node** root)
{
    if (!(*root)) return; 

    freeTree(&(*root)->left);
    freeTree(&(*root)->left);
    freeTask((*root)->data);
    free((*root));
    *root = NULL;
}


int treeHeight(const Node* root)
{
    if (!root) return 0;

    int lHeight = treeHeight(root->left);
    int rHeight = treeHeight(root->right);

    if (lHeight > rHeight) return 1 + lHeight;
    else return 1 + rHeight;
}

int subtreeHeightDiff(const Node* root)
{
    if (!root)
    {
        return 0;
    }
    else
    {
        return (treeHeight(root->left) - treeHeight(root->right));
    }
}

void rotateLeft(Node** root)
{
    Node* aux = (*root)->right;
    (*root)->right = aux->left;
    aux->left = *root;
    *root = aux;
}

void rotateRight(Node** root)
{
    Node* aux = (*root)->left;
    (*root)->left = aux->right;
    aux->right = *root;
    *root = aux;
}

void insert(Node** root, Task t)
{
    // AVL insertion is made up of 2 parts
    // 1. BST insertion
    // 2. balance check & rebalance

    // BST INSERTION
    if (*root == NULL)
    {
        Node* newNode = (Node*)calloc(1, sizeof(Node)); if (!newNode) exit(1);
        newNode->left = NULL;
        newNode->right = NULL;
        // shallow copy so we don't have to free any manually created tasks that we insert
        newNode->data = t;
        *root = newNode;
    }
    else
    {
        if (t.priority < (*root)->data.priority)
        {
            insert(&(*root)->left, t);
        }
        else
        {
            insert(&(*root)->right, t);
        }
    }
    // BST INSERTION END

    // BALANCE CHECK

    int hDelta = subtreeHeightDiff(*root);

    // left imbalance -> we will rotate right
    if (hDelta == 2)
    {
        int subtreeHeightDelta = subtreeHeightDiff((*root)->left);
        // right imbalance -> we will rotate left
        if (subtreeHeightDelta == -1)
        {
            rotateLeft(&(*root)->left);
        }
        rotateRight(root);
    }

    // right imbalance -> we will rotate left
    if (hDelta == -2)
    {
        int subtreeHeightDelta = subtreeHeightDiff((*root)->right);
        // left imbalance -> we will rotate right
        if (subtreeHeightDelta == 1)
        {
            rotateRight(&(*root)->right);
        }
        rotateLeft(root);
    }
    // BALANCE CHECK END
}

void parseFile(Node** root, const char* filename)
{
    FILE* f = NULL;
    f = fopen(filename, "r");
    if (f == NULL) exit(1);

    int id, prio, effort;
    char name[256], deadline[256];
    while (fscanf(f, "%d %s %s %d %d", &id, name, deadline, &prio, &effort) == 5)
    {
        Task t = makeTask(id, name, deadline, prio, effort);
        insert(root, t);
    }

    fclose(f);
}

Node* cloneTree(const Node* root)
{
    if (!root) return NULL;

    Node* clonedRoot = (Node*)calloc(1, sizeof(Node)); if (!clonedRoot) exit(1);
    clonedRoot->data = cloneTask(root->data);
    clonedRoot->left = cloneTree(root->left);
    clonedRoot->right = cloneTree(root->right);

    return clonedRoot;
}

void splitTreeIntoTwoBstClones(const Node* root, Node** rootOne, Node** rootTwo)
{
    if (!root)
    {
        *rootOne = NULL;
        *rootTwo = NULL;
    }
    else
    {
        *rootOne = cloneTree(root->left);
        Node* appendMe = (Node*)calloc(1, sizeof(Node));
        if (!appendMe)
        {
            exit(1);
        }
        appendMe->data = cloneTask(root->data);
        appendMe->left = *rootOne;
        *rootOne = appendMe;
        *rootTwo = cloneTree(root->right);
    }
}

typedef struct ListNode
{
    Task data;
    struct ListNode* next;
    struct ListNode* prev;
} ListNode;

typedef struct List
{
    ListNode* head;
    ListNode* tail;
} List;

void pushLeft(List* list, Task t)
{
    ListNode* newNode = (ListNode*)calloc(1, sizeof(ListNode));
    if (!newNode)
    {
        exit(1);
    }

    newNode->prev = NULL;
    newNode->data = cloneTask(t);

    // replaceHead
    if (!list->head && !list->tail)
    {
        newNode->next = NULL;
        list->head = newNode;
        list->tail = list->head;
    }
    // pushLeft
    else
    {
        newNode->next = list->head;
        list->head->prev = newNode;
        list->head = newNode;
    }
}

void printList(const List* list)
{
    printf("\nDoubly linked list: \n");
    ListNode* it = list->head;
    while (it)
    {
        printTask(it->data);
        it = it->next;
    }
}

void freeList(List* list)
{
    if (!list->head) return;

    while (list->head)
    {
        ListNode* it = list->head;
        list->head = list->head->next;
        freeTask(it->data);
        free(it);
    }
    list->head = NULL;
    list->tail = NULL;
}

void addToListInSortedOrderById(List* list, Task t)
{
    if (!list->head && !list->tail)
    {
        pushLeft(list, t);
    }
    else
    {
        if (list->head && t.id < list->head->data.id)
        {
            pushLeft(list, t);
        }
        else
        {
            ListNode* it = list->head;
            while (it && it->next)
            {
                if (t.id > it->data.id && t.id < it->next->data.id)
                {
                    ListNode* newNode = (ListNode*)calloc(1, sizeof(ListNode));
                    if (!newNode)
                    {
                        exit(1);
                    }
                    newNode->data = cloneTask(t);
                    newNode->next = it->next;
                    newNode->prev = it;
                    it->next->prev = newNode;
                    it->next = newNode;
                    return;
                }
                it = it->next;
            }

            // no node where we can insert our value before
            // which means it must be the last value in the list
            ListNode* newNode = (ListNode*)calloc(1, sizeof(ListNode));
            if (!newNode)
            {
                exit(1);
            }
            newNode->data = cloneTask(t);
            newNode->prev = list->tail;
            newNode->next = NULL;
            list->tail->next = newNode;
            list->tail = newNode;
        }
    }
}

void treeIntoList(const Node* root, List* list)
{
    if (!root) return;

    addToListInSortedOrderById(list, root->data);
    treeIntoList(root->left, list);
    treeIntoList(root->right, list);
}

Task* filterListIntoArrayByEffort(const List* list, size_t* arrSize, int minEffort)
{
    unsigned int filteredCount = 0;
    *arrSize = 0;

    ListNode* it = list->head;
    while (it)
    {
        if (it->data.effort >= minEffort)
        {
            filteredCount++;
        }
        it = it->next;
    }

    if (filteredCount == 0)
    {
        *arrSize = 0;
        return NULL;
    }
    else
    {
        Task* taskArr = (Task*)calloc(filteredCount, sizeof(Task));
        if (!taskArr)
        {
            exit(1);
        }
        *arrSize = filteredCount;

        size_t pos = 0;
        ListNode* it = list->head;
        while (it)
        {
            if (it->data.effort >= minEffort)
            {
                taskArr[pos++] = cloneTask(it->data);
            }
            it = it->next;
        }
        return taskArr;
    }
}

void freeArr(Task* arr, int sz)
{
    for (size_t i = 0; i < sz; i++)
    {
        freeTask(arr[i]);
    }
}

int main()
{
    Node* root = NULL;
    parseFile(&root, "data.txt");
    inorderPrint(root);

    Node* cloned = NULL;
    cloned = cloneTree(root);

    Node* rootOne = NULL, * rootTwo = NULL;
    splitTreeIntoTwoBstClones(root, &rootOne, &rootTwo);

    printf("\n\nLeft subtree:\n");
    inorderPrint(rootOne);
    printf("Left subtree height: %d\n", treeHeight(rootOne));
    printf("\n\Right subtree:\n");
    inorderPrint(rootTwo);
    printf("Right subtree height: %d\n", treeHeight(rootTwo));

    List list = {
        NULL,
        NULL
    };

    treeIntoList(rootOne, &list);
    printList(&list);

    Task* arr = NULL;
    size_t size;

    arr = filterListIntoArrayByEffort(&list, &size, 71);
    printf("\n\nArray: \n");
    for (size_t i = 0; i < size; i++)
    {
        printTask(arr[i]);
    }

    freeTree(&root);
    freeTree(&cloned);
    freeTree(&rootOne);
    freeTree(&rootTwo);
    freeList(&list);
    freeArr(arr, size);
}