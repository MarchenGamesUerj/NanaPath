#include "include/Minimap.h"

#define NUM_DIV_X 10
#define NUM_DIV_Y 10

typedef struct{
    SDL_Rect player_rect;
    SDL_Rect exit_rect;
    SDL_Rect treasure_rect;
} VisibleObjects;

typedef struct{
    int init;
	int counter_area;

    int w_rect;
    int h_rect;
    int ***control_minimap;

    int current_level;
    int max_levels;
    int current_rows, current_cols;
    int **current_level_map;
    VisibleObjects obj_list;
} InfoState;

typedef struct{
    SDL_Texture *player_texture;
	SDL_Texture *exit_texture;
	SDL_Texture *treasure_texture;
	SDL_Texture *rect_texture;
} MinimapTextures;

static InfoState info_state;
static MinimapTextures minimap_textures;
static int aux_apresentacao = 255;

static enum State minimap_handler(SDL_Event* evt, SDL_Renderer* ren);
static void minimap_load_objects (Point exit, Point treasure);
static void minimap_set_objects();
static void minimap_render(SDL_Renderer *ren);
static void load_minimap_textures(SDL_Renderer *ren);

void minimap_init (int max_levels, SDL_Renderer *ren){
    info_state.init = 1;
	info_state.counter_area = 0;
    info_state.max_levels = max_levels;
    info_state.control_minimap = malloc(info_state.max_levels * sizeof(int **));
    for (int i = 0; i < info_state.max_levels; i++){
        info_state.control_minimap[i] = allocate_grid_int (NUM_DIV_Y, NUM_DIV_X);
        reset_grid_int (info_state.control_minimap[i], NUM_DIV_Y, NUM_DIV_X);
    }

    info_state.obj_list.player_rect  = (SDL_Rect){0,0,0,0};
    info_state.obj_list.exit_rect    = (SDL_Rect){0,0,0,0};
    info_state.obj_list.treasure_rect= (SDL_Rect){0,0,0,0};
	load_minimap_textures(ren);
}

int update_minimap(int x, int y){
    if (x<0) x = 0;
    if (y<0) y = 0;

    info_state.obj_list.player_rect.x = (x * LARGURA_TELA) / info_state.current_cols;
    info_state.obj_list.player_rect.y = (y * ALTURA_TELA)  / info_state.current_rows;
    info_state.obj_list.player_rect.w = LARGURA_TELA / info_state.current_cols;
    info_state.obj_list.player_rect.h = ALTURA_TELA / info_state.current_rows;

    int j = (x * NUM_DIV_X) / info_state.current_cols;
    int i = (y * NUM_DIV_Y) / info_state.current_rows;
    int level = info_state.current_level;

    if (j >= NUM_DIV_X) j = NUM_DIV_X - 1;
    if (i >= NUM_DIV_Y) i = NUM_DIV_Y - 1;

    if (info_state.control_minimap[level][i][j] == 0){
        info_state.control_minimap[level][i][j] = 1;
		info_state.counter_area++;
		printf("Nova area descoberta. Contador: %d. \n", info_state.counter_area); //TESTES
        return 1;
    }
    return 0;
}

static void load_minimap_textures(SDL_Renderer *ren){
	minimap_textures.player_texture = IMG_LoadTexture(ren, "assets/img/minimap/player.png");
    assert(minimap_textures.player_texture != NULL);
	
	minimap_textures.exit_texture = IMG_LoadTexture(ren, "assets/img/minimap/door.png");
    assert(minimap_textures.exit_texture != NULL);
	
	minimap_textures.treasure_texture = IMG_LoadTexture(ren, "assets/img/minimap/treasure.png");
    assert(minimap_textures.treasure_texture != NULL);
	
	minimap_textures.rect_texture = IMG_LoadTexture(ren, "assets/img/minimap/minimap_wolf.png");
    assert(minimap_textures.rect_texture != NULL);
}

void destroy_minimap_textures(){
	SDL_DestroyTexture(minimap_textures.player_texture);
	SDL_DestroyTexture(minimap_textures.exit_texture);
	SDL_DestroyTexture(minimap_textures.treasure_texture);
	SDL_DestroyTexture(minimap_textures.rect_texture);
}

int minimap_get_area_counter (){
	return info_state.counter_area;
}

void minimap_load_level(int level, int **map, int rows, int cols, Point exit, Point treasure){
    info_state.current_rows = rows;
    info_state.current_cols = cols;

    info_state.current_level_map = map;
    info_state.current_level = level;

    minimap_set_objects();
    minimap_load_objects(exit, treasure);
}

