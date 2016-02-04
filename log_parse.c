#include "log_parse.h"


static void copy_date(const struct action *source, struct action *dest) {
  dest->date.year = source->date.year;
  dest->date.month = source->date.month;
  dest->date.day = source->date.day;
  dest->date.hour = source->date.hour;
  dest->date.minute = source->date.minute;
  dest->date.second = source->date.second;
}

static struct action *duplicate(const struct action *current) {
  struct action *new_action = malloc(sizeof(struct action));
  if (new_action == NULL) eperror("Failed to malloc new_action at duplicate");
  copy_date(current, new_action);
  if (current->command != NULL) {
    new_action->command = malloc(strlen(current->command) * sizeof(char));
    if (new_action->command == NULL) eperror("Failed to malloc new_action command at duplicate");
    strcpy(new_action->command, current->command);
  }
  else new_action->command = NULL;
  init_darray_pack(&(new_action->packages));
  return new_action;
}

void evaluate_line(char *line, struct action **current, struct darray *actions, const struct arguments *args) {
  if (starts_with(line, "Start-Date")) {
    struct action *new_action = malloc(sizeof(struct action));
    if (new_action == NULL) eperror("Failed to malloc new_action at evaluate_line");
    init_action(new_action);
    *current = new_action;
    get_date(line, &(*current)->date);
    darray_add(actions, *current);
  }
  else if (starts_with(line, "Commandline")) { //not all actions have one 
    get_command(line, *current);
  }
  else if (starts_with(line, "Install")) { //not all actions have one
    if ((*current)->type != UNDEFINED) {
      struct action *new_action = duplicate(*current);
      new_action->type = INSTALL;
      get_packages(line, new_action, args);
      darray_add(actions, new_action);
    }
    else {
      (*current)->type = INSTALL; 
      get_packages(line, *current, args);
    }
  }
  else if (starts_with(line, "Remove")) { //not all actions have one
    if ((*current)->type != UNDEFINED) {
      struct action *new_action = duplicate(*current);
      new_action->type = REMOVE;
      get_packages(line, new_action, args);
      darray_add(actions, new_action);
    }
    else {
      (*current)->type = REMOVE;
      get_packages(line, *current, args);
    }
  }
  else if (starts_with(line, "Upgrade")) { // an action can have install and upgrade at the same time
    if ((*current)->type != UNDEFINED) {
      struct action *new_action = duplicate(*current);
      new_action->type = UPGRADE;
      get_packages(line, new_action, args);
      darray_add(actions, new_action);
    }
    else {
      (*current)->type = UPGRADE;
      get_packages(line, *current, args);
    }
  }
  else if (starts_with(line, "Purge")) {
    if ((*current)->type != UNDEFINED) {
      struct action *new_action = duplicate(*current);
      new_action->type = PURGE;
      get_packages(line, new_action, args);
      darray_add(actions, new_action);
    }
    else {
      (*current)->type = PURGE;
      get_packages(line, *current, args);
    }
  }
  /* Bad formatted lines and unrecognized options are ignored
   *	else if (!isspace(*line) && *line != '\0')
   *	  fprintf(stderr, "Possible bad formated line\n"); // for some reason the first line is bad formated
   */
}

int starts_with(char *line, char *string) {
  if (line == NULL || string == NULL) return 0;
  while(*string != '\0')
    if(*string++ != *line++)
      return 0;
  return 1;
}

