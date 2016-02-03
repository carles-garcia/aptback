#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct date {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};

enum action_type { INSTALL, REMOVE, UPGRADE, SEARCH, UNDEFINED }; // there is also purge and maybe others

struct arguments {
  struct date dat, until;
  enum action_type command;
  int installed;
  int upgraded;
  int removed;
  int yes;
};

struct package {
  char *name;
  char *arch;
  char *version;
  char *newversion; // Only for UPGRADE
  int automatic; // Only for INSTALL
};	

struct darray_pack {
  int size, capacity;
  struct package **array; //no the same as *array[]
};

struct action {
  // start date (end date is ignored when parsing, not useful. Also, start date of installing, not downloading). 
  struct date date; 
  char *command;
  struct darray_pack packages;
  enum action_type type;
};

struct darray {
  int size, capacity;
  struct action **array;
};

void init_darray(struct darray *d);

void darray_add(struct darray *d, struct action *obj);

struct action* darray_get(struct darray *d, int i);

void free_darray(struct darray *d);





void init_darray_pack(struct darray_pack *d);

void darray_pack_add(struct darray_pack *d, struct package *obj);

struct package* darray_pack_get(struct darray_pack *d, int i);

void free_darray_pack(struct darray_pack *d);


void free_action(struct action *actions);

void free_pack(struct package *pack);

void init_action(struct action *current);

void init_pack(struct package *pack);

void eperror(char *msg);