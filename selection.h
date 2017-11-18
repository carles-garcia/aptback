/*
 * Functions to select actions based on arguments
 * Includes actioncmp function for qsort
 *
 */
#pragma once

#include <stdio.h>

#include "darray.h"

/*
 * stats: should be initialized
 */
void selection(const struct arguments *args, struct darray *actions, struct darray *selected, struct statistics *stats, int *user_len);

int satisfies(const struct arguments *args, struct action *act);

int datecmp(const struct date first, const struct date second);

int actioncmp(const void *a, const void *b);
