/*
Copyright (C) 2016  Carles Garcia Cabot  (github.com/carles-garcia)
This file is part of aptback, a tool to search, install, remove and upgrade 
packages logged by apt. Released under the GNU GPLv3 (see COPYING.txt)
*/
#include "argp_aux.h"

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

int parse_select(char *arg, struct arguments *arguments) {
  char *aux = arg;
  while (1) {
    int c = 0;
    while (isalpha(*aux)) {
      ++aux;
      ++c;
    }
    if (c != 0) {
      char *buffer = malloc((c+1) * sizeof(char));
      if (buffer == NULL) eperror("Failed to malloc at parse_select");
      int i;
      for (i = 0; i < c; ++i) buffer[i] = arg[i];
      buffer[i] = '\0';
      if (starts_with("installed", buffer)) arguments->installed = 1;
      else if (starts_with("removed", buffer)) arguments->removed = 1;
      else if (starts_with("upgraded", buffer)) arguments->upgraded = 1;
      else if (starts_with("purged", buffer)) arguments->purged = 1;
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
