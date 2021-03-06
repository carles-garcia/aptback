/*
Copyright (C) 2016  Carles Garcia Cabot  (github.com/carles-garcia)
This file is part of aptback, a tool to search, install, remove and upgrade 
packages logged by apt. Released under the GNU GPLv3 (see COPYING.txt)
*/
#include "print_search.h"

void print_search(struct darray *selected) {
  for (int i = 0; i < selected->size; ++i) {
    struct action *a = darray_get(selected, i);
    for (int j = 0; j < a->packages.size; ++j) {
      struct package *p = darray_pack_get(&a->packages, j);
      printf("%d-%02d-%02d  %02d:%02d", a->date.year, a->date.month, a->date.day, a->date.hour, a->date.minute);
      if (a->type == INSTALL) printf("  install");
      else if (a->type == REMOVE) printf("  remove");
      else if (a->type == PURGE) printf("  purge");
      else printf("  upgrade");
      printf("    %s  %s  (%s)", p->name, p->arch, p->version);
      if (p->automatic) printf("  automatic");
      else if (p->newversion != NULL) printf("-->(%s)", p->newversion);
      printf("\n");
    }
  }
}

void print_export(struct darray *selected, int version) {
  for (int i = 0; i < selected->size; ++i) {
    struct action *a = darray_get(selected, i);
    for (int j = 0; j < a->packages.size; ++j) {
      struct package *p = darray_pack_get(&a->packages, j);
      printf(" %s", p->name);
      if (version) printf("=%s", p->version);
    }
  }
  printf("\n");
}

void print_preview(struct darray *selected) {
  for (int i = 0, k = 0; i < selected->size; ++i) {
    struct action *a = darray_get(selected, i);
    for (int j = 0; j < a->packages.size; ++j, ++k) {
      struct package *p = darray_pack_get(&a->packages, j);
      printf("%s ", p->name);
      if (k == 5) {
	k = 0;
	printf("\n");
      }
    }
  }
  printf("\n\n");
}
