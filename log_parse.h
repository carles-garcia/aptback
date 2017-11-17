/*
 * Functions to parse apt log file
 * Includes starts_with function
 * It also does package selection based on package attributes
 */
#pragma once

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "darray.h"
#include "debug.h"


void evaluate_line(char *line, struct action **current, struct darray *actions, const struct arguments *args);

int starts_with(char *line, char *string);

void get_date(char *line, struct date *dat);

void get_command(char *line, struct action *current);

void get_packages(char *line, struct action *current, const struct arguments *args);

