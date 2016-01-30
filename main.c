#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#include "log_parse.h"
#include "debug.h"
#include "selection.h"
#include "mem.h"


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
    if (buffer == NULL) eperror("Failed to malloc at parse_option");
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
  const char *apt_path = "/var/log/apt/";
  const char *tmp_path = "/tmp/aptback/";
  const char *pipe_path = "/tmp/aptback/pipe";
  
  /* Input arguments processing */
  struct arguments args;
  init_args(&args);
  argp_parse(&argp, argc, argv, 0, 0, &args);

  /* Apt-log search and processing */
  /* debug
  char *filename = "ignore/hist.txt";
  FILE *source;
  if ((source = fopen(filename, "r")) == NULL) 
    perror(filename);
  */
  
  struct action ***actions;
  actions = malloc(sizeof(struct action **));
  if (actions == NULL) eperror("Failed to malloc actions at main");
  *actions = NULL;
  int num_act = 0;
  
  DIR *apt_dir;
  struct dirent *in_file;
  FILE *log_file;

  if ((apt_dir = opendir(apt_path)) == NULL) eperror("Failed to open log directory");
  /* future optimization: since log files are sorted by date, if logs are parsed by date, then there is no need to
   * parse all logs, only until we find the max date */
  if (mkdir(tmp_path, S_IRWXU | S_IROTH | S_IXOTH) == -1) // 0705 so zcat can read from the pipe
    eperror("Failed to create tmp directory");
  if (mknod(pipe_path, S_IFIFO | S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH, 0) != 0)
    eperror("Failed to create pipe");
  
  while ((in_file = readdir(apt_dir))) {
    if (!strcmp (in_file->d_name, "."))
      continue;
    if (!strcmp (in_file->d_name, ".."))    
      continue;
    
    if (starts_with(in_file->d_name, "history.log.")) { 
      int pid = fork();
      if (pid == 0) {
	int fd = open(pipe_path, O_WRONLY);
	if (fd == -1) eperror("Failed to open pipe to write");
	if (dup2(fd, 1) == -1) // change output channel to pipe
	  eperror("Failed to dup2 pipe");
	char path[strlen(apt_path) + strlen(in_file->d_name)];
	sprintf(path, "%s%s", apt_path, in_file->d_name);
	if (execlp("zcat", "zcat", path, NULL) == -1)
	  eperror("Failed to exec to zcat");
      }
      else if (pid == -1) eperror("Failed to fork process to execute zcat");
      /*else { This won't work because pipes are 64k in size and logs can be bigger
	int stat = 0;
	if (wait(&stat) == -1) eperror("Failed to wait for zcat");
	if (WIFEXITED(stat) == 0) eperror("zcat exited abnormally");
	log_file = fopen(pipe_path, "r");
	if (log_file == NULL) eperror("Failed to open pipe to read");
      }*/
      else {
	log_file = fopen(pipe_path, "r");
	if (log_file == NULL) eperror("Failed to open pipe to read");
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
      evaluate_line(line, &current, actions, &num_act);
      free(line);
      line = NULL;  
    }
    if (fclose(log_file) != 0) eperror("Failed to close log_file");
    
  }
  if (closedir(apt_dir) == -1) eperror("Failed to close log directory");
  // max size of pipe?
  if (unlink(pipe_path) == -1) eperror("Failed to remove pipe");
  if (rmdir(tmp_path) == -1) eperror("Failed to remove tmp directory");
  // remove tmp dir

  
  /* Actions selection based on input */
  struct action ***selected;
  selected = malloc(sizeof(struct action **));
  if (selected == NULL) eperror("Failed to malloc selected at main");
    
  *selected = NULL;
  int num_sel = selection(args, *actions, num_act, selected);
  
  // at this point some actions have been freed
  // DON'T USE ***actions AGAIN
  
  /* Apt-get call */
  int k, siz = 0;
  for (k = 0; k < num_sel; ++k)
    siz += (*selected)[k]->num_pack;
  char *apt_argv[siz+2+1]; // +2 for the first 2, +1 for the last NULL
  apt_argv[0] = "apt-get";
  if (args.command == INSTALL) apt_argv[1] = "install";
  else if (args.command == REMOVE) apt_argv[1] = "remove";
  else apt_argv[1] = "upgrade"; // should update before calling upgrade?
  apt_argv[siz+2] = NULL;
  int num = 2;
  for (k = 0; k < num_sel; ++k) {
    int l;
    for (l = 0; l < (*selected)[k]->num_pack; ++l) {
      apt_argv[num++] = (*selected)[k]->packages[l]->name;
    }
  }
  int pid = fork();
  if (pid == 0) {
    if (execvp(apt_argv[0], apt_argv) == -1) eperror("Failed to exec to apt-get"); // if sudo is needed it will tell
  }
  else if (pid == -1) eperror("Failed to fork process to execute apt-get");
  
  //printf("%d\n",num_sel);
  //debug_actions(*selected, num_sel);
  //debug_args(args);
  
  /* Free allocated memory left */
  int i;
  for (i = 0; i < num_sel; ++i)
    free_action((*selected)[i]);
  
  free(*selected);
  free(selected);
  
  free(*actions);
  free(actions);
  
  return 0;
  
}