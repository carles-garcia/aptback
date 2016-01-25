#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include "log_parse.h"

int main() {
  
  char *filename = NULL;
  FILE *source;
  if ((source = fopen(filename, "r")) == NULL) 
    eperror(filename);
  
  struct action **actions;
  actions = malloc(0 * sizeof(struct action *));
  int num_act = 0;
  
  struct action *current = NULL;
  char *line = NULL;  
  while (getline(&line, 0, source) > 0) {
    evaluate_line(line, current, actions, &num_act);
    free(line);
  }
  
  if (fclose(source) != 0) eperror(filename);
  
}