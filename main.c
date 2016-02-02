#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "log_parse.h"
#include "debug.h"
#include "selection.h"
#include "darray.h"


const char *argp_program_version = "aptback v0.1";
const char *argp_program_bug_address = "https://github.com/carles-garcia/aptback/issues";
static char doc[] = 
"aptback -- a utility to search, install and remove packages logged by apt";

static char args_doc[] = "";

static struct argp_option options[] = {
  {"date",	'd', "DATE", 0, "Select packages in a date. DATE must be a valid date" },
  {"until",	'u', "DATE", 0, "If date option specified, select packages from the range date:until (both included)" },
  {"option",   	'o', "OPTIONS", 0, "Select packages that were installed, removed and/or upgraded" },
  { 0 }
};
// --remove-installed -ri
// --remove-upgraded -ru
// --install-removed -ir ; can't be chars
// aptback -i r,u -d 2015-7 -u 2015-7-21
// aptback install -o removed -d 2015-7 -u 2015-7-21
// aptback install --removed
// aptback remove -iu

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
  while (1) {
    int c = 0;
    while (isalpha(*aux)) {
      ++aux;
      ++c;
    }
    if (c != 0) {
      char *buffer = malloc((c+1) * sizeof(char));
      if (buffer == NULL) eperror("Failed to malloc at parse_option");
      for (int i = 0; i < c; ++i) buffer[i] = arg[i];
      buffer[i] = '\0';
      if (starts_with("installed", buffer) == 0) arguments->installed = 1;
      else if (starts_with("removed", buffer) == 0) arguments->removed = 1;
      else if (starts_with("upgraded", buffer) == 0) arguments->upgraded = 1;
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
    else return 0;
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
  const char *apt_path = "/var/log/apt/";

  /* Input arguments processing */
  struct arguments args;
  init_args(&args);
  argp_parse(&argp, argc, argv, 0, 0, &args);

  /* Apt-log search and processing */
  
  struct darray actions;
  init_darray(&actions);
  
  DIR *apt_dir;
  struct dirent *in_file;
  FILE *log_file;
  
  if ((apt_dir = opendir(apt_path)) == NULL) eperror("Failed to open log directory");
  /* future optimization: since log files are sorted by date, if logs are parsed by date, then there is no need to
   * parse all logs, only until we find the max date */
  
  while ((in_file = readdir(apt_dir))) {  
    int pid = -1;
    if (starts_with(in_file->d_name, "history.log.")) { 
      int fildes[2];
      if (pipe(fildes) == -1) eperror("Failed to create pipe");
      pid = fork();
      if (pid == 0) {
	if (close(fildes[0]) == -1) eperror("Failed to close read fildes");
	if (dup2(fildes[1], 1) == -1) eperror("Failed to dup2 pipe");
	if (close(fildes[1]) == -1) eperror("Failed to close fildes[1] after dup2");
	char path[strlen(apt_path) + strlen(in_file->d_name)];
	sprintf(path, "%s%s", apt_path, in_file->d_name);
	if (execlp("zcat", "zcat", path, NULL) == -1)
	  eperror("Failed to exec to zcat");
      }
      else if (pid == -1) eperror("Failed to fork process to execute zcat");
      else {
	if (close(fildes[1]) == -1) eperror("Failed to close write fildes");
	log_file = fdopen(fildes[0], "r");
	if (log_file == NULL) eperror("Failed to fdopen pipe to read");
      }
    }
    else if (strcmp(in_file->d_name, "history.log") == 0) {
      char path[strlen(apt_path) + strlen(in_file->d_name)];
      sprintf(path, "%s%s", apt_path, in_file->d_name);
      log_file = fopen(path, "r");
      if (log_file == NULL) eperror("Failed to open log_file to read");
    }
    else continue; // other irrelevant files
  
    struct action *current = NULL;
    char *line = NULL;  
    size_t n = 0;
    while (getline(&line, &n, log_file) > 0) { 
      evaluate_line(line, &current, &actions);
      free(line);
      line = NULL;
      n = 0;
    } 
    if (fclose(log_file) != 0) eperror("Failed to close log_file");
    if (pid != -1) {
      int status;
      if (waitpid(pid, &status, 0) == -1) eperror("waitpid failed");
      if (WIFEXITED(status) == 0) {
	fprintf(stderr, "\nzcat finished abnormally\n");
	exit(EXIT_FAILURE);
      }
    }
  }
  if (closedir(apt_dir) == -1) eperror("Failed to close log directory");
  
  /* Actions selection based on input */
  struct darray selected;
  init_darray(&selected);
  int total_packages = selection(args, &actions, &selected);
  
  if (total_packages != 0) {
    /* Apt-get call */ // should ask for confirmation before calling apt if action is install
    char *apt_argv[total_packages + 2 + 1]; // +2 for the first 2, +1 for the last NULL
    apt_argv[0] = "apt-get";
    if (args.command == INSTALL) apt_argv[1] = "install";
    else if (args.command == REMOVE) apt_argv[1] = "remove";
    else apt_argv[1] = "upgrade"; // should update before calling upgrade?
    apt_argv[total_packages + 2] = NULL;
    int num = 2;
    for (int k = 0; k < selected.size; ++k) {
      for (int l = 0; l < darray_get(&selected, k)->packages.size; ++l) {
	apt_argv[num++] = darray_pack_get(&(darray_get(&selected, k)->packages), l)->name;
      }
    }
    int pid = fork();
    if (pid == 0) {
      if (execvp(apt_argv[0], apt_argv) == -1) eperror("Failed to exec to apt-get"); // if sudo is needed it will tell
    }
    else if (pid == -1) eperror("Failed to fork process to execute apt-get");
    else {
      int status;
      if (waitpid(pid, &status, 0) == -1) eperror("waitpid for apt-get failed");
      if (WIFEXITED(status) == 0) {
	fprintf(stderr, "\napt-get finished abnormally\n");
	exit(EXIT_FAILURE);
      }
    }
  }
  else printf("No packages match the arguments");
  
  //debug_actions(*selected, num_sel);
  //debug_args(args);
  
  /* Free allocated memory left */
  for (int i = 0; i < actions.size; ++i)
    free_action(darray_get(&actions, i));
  free_darray(&actions);
  free_darray(&selected);

  return 0;
  
}