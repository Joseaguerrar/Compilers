/*
 * valid_01_data_structures.c
 * Programa de prueba sintácticamente válido para C99/C11.
 * Implementación completa de estructuras de datos clásicas en C:
 * Vector dinámico, Lista enlazada simple, Pila, Cola, Árbol Binario de Búsqueda y Tabla Hash.
 * Líneas: > 500
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define VECTOR_INITIAL_CAPACITY 8
#define HASH_MAP_DEFAULT_BUCKETS 16

/* ========================================================================= */
/* 1. Vector Dinámico                                                        */
/* ========================================================================= */

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} Vector;

Vector *vector_create(void) {
    Vector *vec = (Vector *)malloc(sizeof(Vector));
    if (vec == NULL) {
        return NULL;
    }
    vec->size = 0;
    vec->capacity = VECTOR_INITIAL_CAPACITY;
    vec->data = (int *)malloc(vec->capacity * sizeof(int));
    if (vec->data == NULL) {
        free(vec);
        return NULL;
    }
    return vec;
}

int vector_push_back(Vector *vec, int value) {
    if (vec == NULL) {
        return 0;
    }
    if (vec->size >= vec->capacity) {
        size_t new_cap = vec->capacity * 2;
        int *new_data = (int *)realloc(vec->data, new_cap * sizeof(int));
        if (new_data == NULL) {
            return 0;
        }
        vec->data = new_data;
        vec->capacity = new_cap;
    }
    vec->data[vec->size] = value;
    vec->size++;
    return 1;
}

int vector_pop_back(Vector *vec, int *out_value) {
    if (vec == NULL || vec->size == 0) {
        return 0;
    }
    vec->size--;
    if (out_value != NULL) {
        *out_value = vec->data[vec->size];
    }
    return 1;
}

int vector_get(const Vector *vec, size_t index, int *out_value) {
    if (vec == NULL || index >= vec->size || out_value == NULL) {
        return 0;
    }
    *out_value = vec->data[index];
    return 1;
}

int vector_set(Vector *vec, size_t index, int value) {
    if (vec == NULL || index >= vec->size) {
        return 0;
    }
    vec->data[index] = value;
    return 1;
}

void vector_destroy(Vector *vec) {
    if (vec != NULL) {
        free(vec->data);
        free(vec);
    }
}

/* ========================================================================= */
/* 2. Lista Enlazada Simple                                                  */
/* ========================================================================= */

typedef struct ListNode {
    int val;
    struct ListNode *next;
} ListNode;

typedef struct {
    ListNode *head;
    ListNode *tail;
    size_t length;
} LinkedList;

LinkedList *list_create(void) {
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    return list;
}

int list_append(LinkedList *list, int val) {
    if (list == NULL) {
        return 0;
    }
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    if (node == NULL) {
        return 0;
    }
    node->val = val;
    node->next = NULL;

    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->length++;
    return 1;
}

int list_prepend(LinkedList *list, int val) {
    if (list == NULL) {
        return 0;
    }
    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    if (node == NULL) {
        return 0;
    }
    node->val = val;
    node->next = list->head;
    list->head = node;
    if (list->tail == NULL) {
        list->tail = node;
    }
    list->length++;
    return 1;
}

