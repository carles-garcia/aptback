#include "print_search.h"

void print_search(struct darray *selected) {
  for (int i = 0; i < selected->size; ++i) {
    struct action *a = darray_get(selected, i);
    for (int j = 0; j < a->packages.size; ++j) {
      struct package *p = darray_pack_get(&a->packages, j);
      printf("%d-%02d-%02d %02d:%02d ", a->date.year, a->date.month, a->date.day, a->date.hour, a->date.minute);
      if (a->type == INSTALL) printf("install ");
      else if (a->type == REMOVE) printf("remove ");
      else printf("upgrade ");
      printf("%s %s %s ", p->name, p->arch, p->version);
      if (a->type == INSTALL) printf("automatic");
      else if (p->newversion != NULL) printf(" -> %s", p->newversion);
      printf("\n");
    }
  }
}