void get_date(char *line, struct date *dat) {
  while (*line++ != ' '); //pass space
  char year[5];
  int i = 0;
  while (i < 4) year[i++] = *line++;
  year[i] = '\0';
  dat->year = atoi(year);
  ++line;
  char month[3];
  i = 0;
  while (i < 2) month[i++] = *line++;
  month[i] = '\0';
  dat->month = atoi(month);
  ++line;
  char day[3];
  i = 0;
  while (i < 2) day[i++] = *line++;
  day[i] = '\0';
  dat->day = atoi(day);
  ++line;
  
  while (isspace(*line)) ++line;
  char hour[3];
  i = 0;
  while (i < 2) hour[i++] = *line++;
  hour[i] = '\0';
  dat->hour = atoi(hour);
  ++line;
  char minute[3];
  i = 0;
  while (i < 2) minute[i++] = *line++;
  minute[i] = '\0';
  dat->minute = atoi(minute);
  ++line;
  char second[3];
  i = 0;
  while (i < 2) second[i++] = *line++;
  second[i] = '\0';
  dat->second = atoi(second);
  
}

void get_command(char *line, struct action *current) {
  while (!isspace(*line++)); 
  while (isspace(*line)) ++line;
  // now we have the command
  current->command = malloc((strlen(line)+1) * sizeof(char)); //will free when exiting program
  if (current->command == NULL) eperror("Failed to malloc command at get_command");
  strcpy(current->command, line); 
}

static int pack_satisfies(const struct package *new_pack, const struct arguments *args) {
  if (args->manual && new_pack->automatic) return 0;
  else if (args->automatic && !new_pack->automatic) return 0;
  else return 1;
}

void get_packages(char *line, struct action *current, const struct arguments *args) {
  while (*line++ != ' '); //pass space
  while (1) {
    struct package *new_pack = malloc(sizeof(struct package));
    if (new_pack == NULL) eperror("Failed to malloc new_pack at get_packages");
    init_pack(new_pack);
    char *line_aux = line;
    int c = 0;
    while (*line_aux != ':') {
      ++c; //number of chars in name
      ++line_aux;
    }
    ++line_aux;
    new_pack->name = malloc((c+1) * sizeof(char));
    if (new_pack->name == NULL) eperror("Failed to malloc name at get_packages");
    int i;
    for (i = 0; i < c; ++i) {
      new_pack->name[i] = line[i];
    }
    new_pack->name[i] = '\0';
    line = line_aux;
    c = 0;
    while (*line_aux != ' ') {
      ++c; //number of chars in arch
      ++line_aux;
    }
    ++line_aux;
    ++line_aux; //now we are at version
    new_pack->arch = malloc((c+1) * sizeof(char));
    if (new_pack->arch == NULL) eperror("Failed to malloc arch at get_packages");
    for (i = 0; i < c; ++i) {
      new_pack->arch[i] = line[i];
    }
    new_pack->arch[i] = '\0';
    line = line_aux;
    c = 0;
    while (*line_aux != ',' && *line_aux != ')') {
      ++c; //number of chars in version
      ++line_aux;
    }
    if (current->type == INSTALL && *line_aux == ',') new_pack->automatic = 1;
    else new_pack->automatic = 0;
    new_pack->version = malloc((c+1) * sizeof(char));
    if (new_pack->version == NULL) eperror("Failed to malloc version at get_packages");
    for (i = 0; i < c; ++i) {
      new_pack->version[i] = line[i];
    }
    new_pack->version[i] = '\0';
    if (current->type == UPGRADE && *line_aux == ',') {
      ++line_aux;
      ++line_aux;
      line = line_aux;
      c = 0;
      while (*line_aux != ')') {
	++c; //number of chars in version
	++line_aux;
      }
      new_pack->newversion = malloc((c+1) * sizeof(char));
      if (new_pack->newversion == NULL) eperror("Failed to malloc newversion at get_packages");
      for (i = 0; i < c; ++i) {
	new_pack->newversion[i] = line[i];
      }
      new_pack->newversion[i] = '\0';
    }
    
    //package finished
    if (pack_satisfies(new_pack, args))
      darray_pack_add(&(current->packages), new_pack);
    
    line = line_aux;
    ++line; // now it's either in ' ' or in ',' or end
    while (*line != ',' && *line != '\n') ++line;
    
    if (*line == '\n') break; // exit point: all packages checked 
    else {
      ++line;
      ++line;
    }
    // now we are in new package name
  }
}
