#ifndef DIMACS_H
#define DIMACS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "sodium.h"

#define MIN_ARGS 6
/* #define DEBUG */
#define FREE(x) if (x != NULL ) { free(x); x = NULL; } else { \
                                  printf ("Free err\n"); exit (-1); }

struct entry {
  int x_index;
  int y_index;
  int weight;
};

struct available_entries {
  int counter;
  struct entry **harray;
};

enum mode {
  pst,
  add_edges,
  rem_edges
};

#endif
