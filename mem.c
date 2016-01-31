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

void eperror(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

/*

void free_action(struct action *action) {
  free(action->command);
  free_darray_pack(actions->upg_packages);
  free_darray_pack(actions->ins_packages);
  free_darray_pack(actions->rem_packages);
  free(action); 
}

void free_pack(struct package *pack) {
  free(pack->name);
  free(pack->arch);
  free(pack->version);
  if (pack->newversion != NULL) free(pack->newversion);
  free(pack);
}

*/
  