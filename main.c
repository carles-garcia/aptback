#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include "log_parse.h"


const char *argp_program_version = "aptback v0.1";
const char *argp_program_bug_address = "https://github.com/carles-garcia/aptback/issues";
static char doc[] = 
"aptback -- a utility to search, install and remove packages logged by apt";

static char args_doc[] = "install|remove|upgrade|search";

static struct argp_option options[] = {
  {"date"	'd', "DATE", 0, "Select packages in a date" },
  {"until",	'u', "DATE", 0, "If date option specified, select packages from the range date:until (both included)" },
  {"option",   	'o', "OPTIONS", 0, "Select packages that were installed, removed and/or upgraded" },
  { 0 }
};

struct arguments {
  char *date, *until, *options, *action;
};

/* aptback remove --option installed,updated --date 2015-11-09-17-54-22 --until 2015-12-19
 * date format: 2015-11-09-17-54-22
 * minimum input is year. Then others wil be auto completed.*/
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key)
  {
    case 'd':
      arguments->date = arg;
      break;
    case 'u':
      arguments->until = arg;
      break;
    case 'o':
      arguments->options = arg;
      break;
    case ARGP_KEY_ARG:
      arguments->action = arg;
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1) argp_usage(state);
      arguments->arg_num = state->arg_num;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main() {
  
  char *filename = "ignore/hist.txt";
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
  while (getline(&line, &n, source) > 0) { // or >= ???
    evaluate_line(line, &current, actions, &num_act);
    free(line);
    line = NULL;  
  }
  
  if (fclose(source) != 0) perror(filename);
  int i;
  for (i = 0; i < num_act; ++i) {
    int j;
    for (j = 0; j < (*actions)[i]->num_pack; ++j) {
      printf((*actions)[i]->packages[j]->name);
      printf("\n");
    }
  }
  return 0;
  
}