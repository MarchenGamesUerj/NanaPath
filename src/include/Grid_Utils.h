#ifndef GRID_UTILS_H
#define GRID_UTILS_H
#include "Global.h"

int ** allocate_grid_int (int rows, int cols);
float ** allocate_grid_float (int rows, int cols);
void free_grid_float (float ** grid, int rows);
void free_grid_int (int ** grid, int rows);
void reset_grid_float (float ** grid, int rows, int cols);
void reset_grid_int (int ** grid, int rows, int cols);
void print_grid_float (float ** grid, int rows, int cols);
void print_grid_int (int ** grid, int rows, int cols);

#endif