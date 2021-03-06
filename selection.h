/*
 * Functions to select actions based on arguments
 * Includes actioncmp function for qsort
 * 
 */
#pragma once

#include <stdio.h>

#include "darray.h"


int selection(const struct arguments *args, struct darray *actions, struct darray *selected);

int satisfies(const struct arguments *args, struct action *act);

int datecmp(const struct date first, const struct date second);

int actioncmp(const void *a, const void *b);