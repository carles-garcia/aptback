#pragma once

#include <stdio.h>

#include "darray.h"


int selection(struct arguments args, struct darray *actions, struct darray *selected);

int satisfies(struct arguments args, struct action *act);

int datecmp(struct date first, struct date second);