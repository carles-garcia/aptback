#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include "log_parse.h"
#include "debug.h"

const char *argp_program_version = "aptback v0.1";
const char *argp_program_bug_address = "https://github.com/carles-garcia/aptback/issues";
static char doc[] = 
"aptback -- a utility to search, install and remove packages logged by apt";

static char args_doc[] = "install|remove|upgrade|search";

static struct argp_option options[] = {
  {"date",	'd', "DATE", 0, "Select packages in a date. DATE must be a valid date" },
  {"until",	'u', "DATE", 0, "If date option specified, select packages from the range date:until (both included)" },
  {"option",   	'o', "OPTIONS", 0, "Select packages that were installed, removed and/or upgraded" },
  { 0 }
};


int parse_date(char *arg, struct date *dat) {
  if (arg == NULL || *arg == '\0') return 0;
  char *aux = arg;
  int c, i, field;
  for (field = 0; field < 6; ++field) {
    c = 0;
    while (isdigit(*aux)) {
      ++aux;
      ++c;
    }
    if (c > 0) {
      char buffer[c+1];
      for (i = 0; i < c; ++i) buffer[i] = arg[i];
      buffer[i] = '\0';
      switch (field) {
	case 0:
	  dat->year = atoi(buffer);
	  if (dat->year <= 0 || dat->year >= 3000) return 0; //range check (a bit arbitrary)
	  break;
	case 1:
	  dat->month = atoi(buffer);
	  if (dat->month < 1 || dat->month > 12) return 0;
	  break;
	case 2:
	  dat->day= atoi(buffer);
	  if (dat->day < 1 || dat->day > 31) return 0;
	  break;
	case 3:
	  dat->hour= atoi(buffer);
	  if (dat->hour < 0 || dat->hour > 24) return 0;
	  break;
	case 4:
	  dat->minute = atoi(buffer);
	  if (dat->minute < 0 || dat->minute > 59) return 0;
	  break;
	case 5:
	  dat->second = atoi(buffer);
	  if (dat->second < 0 || dat->second > 59) return 0;
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

int parse_option(char *arg, struct arguments *arguments) {
  char *aux = arg;
  int c;
  while (1) {
    c = 0;
    while (isalpha(*aux)) {
      ++aux;
      ++c;
    }
    char *buffer = malloc((c+1) * sizeof(char));
    int i;
    for (i = 0; i < c; ++i) buffer[i] = arg[i];
    buffer[i] = '\0';
    if (strcmp("installed", buffer) == 0) arguments->installed = 1;
    else if (strcmp("removed", buffer) == 0) arguments->removed = 1;
    else if (strcmp("upgraded", buffer) == 0) arguments->upgraded = 1;
    else {
      free(buffer);
      return 0;
    }
    free(buffer);
    if (*aux == '\0') break;
    else if (*aux != ',') return 0;
    ++aux;
    arg = aux;
  }
  return 1;
}
  
  
/* aptback remove -o installed,upgraded -d 2015-11-09-17-54-22 -u 2015-12-19
 * date format: 2015-11-09-17-54-22
 * minimum input is year. Then others wil be auto completed.*/
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
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
      if (!parse_option(arg, arguments)) argp_usage(state);
      break;
    case ARGP_KEY_ARG:
      if (strcmp(arg, "remove") == 0) arguments->command = REMOVE;
      else if (strcmp(arg, "install") == 0) arguments->command = INSTALL;
      else if (strcmp(arg, "upgrade") == 0) arguments->command = UPGRADE;
      else argp_usage(state);
      break;
    case ARGP_KEY_END:
      if (state->arg_num != 1) argp_usage(state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

void init_args(struct arguments *args) {
  args->installed = args->removed = args->upgraded = 0;
  args->until.year = args->until.month = args->until.day = args->until.hour = args->until.minute = args->until.second = -1;
  args->dat.year = args->dat.month = args->dat.day = args->dat.hour = args->dat.minute = args->dat.second = -1;
  args->command = UNDEFINED;
}

int main(int argc, char *argv[]) {
  struct arguments args;
  init_args(&args);
  argp_parse(&argp, argc, argv, 0, 0, &args);
  
  
  char *filename = "ignore/hist.txt";
  FILE *source;
  if ((source = fopen(filename, "r")) == NULL) 
    perror(filename);
  
  struct action ***actions;
  actions = malloc(sizeof(struct action **));
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
  
  struct action ***selected;
  selected = malloc(sizeof(struct action **));
  *selected = malloc(0 * sizeof(struct action *));
  int num_sel = select(args, *actions, num_act, selected);
  
  //debug_actions(actions, num_act);
  debug_args(args);
  
  return 0;
  
}