int list_contains(const LinkedList *list, int val) {
    if (list == NULL) {
        return 0;
    }
    ListNode *curr = list->head;
    while (curr != NULL) {
        if (curr->val == val) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

int list_remove_value(LinkedList *list, int val) {
    if (list == NULL || list->head == NULL) {
        return 0;
    }
    ListNode *curr = list->head;
    ListNode *prev = NULL;

    while (curr != NULL) {
        if (curr->val == val) {
            if (prev == NULL) {
                list->head = curr->next;
            } else {
                prev->next = curr->next;
            }
            if (curr == list->tail) {
                list->tail = prev;
            }
            free(curr);
            list->length--;
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return 0;
}

void list_destroy(LinkedList *list) {
    if (list == NULL) {
        return;
    }
    ListNode *curr = list->head;
    while (curr != NULL) {
        ListNode *tmp = curr->next;
        free(curr);
        curr = tmp;
    }
    free(list);
}

/* ========================================================================= */
/* 3. Pila (Stack)                                                           */
/* ========================================================================= */

typedef struct StackNode {
    int item;
    struct StackNode *below;
} StackNode;

typedef struct {
    StackNode *top;
    size_t count;
} Stack;

Stack *stack_create(void) {
    Stack *st = (Stack *)malloc(sizeof(Stack));
    if (st == NULL) {
        return NULL;
    }
    st->top = NULL;
    st->count = 0;
    return st;
}

int stack_push(Stack *st, int item) {
    if (st == NULL) {
        return 0;
    }
    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    if (node == NULL) {
        return 0;
    }
    node->item = item;
    node->below = st->top;
    st->top = node;
    st->count++;
    return 1;
}

int stack_pop(Stack *st, int *out_item) {
    if (st == NULL || st->top == NULL) {
        return 0;
    }
    StackNode *node = st->top;
    if (out_item != NULL) {
        *out_item = node->item;
    }
    st->top = node->below;
    free(node);
    st->count--;
    return 1;
}

int stack_peek(const Stack *st, int *out_item) {
    if (st == NULL || st->top == NULL || out_item == NULL) {
        return 0;
    }
    *out_item = st->top->item;
    return 1;
}

bool stack_is_empty(const Stack *st) {
    return (st == NULL || st->top == NULL);
}

void stack_destroy(Stack *st) {
    if (st == NULL) {
        return;
    }
    int dummy;
    while (stack_pop(st, &dummy)) {
        /* vaciar */
    }
    free(st);
}

/* ========================================================================= */
/* 4. Cola (Queue)                                                           */
/* ========================================================================= */

typedef struct QueueNode {
    int value;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    size_t count;
} Queue;

Queue *queue_create(void) {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q == NULL) {
        return NULL;
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    return q;
}

int queue_enqueue(Queue *q, int value) {
    if (q == NULL) {
        return 0;
    }
    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    if (node == NULL) {
        return 0;
    }
    node->value = value;
    node->next = NULL;
    if (q->rear == NULL) {
        q->front = node;
        q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    q->count++;
    return 1;
}

int queue_dequeue(Queue *q, int *out_value) {
    if (q == NULL || q->front == NULL) {
        return 0;
    }
    QueueNode *temp = q->front;
    if (out_value != NULL) {
        *out_value = temp->value;
    }
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    q->count--;
    return 1;
}

bool queue_is_empty(const Queue *q) {
    return (q == NULL || q->front == NULL);
}

void queue_destroy(Queue *q) {
    if (q == NULL) {
        return;
    }
    int dummy;
    while (queue_dequeue(q, &dummy)) {
        /* vaciar */
    }
    free(q);
}

/* ========================================================================= */
/* 5. Árbol Binario de Búsqueda (BST)                                       */
/* ========================================================================= */

typedef struct BSTNode {
    int key;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

typedef struct {
    BSTNode *root;
    size_t count;
} BST;

BST *bst_create(void) {
    BST *tree = (BST *)malloc(sizeof(BST));
    if (tree == NULL) {
        return NULL;
    }
    tree->root = NULL;
    tree->count = 0;
    return tree;
}

static BSTNode *bst_node_create(int key) {
    BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));
    if (node == NULL) {
        return NULL;
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static BSTNode *bst_insert_rec(BSTNode *root, int key, bool *inserted) {
    if (root == NULL) {
        *inserted = true;
        return bst_node_create(key);
    }
    if (key < root->key) {
        root->left = bst_insert_rec(root->left, key, inserted);
    } else if (key > root->key) {
        root->right = bst_insert_rec(root->right, key, inserted);
    } else {
        *inserted = false;
    }
    return root;
}

int bst_insert(BST *tree, int key) {
    if (tree == NULL) {
        return 0;
    }
    bool inserted = false;
    tree->root = bst_insert_rec(tree->root, key, &inserted);
    if (inserted) {
        tree->count++;
        return 1;
    }
    return 0;
}

static bool bst_search_rec(const BSTNode *root, int key) {
    if (root == NULL) {
        return false;
    }
    if (key == root->key) {
        return true;
    }
    if (key < root->key) {
        return bst_search_rec(root->left, key);
    }
    return bst_search_rec(root->right, key);
}

bool bst_contains(const BST *tree, int key) {
    if (tree == NULL) {
        return false;
    }
    return bst_search_rec(tree->root, key);
}

static void bst_inorder_rec(const BSTNode *root, int *buffer, size_t *index, size_t max_items) {
    if (root == NULL || *index >= max_items) {
        return;
    }
    bst_inorder_rec(root->left, buffer, index, max_items);
    if (*index < max_items) {
        buffer[*index] = root->key;
        (*index)++;
    }
    bst_inorder_rec(root->right, buffer, index, max_items);
}

size_t bst_to_sorted_array(const BST *tree, int *buffer, size_t max_items) {
    if (tree == NULL || buffer == NULL) {
        return 0;
    }
    size_t index = 0;
    bst_inorder_rec(tree->root, buffer, &index, max_items);
    return index;
}

static void bst_destroy_rec(BSTNode *root) {
    if (root == NULL) {
        return;
    }
    bst_destroy_rec(root->left);
    bst_destroy_rec(root->right);
    free(root);
}

void bst_destroy(BST *tree) {
    if (tree != NULL) {
        bst_destroy_rec(tree->root);
        free(tree);
    }
}

/* ========================================================================= */
/* 6. Tabla Hash con Encadenamiento (Hash Map)                               */
/* ========================================================================= */

typedef struct HashEntry {
    char *key;
    int value;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    HashEntry **buckets;
    size_t bucket_count;
    size_t entry_count;
} HashMap;

static unsigned long hash_function(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (int)*str++) != 0) {
        hash = ((hash << 5) + hash) + (unsigned long)c;
    }
    return hash;
}

HashMap *hashmap_create(size_t bucket_count) {
    if (bucket_count == 0) {
        bucket_count = HASH_MAP_DEFAULT_BUCKETS;
    }
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    if (map == NULL) {
        return NULL;
    }
    map->bucket_count = bucket_count;
    map->entry_count = 0;
    map->buckets = (HashEntry **)calloc(bucket_count, sizeof(HashEntry *));
    if (map->buckets == NULL) {
        free(map);
        return NULL;
    }
    return map;
}

int hashmap_put(HashMap *map, const char *key, int value) {
    if (map == NULL || key == NULL) {
        return 0;
    }
    unsigned long idx = hash_function(key) % map->bucket_count;
    HashEntry *entry = map->buckets[idx];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return 1;
        }
        entry = entry->next;
    }

    HashEntry *new_entry = (HashEntry *)malloc(sizeof(HashEntry));
    if (new_entry == NULL) {
        return 0;
    }
    new_entry->key = (char *)malloc(strlen(key) + 1);
    if (new_entry->key == NULL) {
        free(new_entry);
        return 0;
    }
    strcpy(new_entry->key, key);
    new_entry->value = value;
    new_entry->next = map->buckets[idx];
    map->buckets[idx] = new_entry;
    map->entry_count++;
    return 1;
}

int hashmap_get(const HashMap *map, const char *key, int *out_value) {
    if (map == NULL || key == NULL || out_value == NULL) {
        return 0;
    }
    unsigned long idx = hash_function(key) % map->bucket_count;
    HashEntry *entry = map->buckets[idx];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            *out_value = entry->value;
            return 1;
        }
        entry = entry->next;
    }
    return 0;
}

