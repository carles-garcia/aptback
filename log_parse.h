#pragma once

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h> 

#include "darray.h"
#include "debug.h"


void evaluate_line(char *line, struct action **current, struct darray *actions);

int starts_with(char *line, char *string);

void get_date(char *line, struct date *dat);

void get_command(char *line, struct action *current);

void get_packages(char *line, struct action *current);

