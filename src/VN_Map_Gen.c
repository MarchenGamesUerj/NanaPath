#include "include/VN_Map_Gen.h"

static void VN_initialise_grid (float ** grid, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j] = (float)rand() / RAND_MAX;
        }
    }
}

static float interpolate(float x0, float x1, float alpha){
   return (x0 * (1 - alpha) + alpha * x1);
}

static float ** GenerateSmoothNoise(float ** original, int octave, int rows, int cols){
    float ** smoothNoise = allocate_grid_float (rows, cols);
    int samplePeriod = 1 << octave;
    float sampleFrequency = 1.0f / samplePeriod; 
    
    for(int i=0;i<rows;i++) {
        int i0 = (i / samplePeriod) * samplePeriod;
        int i1 = (i0 + samplePeriod) % rows;
        float horizontal_blend = (i - i0) * sampleFrequency;

        for(int j=0;j<cols;j++) {
            int j0 = (j / samplePeriod) * samplePeriod;
            int j1 = (j0 + samplePeriod) % cols;
            float vertical_blend = (j - j0) * sampleFrequency;

            float top = interpolate(original[i0][j0], original[i1][j0], horizontal_blend);
            float bottom = interpolate(original[i0][j1], original[i1][j1], horizontal_blend);

            smoothNoise[i][j] = interpolate(top, bottom, vertical_blend);
        }
    }
	
    return smoothNoise;
}

static float **GenerateNewNoise(float ** baseNoise, int octaveCount, float persistence, int rows, int cols) {
    float ***smoothNoise = (float ***) malloc(octaveCount * sizeof(float **));

    for (int i = 0; i < octaveCount; i++)
        smoothNoise[i] = GenerateSmoothNoise(baseNoise, i, rows, cols);

    float **newNoise = allocate_grid_float(rows, cols); 
	reset_grid_float (newNoise, rows, cols); //zera matriz

    float amplitude = 1.0f;
    float totalAmplitude = 0.0f;

    for (int octave = octaveCount - 1; octave >= 0; octave--) {
        amplitude *= persistence;
        totalAmplitude += amplitude;

        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                newNoise[i][j] += smoothNoise[octave][i][j] * amplitude;

    }

    for (int i = 0; i < rows; i++) //Normaliza
        for (int j = 0; j < cols; j++)
            newNoise[i][j] /= totalAmplitude;

    for (int i = 0; i < octaveCount; i++)
        free_grid_float(smoothNoise[i], rows);
    
    free(smoothNoise); 
	
    return newNoise;
}

// converte a matriz float para int (compatibilidade)
static int ** classify_noise (float ** matriz, int rows, int cols){
    int ** final_matriz = allocate_grid_int (rows, cols);
    for (int i=0; i<rows; i++){
        for (int j=0; j<cols; j++){
            if(matriz[i][j] < 0.4) final_matriz[i][j] = 1;      // água
            else if(matriz[i][j] > 0.7) final_matriz[i][j] = 2; // lama
            else final_matriz[i][j] = 0;                        // chão
        }
    }
    return final_matriz;
}

static void VN_cookies_shape (float ** grid, int rows, int cols, int radius){
	for (int j = 0; j < cols; j++) {
		for (int i = 0; i < rows; i++) {
			if (sqrt(pow(i - rows * 0.5, 2) + pow(j - cols * 0.5, 2)) > radius) {
				grid[i][j] = 0;
			}
		}
	}
}

int ** VN_build_map (int octaveCount, float persistence, int rows, int cols){
    float** matriz = allocate_grid_float (rows, cols);
    VN_initialise_grid(matriz, rows, cols);
	int size_cookie;
	if(rows>cols) size_cookie=cols*0.45;
	else size_cookie=rows*0.45;
	//VN_cookies_shape(matriz, rows, cols, size_cookie);
    float ** vNoise = GenerateNewNoise(matriz, octaveCount, persistence, rows, cols);
    int ** final = classify_noise (vNoise, rows, cols);
    
    free_grid_float(matriz, rows);
    free_grid_float(vNoise, rows);
    
    return final;
}