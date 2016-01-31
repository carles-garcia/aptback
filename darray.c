#include "darray.h"

void init_darray(struct *darray d) {
  d->size = 0;
  d->capacity = 10;
  d->array = malloc(10 * sizeof(struct *action));
  if (d->array == NULL) eperror("Failed to malloc darray");
}

void darray_add(struct *darray d, struct *action obj) {
  if (d->size == d->capacity) { //instead of adding obj, create it a return it?
    d->capacity *= 2;
    d->array = realloc(d->array, d->capacity * sizeof(obj);
    if (d->array == NULL) eperror("Failed to realloc darray");
  }
  d->array[size++] = obj;
}

struct *action darray_get(struct *darray d, int i) {
  return d->array[i];
}

void free_darray(struct *darray d) {
  for (int i = 0; i < d->size; ++i) 
    free_action(d->array[i]);
  free(d->array);
}


void init_darray_pack(struct *darray_pack d) {
  d->size = 0;
  d->capacity = 10;
  d->array = malloc(10 * sizeof(struct *package));
  if (d->array == NULL) eperror("Failed to malloc darray");
}

void darray_pack_add(struct *darray d, struct *package obj) {
  if (d->size == d->capacity) {
    d->capacity *= 2;
    d->array = realloc(d->array, d->capacity * sizeof(obj));
    if (d->array == NULL) eperror("Failed to realloc darray");
  }
  d->array[size++] = obj;
}

struct *package darray_pack_get(struct *darray d, int i) {
  return d->array[i];
}


void free_darray_pack(struct *darray_pack d) {
  for (int i = 0; i < d->size; ++i) 
    free_pack(d->array[i]);
  free(d->array);
}