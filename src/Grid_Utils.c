#include "include/Grid_Utils.h"

int ** allocate_grid_int (int rows, int cols) {
	int ** grid = (int **) malloc(sizeof(int*)*rows);
	for (int i=0; i<rows; i++)
		grid[i] = (int *) malloc(sizeof(int)*cols);
	return grid;
}

float ** allocate_grid_float (int rows, int cols) {
    float ** grid = (float **) malloc(sizeof(float*)*rows);
    for (int i = 0; i < rows; i++) {
        grid[i] = (float *) malloc(sizeof(float)*cols);
    }
	return grid;
}

void free_grid_float (float ** grid, int rows) {
    for (int i=0; i<rows; i++)
        free(grid[i]);
    free(grid);
}

void free_grid_int (int ** grid, int rows) {
    for (int i=0; i<rows; i++)
        free(grid[i]);
    free(grid);
}

void reset_grid_float (float ** grid, int rows, int cols) {
	for(int i=0; i<rows; i++){
		for (int j=0; j<cols; j++){
			grid[i][j] = 0;
		}
	}
}

void reset_grid_int (int ** grid, int rows, int cols) {
	for(int i=0; i<rows; i++){
		for (int j=0; j<cols; j++){
			grid[i][j] = 0;
		}
	}
}

void print_grid_float (float ** grid, int rows, int cols) {
    int i, j;
    for (i=0; i<rows; i++) {
        for (j=0; j<cols; j++) 
            printf ("%.2f ", grid[i][j]);
        printf ("\n");
    }
}

void print_grid_int (int ** grid, int rows, int cols) {
    int i, j;
    for (i=0; i<rows; i++) {
        for (j=0; j<cols; j++) 
            printf ("%d ", grid[i][j]);
        printf ("\n");
    }
}