static void minimap_load_objects(Point exit, Point treasure){

    if (exit.x < 0){
        info_state.obj_list.exit_rect.w = 0;
    } else {
        info_state.obj_list.exit_rect.x = (exit.x * LARGURA_TELA) / info_state.current_cols;
        info_state.obj_list.exit_rect.y = (exit.y * ALTURA_TELA)  / info_state.current_rows;
        info_state.obj_list.exit_rect.w = LARGURA_TELA / info_state.current_cols;
        info_state.obj_list.exit_rect.h = ALTURA_TELA / info_state.current_rows;
    }

    if (treasure.x < 0){
        info_state.obj_list.treasure_rect.w = 0;
    } else {
        info_state.obj_list.treasure_rect.x = (treasure.x * LARGURA_TELA) / info_state.current_cols;
        info_state.obj_list.treasure_rect.y = (treasure.y * ALTURA_TELA)  / info_state.current_rows;
        info_state.obj_list.treasure_rect.w = LARGURA_TELA / info_state.current_cols;
        info_state.obj_list.treasure_rect.h = ALTURA_TELA / info_state.current_rows;
    }
}

static void minimap_set_objects(){
    // placeholder
}

enum State Map_next_state (SDL_Event *evt, SDL_Renderer *ren){
    enum State new_state = minimap_handler(evt, ren);
    minimap_render(ren);
    return new_state;
}

static enum State minimap_handler(SDL_Event* evt, SDL_Renderer* ren) {
    enum State new_state = STATE_MAP;
      
    if (evt->type == SDL_QUIT)
        new_state = STATE_QUIT;

    else if (evt->type == SDL_KEYDOWN) {
        if (evt->key.keysym.sym == SDLK_d) new_state = STATE_PLAY;
		else if (evt->key.keysym.sym == SDLK_F5) aux_apresentacao = 150; //teste
		else if (evt->key.keysym.sym == SDLK_F6) aux_apresentacao = 255; //teste
    }
    return new_state;
}

static void minimap_render(SDL_Renderer *ren){
    int cols = info_state.current_cols;
    int rows = info_state.current_rows;
    int level = info_state.current_level;

    render_map_minimap(ren, info_state.current_level_map, rows, cols, LARGURA_TELA, ALTURA_TELA);

    if(info_state.obj_list.exit_rect.w>0){
        SDL_SetRenderDrawColor(ren,255,0,0,255);
        SDL_RenderFillRect(ren, &info_state.obj_list.exit_rect);
		//SDL_RenderCopy(ren, minimap_textures.exit_texture, NULL, &info_state.obj_list.exit_rect);
    }

    if(info_state.obj_list.treasure_rect.w>0){
        SDL_SetRenderDrawColor(ren,0,0,255,255);
        SDL_RenderFillRect(ren, &info_state.obj_list.treasure_rect);
		//SDL_RenderCopy(ren, minimap_textures.treasure_texture, NULL, &info_state.obj_list.treasure_rect);
    }
	
	//SDL_SetRenderDrawColor(ren, 255,255,255,255);
    //SDL_RenderFillRect(ren, &info_state.obj_list.player_rect);
	SDL_RenderCopy(ren, minimap_textures.player_texture, NULL, &info_state.obj_list.player_rect);

	SDL_Rect tile;
    for (int i = 0; i < NUM_DIV_Y; i++){
        for (int j = 0; j < NUM_DIV_X; j++){

            if (info_state.control_minimap[level][i][j] == 0){
                tile.x = j * LARGURA_TELA  / NUM_DIV_X;
                tile.y = i * ALTURA_TELA   / NUM_DIV_Y;
                tile.w = LARGURA_TELA  / NUM_DIV_X;
                tile.h = ALTURA_TELA   / NUM_DIV_Y;
                SDL_SetRenderDrawColor(ren, 0,0,0,aux_apresentacao);
                SDL_RenderFillRect(ren, &tile);
				//SDL_RenderCopy(ren, minimap_textures.rect_texture, NULL, &tile);
            }
        }
    }

    SDL_RenderPresent(ren);
}

void Map_update(SDL_Renderer *ren){
    minimap_render(ren);
}

void Minimap_quit(){
    for (int i = 0; i< info_state.max_levels; i++){
        free_grid_int (info_state.control_minimap[i], NUM_DIV_Y);
    }
	destroy_minimap_textures();
    free(info_state.control_minimap);
    info_state.control_minimap = NULL;
}
