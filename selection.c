#include "selection.h"

int selection(struct arguments args, struct action **actions, int num_act, struct action ***selected) {
  int i, num_sel = 0;
  for (i = 0; i < num_act; ++i) {
    if (satisfies(args, actions[i])) {
      ++num_sel;
      *selected = realloc(*selected, num_sel * sizeof(struct action *));
      (*selected)[num_sel-1] = actions[i];
    }
    else free(actions[i]); //was allocated in evaluate from log_parse.c
  }
  return num_sel;
}
      
int satisfies(struct arguments args, struct action *act) {
  if ((args.installed && act->type == INSTALL)
    || (args.removed && act->type == REMOVE) 
    || (args.installed && act->type == INSTALL)) {
    if (args.until.year != -1) {
      if (datecmp(args.dat, act->start_date) <= 0 && datecmp(args.until, act->start_date) >= 0) return 1;
      else return 0;
    }
    else { //no range, only date
      if (datecmp(args.dat, act->start_date) == 0) return 1;
      else return 0;
    }
  }
  return 0;
}

int datecmp(struct date first, struct date second) {
  if (first.year < 0) fprintf(stderr, "datecmp error");
  if (first.year < second.year) return -1;
  else if (first.year > second.year) return 1;
  else {
    if (first.month == -1) return 0;
    if (first.month < second.month) return -1;
    else if (first.month > second.month) return 1;
    else {
      if (first.day == -1) return 0;
      if (first.day < second.day) return -1;
      else if (first.day > second.day) return 1;
      else {
	if (first.hour == -1) return 0;
	if (first.hour < second.hour) return -1;
	else if (first.hour > second.hour) return 1;
	else {
	  if (first.minute == -1) return 0;
	  if (first.minute < second.minute) return -1;
	  else if (first.minute > second.minute) return 1;
	  else {
	    if (first.second == -1) return 0;
	    if (first.second < second.second) return -1;
	    else if (first.second > second.second) return 1;
	    else return 0;
	  }
	}
      }
    }
  }
}
	