int hashmap_remove(HashMap *map, const char *key) {
    if (map == NULL || key == NULL) {
        return 0;
    }
    unsigned long idx = hash_function(key) % map->bucket_count;
    HashEntry *entry = map->buckets[idx];
    HashEntry *prev = NULL;

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            if (prev == NULL) {
                map->buckets[idx] = entry->next;
            } else {
                prev->next = entry->next;
            }
            free(entry->key);
            free(entry);
            map->entry_count--;
            return 1;
        }
        prev = entry;
        entry = entry->next;
    }
    return 0;
}

void hashmap_destroy(HashMap *map) {
    if (map == NULL) {
        return;
    }
    for (size_t i = 0; i < map->bucket_count; i++) {
        HashEntry *entry = map->buckets[i];
        while (entry != NULL) {
            HashEntry *tmp = entry->next;
            free(entry->key);
            free(entry);
            entry = tmp;
        }
    }
    free(map->buckets);
    free(map);
}

/* ========================================================================= */
/* 7. Función Principal de Pruebas (Driver)                                 */
/* ========================================================================= */

int main(void) {
    printf("=== Probando Vector Dinamico ===\n");
    Vector *vec = vector_create();
    for (int i = 0; i < 20; i++) {
        vector_push_back(vec, i * 10);
    }
    for (size_t i = 0; i < vec->size; i++) {
        int v = 0;
        vector_get(vec, i, &v);
        printf("vec[%zu] = %d\n", i, v);
    }
    vector_destroy(vec);

    printf("=== Probando Lista Enlazada ===\n");
    LinkedList *list = list_create();
    list_append(list, 100);
    list_append(list, 200);
    list_prepend(list, 50);
    printf("Lista contiene 200? %d\n", list_contains(list, 200));
    list_remove_value(list, 100);
    list_destroy(list);

    printf("=== Probando Pila y Cola ===\n");
    Stack *st = stack_create();
    stack_push(st, 1);
    stack_push(st, 2);
    stack_push(st, 3);
    int popped = 0;
    while (stack_pop(st, &popped)) {
        printf("Pila pop: %d\n", popped);
    }
    stack_destroy(st);

    Queue *q = queue_create();
    queue_enqueue(q, 10);
    queue_enqueue(q, 20);
    int dequeued = 0;
    while (queue_dequeue(q, &dequeued)) {
        printf("Cola dequeue: %d\n", dequeued);
    }
    queue_destroy(q);

    printf("=== Probando Arbol Binario ===\n");
    BST *tree = bst_create();
    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); i++) {
        bst_insert(tree, keys[i]);
    }
    int sorted[16];
    size_t sorted_count = bst_to_sorted_array(tree, sorted, 16);
    printf("Elementos ordenados del arbol:\n");
    for (size_t i = 0; i < sorted_count; i++) {
        printf("%d ", sorted[i]);
    }
    printf("\n");
    bst_destroy(tree);

    printf("=== Probando Hash Map ===\n");
    HashMap *map = hashmap_create(8);
    hashmap_put(map, "alfa", 1);
    hashmap_put(map, "beta", 2);
    hashmap_put(map, "gamma", 3);
    int val = 0;
    if (hashmap_get(map, "beta", &val)) {
        printf("map[\"beta\"] = %d\n", val);
    }
    hashmap_remove(map, "alfa");
    hashmap_destroy(map);

    printf("=== Pruebas de estructuras completadas con exito ===\n");
    return 0;
}
