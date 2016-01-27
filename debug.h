#pragma once

#include <argp.h>
#include "log_parse.h"

void debug_args(struct arguments args);

void debug_actions(struct action ***actions, int num_act);
