/*
Copyright (C) 2016  Carles Garcia Cabot  (github.com/carles-garcia)
This file is part of aptback, a tool to search, install, remove and upgrade
packages logged by apt. Released under the GNU GPLv3 (see COPYING.txt)
*/
#include "selection.h"

void selection(const struct arguments *args, struct darray *actions, struct darray *selected, struct statistics *stats, int *user_len) {
    for (int i = 0; i < actions->size; ++i) {
        struct action *act = darray_get(actions, i);
        if (satisfies(args, act)) {
            darray_add(selected, act);
            int aux_size = act->packages.size;
            stats->num_selected += aux_size;
            stats->num_packages[act->type] += aux_size;
            if (act->user) {
                int aux_len = strlen(act->user);
                *user_len = aux_len > *user_len ? aux_len : *user_len;
            }
        }
    }
}

int satisfies(const struct arguments *args, struct action *act) {
    if ((args->installed && act->type == INSTALL)
            || (args->removed && act->type == REMOVE)
            || (args->upgraded && act->type == UPGRADE)
            || (args->purged && act->type == PURGE)) { //if UNDEFINED will return 0
        if (args->until.year != -1) {
            if (datecmp(args->dat, act->date) <= 0 && datecmp(args->until, act->date) >= 0) return 1;
            else return 0;
        }
        else { //no range, only date
            if (datecmp(args->dat, act->date) == 0) return 1;
            else return 0;
        }
    }
    return 0;
}

int datecmp(const struct date first, const struct date second) {
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

int actioncmp(const void *a, const void *b) {
    const struct action *act1 = *(const struct action **)a;
    const struct action *act2 = *(const struct action **)b;
    return datecmp(act1->date, act2->date);
}

