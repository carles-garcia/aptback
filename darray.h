/*
 * Type definitions and related functions.
 * Includes eperror function
 *
 */
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

enum action_type { INSTALL, REMOVE, UPGRADE, PURGE, SEARCH, UNDEFINED, ENUM_ACTION_SIZE };

struct statistics {
    unsigned int num_packages[ENUM_ACTION_SIZE];
    unsigned int num_selected;
    unsigned int num_total;
};

struct arguments {
    struct date dat, until;
    enum action_type option;
    int installed;
    int upgraded;
    int removed;
    int purged;
    int yes;
    int automatic;
    int manual;
    int exp;
    int version;
    int stats;
    int user;
};

struct darray {
    size_t size;
    size_t capacity;
    void **data;
    void (*free_f)(void*); // pointer to function that can free data content
};

struct package {
    char *name;
    char *arch;
    char *version;
    char *newversion; // Only for UPGRADE
    int automatic; // Only for INSTALL
};

struct action {
    // start date (end date is ignored when parsing, not useful. Also, start date of installing, not downloading).
    struct date date;
    char *command;
    struct darray packages;
    enum action_type type;
    char *user;
};




void init_darray(struct darray *d, void (*free_f)(void*));

void darray_add(struct darray *d, void *obj);

void free_darray(struct darray *d);



void free_action(void *actions);

void free_pack(void *pack);

void init_action(struct action *current);

void init_pack(struct package *pack);



void eperror(char *msg);
