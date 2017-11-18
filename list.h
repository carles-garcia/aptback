#pragma once

#include <stdlib.h>

struct Node {
    void *data;
    struct Node* next;
};

struct List {
    struct Node* head;
    struct Node* tail;
    size_t size;
    void (*free_f)(void*);
};

struct List fnew_list(void (*free_func)(void*)) {
    struct List new_l = {0};
    new_l.free_f = free_func;
    return new_l
}

void list_add(struct List *list, void *data) {
    struct Node *new_node = malloc(sizeof(struct Node));
    if (new_node == NULL) eperror("Failed to add data to list");
    new_node->data = data;
    new_node->next = NULL;
    list->tail->next = new_node;
    list->tail = new_node;
    list->size += 1;
}

void list_free(struct List *list) {
    struct Node* aux = list->head;
    while (aux) {
        list->free_f(aux->data);
        struct Node* tmp = aux->next;
        free(aux);
        aux = tmp;
    }
}

