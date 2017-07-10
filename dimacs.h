#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sodium.h"

struct matrix {
  int **weight_matrix;
  int n;
};
