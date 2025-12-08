#include "include/RW_Map_Gen.h"

static void RW_initialise_grid (int ** grid, int rows, int cols, int level) {
	int seed = rand();
	int x = seed % rows;
	int y = seed % cols;
	grid[x][y] = 0;
	
	int h[4] = {-1,1,0,0};
	int v[4] = {0,0,-1,1};

	int num;
    for (int i = 0; i < 15000; i++) {
		num = rand()%(100+1);
        if(num<25){
			x+=h[0];
			y+=v[0];
		}
		else if(num<50){
			x+=h[1];
			y+=v[1];
		}
		else if(num<75){
			x+=h[2];
			y+=v[2];
		}
		else{
			x+=h[3];
			y+=v[3];
		}
		
		if (x < 0) x = 0;
        if (x >= rows) x = rows - 1;
        if (y < 0) y = 0;
        if (y >= cols) y = cols - 1;
		
		grid[x][y] = 0; //chao
    }
}

static void fill_water (int ** grid, int rows, int cols) {
	for(int i=0; i<rows; i++){
		for (int j=0; j<cols; j++){
			grid[i][j] = 1;
		}
	}
}

static void side_islands (int **grid, int rows, int cols){
	int num_side_islands = rand() % 20;
	
	for (int i=0; i<num_side_islands; i++){
		int x = rand() % rows;
		int y = rand() % cols;
		grid[x][y] = 0;
	
		int h[4] = {-1,1,0,0};
		int v[4] = {0,0,-1,1};

		int num;
		for (int i = 0; i < 500; i++) {
			num = rand()%(100+1);
			if(num<25){
				x+=h[0];
				y+=v[0];
			}
			else if(num<50){
				x+=h[1];
				y+=v[1];
			}
			else if(num<75){
				x+=h[2];
				y+=v[2];
			}
			else{
				x+=h[3];
				y+=v[3];
			}
		
			if (x < 0) x = 0;
			if (x >= rows) x = rows - 1;
			if (y < 0) y = 0;
			if (y >= cols) y = cols - 1;
		
			grid[x][y] = 0; //chao
		}
	}
}

int ** RW_build_map (int rows, int cols, int level){
	int ** map = allocate_grid_int (rows, cols);
	fill_water(map,rows,cols);
	RW_initialise_grid(map, rows, cols, level);
	side_islands(map, rows, cols);
	return map;
}
