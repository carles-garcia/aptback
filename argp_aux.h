/*
 * Auxiliar functions to parse arguments
 *
 */
#pragma once

#include <ctype.h>
#include <stdlib.h>

#include "log_parse.h"

int parse_date(char *arg, struct date *dat);

int parse_select(char *arg, struct arguments *arguments);