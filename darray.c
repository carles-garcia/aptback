#include "darray.h"

void init_darray(struct *darray d) {
  d->size = 0;
  d->capacity = 10;
  d->array = malloc(10 * sizeof(struct *action));
}

void darray_add(struct *darray d, struct *action obj) {
  if (d->size == d->capacity) {
    d->capacity *= 2;
    d->array = realloc(d->array, d->capacity * sizeof(obj);
  }
  d->array[size++] = obj;
}

struct *action darray_get(struct *darray d, int i) {
  return d->array[i];
}


void init_darray_pack(struct *darray_pack d) {
  d->size = 0;
  d->capacity = 10;
  d->array = malloc(10 * sizeof(struct *package));
}

void darray_pack_add(struct *darray d, struct *package obj) {
  if (d->size == d->capacity) {
    d->capacity *= 2;
    d->array = realloc(d->array, d->capacity * sizeof(obj));
  }
  d->array[size++] = obj;
}

struct *package darray_pack_get(struct *darray d, int i) {
  return d->array[i];
}


