#ifndef BUILD_MAP_UTILS_H
#define BUILD_MAP_UTILS_H

#include "Global.h"
#include "Generators.h"
#include "Find_Largest_Island.h"
#include "Grid_Utils.h"
#include "CA_Map_Gen.h"
#include "RW_Map_Gen.h"
#include "VN_Map_Gen.h"
#include "Render_Map.h"

enum Types_item {
	P_INICIAL = 10,
    P_EXIT,
    P_POTION,
    P_ARTIFACT,
	P_TREASURE,
	P_THREAT, //inimigo?
	P_ITENS //testes
};

enum MapType {
	M_CELLULAR_AUTOMATA,
	M_VALUE_NOISE,
	M_RANDOM_WALK
};

typedef struct SeedInfo SeedInfo;

SeedInfo *set_seeds(int game_mode, int max_levels);
int **build_map(int level, SeedInfo *seeds);
InfoLevel *level_init(int level, SeedInfo *seeds);
int get_level_has_treasure(int level, SeedInfo *seeds);
void free_level(InfoLevel *info);
void render_map (SDL_Renderer *ren, int **current_map, int cam_viewport_x0, int cam_viewport_y0, int tile_size_game, int hud_height, int rows, int cols);


	
#endif


		
		