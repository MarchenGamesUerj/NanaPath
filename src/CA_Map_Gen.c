#include "include/CA_Map_Gen.h"

static int adjacent_to (int ** grid, int rows, int cols, int i, int j) {
	int	k, l, count=0;
	
	int sk, ek;
    if (i > 0) {
        sk = i - 1;
    } else {
        sk = i;
    }

    if (i + 1 < rows) {
        ek = i + 1;
    } else {
        ek = i;
    }

    int sl, el;
    if (j > 0) {
        sl = j - 1;
    } else {
        sl = j;
    }

    if (j + 1 < cols) {
        el = j + 1;
    } else {
        el = j;
    }

	for (k=sk; k<=ek; k++)
		for (l=sl; l<=el; l++)
			count+=grid[k][l];
	count-=grid[i][j];
	
	return count;
}

static void play_rules (int ** grid, int ** new_grid, int rows, int cols, int deathLimit, int birthLimit) {
	int	i, j, a;
	for (i=0; i<rows; i++)
		for (j=0; j<cols; j++) {
			a = adjacent_to (grid, rows, cols, i, j);
            if (grid[i][j] == 1) {  
                // célula é parede
                if (a < deathLimit)
                    new_grid[i][j] = 0;   // vira chão se tem menos de zeroLimit paredes
                else
                    new_grid[i][j] = 1;   // permanece parede
            } else {  
                // célula é chão
                if (a >= birthLimit)
                    new_grid[i][j] = 1;   // vira parede se tem >= oneLimit paredes
                else
                    new_grid[i][j] = 0;   // permanece chão
            }
			if (grid[i][j] == 1 && a <= 1)
				new_grid[i][j] = 0;
		}
}

static void CA_initialise_grid (int ** grid, int rows, int cols, float chanceToStartAlive) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float r = (float)rand() / (float)RAND_MAX;
            if (r < chanceToStartAlive) {
                grid[i][j] = 1;
            } else {
                grid[i][j] = 0;
            }
        }
    }
}

static void CA_cookies_shape (int ** grid, int rows, int cols, int radius){
	for (int j = 0; j < cols; j++) {
		for (int i = 0; i < rows; i++) {
			if (sqrt(pow(i - rows * 0.5, 2) + pow(j - cols * 0.5, 2)) > radius) {
				grid[i][j] = 1;
			}
		}
	}
}

int ** CA_build_map (int rows, int cols, int steps, float chance){
	
	int ** prev = allocate_grid_int (rows, cols);
	CA_initialise_grid(prev, rows, cols, chance);
	int ** next = allocate_grid_int (rows, cols);
	int ** tmp;
	int i,j;
	
	int size_cookie;
	if(rows>cols) size_cookie=cols*0.45;
	else size_cookie=rows*0.45;
	CA_cookies_shape(prev, rows, cols, size_cookie);
	
	for (i=0; i<steps; i++) {
		play_rules (prev,next,rows,cols, 4, 5);
		tmp = next;
		next = prev;
		prev = tmp;
	}

	free_grid_int(next, rows);
	return prev;
}
