#pragma once

#include "log_parse.h"
#include <stdio.h>

int selection(struct arguments args, struct action **actions, int num_act, struct action ***selected);

int satisfies(struct arguments args, struct action *act);

int datecmp(struct date first, struct date second);