#pragma once

#include <stdlib.h>

struct darray {
  int size, capacity;
  struct action *array[];
}

void init_darray(struct *darray d);

void darray_add(struct *darray d, struct *action obj);

struct *action darray_get(struct *darray d, int i);

struct darray_pack {
  int size, capacity;
  struct package *array[];
}

void init_darray_pack(struct *darray_pack d);

void darray_pack_add(struct  *darray_pack d, struct *package obj);

struct *package darray_pack_get(struct  *darray_pack d, int i);
