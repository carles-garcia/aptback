#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include "log_parse.h"

int main() {
  
  char *filename = "hist.txt";
  FILE *source;
  if ((source = fopen(filename, "r")) == NULL) 
    perror(filename);
  
  struct action ***actions;
  actions = malloc(sizeof(struct action *));
  *actions = malloc(0 * sizeof(struct action *));
  int num_act = 0;
  
  struct action *current = NULL;
  char *line = NULL;  
  size_t n = 0;
  while (getline(&line, &n, source) >= 0) {
    evaluate_line(line, &current, actions, &num_act);
    //free(line);
  }
  
  if (fclose(source) != 0) perror(filename);
  int i;
  for (i = 0; i < 3; ++i) {
    int j;
    for (j = 0; j < (*actions)[i]->num_pack; ++j) {
      printf((*actions)[i]->packages[j]->name);
      printf("\n");
    }
  }
  return 0;
  
}