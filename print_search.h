/*
 * Functions to print search results
 * 
 */
#pragma once

#include <stdio.h>

#include "darray.h"

void print_search(struct darray *selected);

void print_export(struct darray *selected, int version);

void print_preview(struct darray *selected);
