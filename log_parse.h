#pragma once

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

enum action_type { INSTALL, REMOVE, UPGRADE, UNDEFINED };

struct arguments {
  struct date dat, until;
  enum action_type command;
  int installed;
  int upgraded;
  int removed;
};

struct package {
  char *name;
  char *arch;
  char *version;
  char *newversion; // Only for UPGRADE
  int automatic; // Only for INSTALL
};	

// modify to accept installed and upgraded and removed at the same time (and maybe other combinations)
struct action {
  struct date start_date;
  char *command;
  enum action_type type;
  struct package **packages;
  int num_pack;
  struct date end_date;
  /*
  int installed;
  int removed;
  int upgraded;*/
};


void evaluate_line(char *line, struct action **current, struct action ***actions, int *num_act);

int starts_with(char *line, char *string);

void get_date(char *line, struct date *dat);

void get_command(char *line, struct action *current);

void get_packages(char *line, struct action *current);

void init_action(struct action *current);

void init_pack(struct package *pack);