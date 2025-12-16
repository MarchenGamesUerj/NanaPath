#include "include/Build_Map_Utils.h"

struct SeedInfo{
	enum MapType type;
	unsigned int seed;
	int num_rows, num_cols;
	int has_treasure;

	union {
		struct {
			int CA_Steps;
			float CA_Chance;
		} CA_Params;

		struct {
			int VN_octaves;
			float VN_persistence;
		} VN_Params;

		struct {
			int level;
		} RW_Params;
	};
};

//usar ao iniciar jogo ou trocar o modo de jogo
SeedInfo *set_seeds(int game_mode, int max_levels){
	SeedInfo *seeds = malloc(max_levels * sizeof(SeedInfo));
	unsigned int seed;
	srand(time(NULL));
	
	for (int i=0; i<max_levels; i++){
		if (game_mode == 0)
			seed = i*11+100;
		else
			seed = (i+1)*rand();  
		seeds[i].seed = seed;
	}
	srand(seeds[0].seed); //usa semente do zero para escolher level com tesouro
	rand();
	int pick_random_level = (rand()%3) + 1; //entre 1 e 3
	
	for (int i=0; i<max_levels; i++){
		srand(seeds[i].seed);
		rand();
		rand();
		rand();
		seeds[i].type = (enum MapType)rand() % 3;
		set_size_cols_rows (&seeds[i].num_rows, &seeds[i].num_cols, i);
		if (i==0) seeds[i].has_treasure = 0;
		else if (i == max_levels-1 || i == pick_random_level) seeds[i].has_treasure = 1;
		else seeds[i].has_treasure = 0;
		
		switch(seeds[i].type){
			case M_CELLULAR_AUTOMATA:
				seeds[i].CA_Params.CA_Steps = CA_STEPS;
				seeds[i].CA_Params.CA_Chance = CA_CHANCE;
				break;

			case M_VALUE_NOISE:
				seeds[i].VN_Params.VN_octaves = OCTAVES;
				seeds[i].VN_Params.VN_persistence = PERSISTENCE;
				break;
		
			case M_RANDOM_WALK:
				seeds[i].RW_Params.level = i;
				break;
		}
		printf("Level %d | Seed %u | Mapa de tipo %d | Tem seed: %d | Tam %d x %d \n", i, seeds[i].seed, seeds[i].type, seeds[i].has_treasure, seeds[i].num_rows, seeds[i].num_cols);
	}
	return seeds;
}

int **build_map(int level, SeedInfo *seeds){
	int **map;
	srand(seeds[level].seed);
	
	switch(seeds[level].type){
		case M_CELLULAR_AUTOMATA:
			map = CA_build_map (seeds[level].num_rows, seeds[level].num_cols, seeds[level].CA_Params.CA_Steps, seeds[level].CA_Params.CA_Chance);
			break;

		case M_VALUE_NOISE:
			map = VN_build_map (seeds[level].VN_Params.VN_octaves, seeds[level].VN_Params.VN_persistence, seeds[level].num_rows, seeds[level].num_cols);
			break;
		
		case M_RANDOM_WALK:
			map = RW_build_map (seeds[level].num_rows, seeds[level].num_cols, level);
			break;
	}
	return map;
}

