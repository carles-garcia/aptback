#pragma once

#include "log_parse.h"
#include <stdio.h>
#include "mem.h"

int selection(struct arguments args, struct action **actions, int num_act, struct action ***selected, int *total_packages);

int satisfies(struct arguments args, struct action *act);

int datecmp(struct date first, struct date second);