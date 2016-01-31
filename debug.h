#pragma once

#include <argp.h>
#include <stdlib.h>
#include <stdio.h>

#include "darray.h"

void debug_args(struct arguments args);

void debug_actions(struct action **actions, int num_act);
