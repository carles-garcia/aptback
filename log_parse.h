#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct date {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

enum action_type { INSTALL, REMOVE, UPGRADE };

struct package {
  char *name;
  char *arch;
  char *version;
  char *newversion; // Only for UPGRADE
  int automatic; // Only for INSTALL
};	

struct action {
  struct date start_date;
  char *command;
  enum action_type type;
  struct package **packages;
  int num_pack;
  struct date end_date;
};


void evaluate_line(char *line, struct action **current, struct action ***actions, int *num_act);

int starts_with(char *line, char *string);

void get_date(char *line, struct date dat);

void get_command(char *line, struct action *current);

void get_packages(char *line, struct action *current);

void init_action(struct action *current);

void init_pack(struct package *pack);