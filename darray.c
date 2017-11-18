/*
Copyright (C) 2016  Carles Garcia Cabot  (github.com/carles-garcia)
This file is part of aptback, a tool to search, install, remove and upgrade
packages logged by apt. Released under the GNU GPLv3 (see COPYING.txt)
*/
#include "darray.h"

void init_darray(struct darray *d, void (*free_f)(void*)) {
    d->size = 0;
    d->capacity = 16;
    d->data = malloc(16 * sizeof(void*)); //calloc?
    if (d->data == NULL) eperror("Failed to malloc darray");
    d->free_f = free_f;
}

void darray_add(struct darray *d, void *obj) {
    if (d->size == d->capacity) {
        d->capacity *= 2;
        d->data = realloc(d->data, d->capacity * sizeof(void*));
        if (d->data == NULL) eperror("Failed to realloc darray");
    }
    d->data[d->size++] = obj;
}

void free_darray(struct darray *d) {
    for (size_t i = 0; i < d->size; ++i) {
        d->free_f(d->data[i]);
    }
}

void free_action(void *act) {
    struct action *a = act;
    free(a->command);
    free(a->user);  
    free_darray(&a->packages);
    free(a);
}

void free_pack(void *pack) {
    struct package *p = pack;
    free(p->name);
    free(p->arch);
    free(p->version);
    free(p->newversion);
    free(p);
}

void init_action(struct action *current) {
    //memset(&current->date, 0, sizeof(struct date));
    current->date = (struct date){0};
    init_darray(&current->packages, &free_pack);
    current->command = NULL;
    current->type = UNDEFINED;
    current->user = NULL;
}

void init_pack(struct package *pack) {
    pack->name = NULL;
    pack->arch = NULL;
    pack->version = NULL;
    pack->newversion = NULL;
    pack->automatic = 0;
}

void eperror(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
