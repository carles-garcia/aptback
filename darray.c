/*
Copyright (C) 2016  Carles Garcia Cabot  (github.com/carles-garcia)
This file is part of aptback, a tool to search, install, remove and upgrade
packages logged by apt. Released under the GNU GPLv3 (see COPYING.txt)
*/
#include "darray.h"


void init_darray(struct darray *d) {
    d->size = 0;
    d->capacity = 10;
    d->array = malloc(10 * sizeof(struct action*)); //calloc?
    if (d->array == NULL) eperror("Failed to malloc darray");
}

void darray_add(struct darray *d, struct action *obj) {
    if (d->size == d->capacity) {
        d->capacity *= 2;
        d->array = realloc(d->array, d->capacity * sizeof(obj));
        if (d->array == NULL) eperror("Failed to realloc darray");
    }
    d->array[d->size++] = obj;
}

struct action* darray_get(struct darray *d, int i) {
    return d->array[i];
}

void free_darray(struct darray *d) {
    free(d->array);
}


void init_darray_pack(struct darray_pack *d) {
    d->size = 0;
    d->capacity = 10;
    d->array = malloc(10 * sizeof(struct package*));
    if (d->array == NULL) eperror("Failed to malloc darray");
}

void darray_pack_add(struct darray_pack *d, struct package *obj) {
    if (d->size == d->capacity) {
        d->capacity *= 2;
        d->array = realloc(d->array, d->capacity * sizeof(obj));
        if (d->array == NULL) eperror("Failed to realloc darray");
    }
    d->array[d->size++] = obj;
}

struct package* darray_pack_get(struct darray_pack *d, int i) {
    return d->array[i];
}

void free_darray_pack(struct darray_pack *d) {
    free(d->array);
}


void free_action(struct action *actions) {
    free(actions->command);
    free(actions->user);
    for (int j = 0; j < actions->packages.size; ++j)
        free_pack(darray_pack_get(&actions->packages, j));
    free_darray_pack(&actions->packages);
    free(actions);
}

void free_pack(struct package *pack) {
    free(pack->name);
    free(pack->arch);
    free(pack->version);
    free(pack->newversion);
    free(pack);
}

void init_action(struct action *current) {
    memset(&current->date, 0, sizeof(struct date));
    init_darray_pack(&(current->packages));
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
