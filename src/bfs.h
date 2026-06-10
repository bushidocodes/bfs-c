#pragma once
#include "globals.h"

void find_path(unsigned long start, unsigned long end, unsigned long parents[]);
void print_parents(graph *g);
