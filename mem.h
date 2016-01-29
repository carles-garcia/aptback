#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "log_parse.h"

void free_action(struct action *actions);

void eperror(char *msg);