InfoLevel *level_init(int level, SeedInfo *seeds){	
	InfoLevel *info_level = malloc(sizeof(InfoLevel));
	info_level->level = level;
	info_level->map = build_map(level, seeds);
	info_level->rows=seeds[level].num_rows;
	info_level->cols=seeds[level].num_cols;
	
	Element *region_list = NULL;
	int total_regions = 0;

	// Mapeia todas as regiões do mapa
	int **visited_map = mapping_the_map(info_level->map, &region_list, &total_regions, seeds[level].num_rows, seeds[level].num_cols);

	// Identifica a maior ilha
	int largest_island = find_largest_island(region_list);
	Element* largest = find_Element(&region_list, largest_island);

	// Aloca entrada/saída dentro da maior ilha
	info_level->entrada = allocate_item(largest->x0, largest->xf,
                            largest->y0, largest->yf,
                            info_level->map, visited_map,
							largest_island, P_INICIAL);
	
	// Aloca itens (P_ITENS é um tipo genérico para testes)
	Point item_pos;
	info_level->qtd_itens = how_many_itens (level, largest->tam);
	info_level->itens = malloc (info_level->qtd_itens * sizeof(ItemBlock));
	info_level->rects_itens = malloc (info_level->qtd_itens * sizeof(SDL_Rect));
	for (int i=0; i<info_level->qtd_itens; i++){
		enum Type_selection type = pick_random_item_type ();
		int id = pick_random_item (level, type);
		item_pos = allocate_item(largest->x0, largest->xf,
                            largest->y0, largest->yf,
                            info_level->map, visited_map,
							largest_island, P_ITENS);
		info_level->itens[i] = (ItemBlock){
			.type = type,
			.id = id,
			.pos_item = item_pos
		};
		info_level->rects_itens[i].w = TILE_SIZE_GAME;
		info_level->rects_itens[i].h = TILE_SIZE_GAME;
		info_level->rects_itens[i].x = item_pos.x * TILE_SIZE_GAME;
		info_level->rects_itens[i].y = item_pos.y * TILE_SIZE_GAME; 
		
	}
	
	info_level->saida = allocate_item(largest->x0, largest->xf,
							largest->y0, largest->yf,
                            info_level->map, visited_map,
							largest_island, P_EXIT);
	if (seeds[level].has_treasure == 1){
		info_level->tesouro = malloc (sizeof(Point));
		*(info_level->tesouro) = allocate_item(largest->x0, largest->xf,
								largest->y0, largest->yf,
								info_level->map, visited_map,
								largest_island, P_TREASURE);
	} else info_level->tesouro = NULL;
	
								
	
	free_grid_int(visited_map, seeds[level].num_rows);
	free_Element_list(region_list); //lista de regiões liberadas
	
	return info_level;
}

int get_level_has_treasure(int level, SeedInfo *seeds){
	return seeds[level].has_treasure;
}

void free_level(InfoLevel *info) {
    
	free_grid_int(info->map, info->rows);
	free(info->itens);
	free(info->rects_itens);
	free(info->tesouro);
	free(info);
	
}

void render_map (SDL_Renderer *ren, int **current_map, int cam_viewport_x0, int cam_viewport_y0, int tile_size_game, int hud_height, int rows, int cols){
	SDL_Rect tile = {0, 0, tile_size_game, tile_size_game};

	int start_col = cam_viewport_x0 / tile_size_game;
	int start_row = cam_viewport_y0 / tile_size_game;
	int end_col = (cam_viewport_x0 + LARGURA_TELA) / tile_size_game + 1;
	int end_row = (cam_viewport_y0 + (ALTURA_TELA - hud_height)) / tile_size_game + 1;

	if (start_col < 0) start_col = 0;
	if (start_row < 0) start_row = 0;
	if (end_col > cols) end_col = cols;
	if (end_row > rows) end_row = rows;
		
	for (int i = start_row; i < end_row; i++) {
		for (int j = start_col; j < end_col; j++) {
			tile.x = j * tile_size_game - cam_viewport_x0;
			tile.y = i * tile_size_game - cam_viewport_y0;
			int v = current_map[i][j];
			switch (v) {
				case 0: SDL_SetRenderDrawColor(ren, 168, 145, 50, 255); break; // chão
				case 1: SDL_SetRenderDrawColor(ren, 50, 121, 168, 255); break; // água
				case 2: SDL_SetRenderDrawColor(ren, 112, 77, 49, 255); break; // lama
				//case P_INICIAL: SDL_SetRenderDrawColor(ren, 0, 255, 0, 255); break;
				//case P_EXIT: SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); break;
				//case P_ITENS: SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); break;
				//case P_TREASURE: SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); break;
				//default: SDL_SetRenderDrawColor(ren, 80, 80, 80, 255); break;
			}

			SDL_RenderFillRect(ren, &tile);
		}
	}
}
	

	


		
		
