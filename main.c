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
  struct date dat, until;
  enum action_type command;
  int installed;
  int upgraded;
  int removed;
};


int parse_date(char *arg, struct date *dat) {
  if (arg == NULL || *arg == '\0') return 0;
  dat->year = dat->month = dat->day = dat->hour = dat->minute = dat->second = -1;
  char *aux = arg;
  int c, i, field;
  for (field = 0; field < 6; ++field) {
    c = 0;
    while (isdigit(*aux)) {
      ++aux;
      ++c;
    }
    if (c > 0) {
      char buffer[c];
      for (i = 0; i < c; ++i) buffer[i] = arg[i];
      switch (field) {
	case 0:
	  dat->year = atoi(buffer);
	  break;
	case 1:
	  dat->month = atoi(buffer);
	  break;
	case 2:
	  dat->day= atoi(buffer);
	  break;
	case 3:
	  dat->hour= atoi(buffer);
	  break;
	case 4:
	  dat->minute = atoi(buffer);
	  break;
	case 5:
	  dat->second = atoi(buffer);
	  break;
      }
    }
    else return 0;
    
    if (*aux == '\0') return 1;
    else if (*aux != '-') return 0;
  
    ++aux;
    arg = aux;
  
  }
  return 0;
}

  
  
  
  
/* aptback remove --option installed,upgraded --date 2015-11-09-17-54-22 --until 2015-12-19
 * date format: 2015-11-09-17-54-22
 * minimum input is year. Then others wil be auto completed.*/
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key)
  {
    case 'd':
      if (!parse_date(arg, &arguments->dat)) argp_usage(state);
      break;
    case 'u':
      if (!parse_date(arg, &arguments->until)) argp_usage(state);
      break;
    case 'o':
      if (strcmp(arg, "remove") == 0) arguments->command = REMOVE;
      else if (strcmp(arg, "install") == 0) arguments->command = INSTALL;
      else if (strcmp(arg, "upgrade") == 0) arguments->command = UPGRADE;
      else argp_usage(state);
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