#include "selection.h"

int select(struct arguments args, struct action **actions, int num_act, struct action ***selected) {
  int i, num_sel = 0;
  for (i = 0; i < num_act; ++i) {
    if (satisfies(args, actions[i]) {
      ++num_sel;
      *selected = realloc(*selected, *num_sel * sizeof(struct action *));
      (*selected)[*num_sel-1] = actions[i];
    }
  }
  return num_sel;
}
      
int satisfies(struct arguments args, struct action *actions);