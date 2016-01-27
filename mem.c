#include "mem.h"

void free_action(struct action *actions) {
  // Free memory allocated for actions no longer needed
  // It was allocated in evaluate_line() from log_parse.c
  free(actions->command);
  int j;
  for (j=0; j < actions->num_pack; ++j) {
    free(actions->packages[j]->name);
    free(actions->packages[j]->arch);
    free(actions->packages[j]->version);
    if(actions->type == UPGRADE) free(actions->packages[j]->newversion);
    free(actions->packages[j]);
  }
  free(actions->packages);
  free(actions); 
}