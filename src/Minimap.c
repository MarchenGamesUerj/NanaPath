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
    //vazio
} MinimapTextures;

static InfoState info_state;
static MinimapTextures minimap_textures;

static enum State minimap_handler(SDL_Event* evt, SDL_Renderer* ren);
static void minimap_load_objects (Point exit, Point treasure);
static void minimap_set_objects();
static void minimap_render(SDL_Renderer *ren);
static void load_minimap_textures(SDL_Renderer *ren);

void minimap_init (int max_levels){
    info_state.init = 1;
    info_state.max_levels = max_levels;
    info_state.control_minimap = malloc(info_state.max_levels * sizeof(int **));
    for (int i = 0; i < info_state.max_levels; i++){
        info_state.control_minimap[i] = allocate_grid_int (NUM_DIV_Y, NUM_DIV_X);
        reset_grid_int (info_state.control_minimap[i], NUM_DIV_Y, NUM_DIV_X);
    }

    info_state.obj_list.player_rect  = (SDL_Rect){0,0,0,0};
    info_state.obj_list.exit_rect    = (SDL_Rect){0,0,0,0};
    info_state.obj_list.treasure_rect= (SDL_Rect){0,0,0,0};
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
        return 1;
    }
    return 0;
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
    }
    return new_state;
}

static void minimap_render(SDL_Renderer *ren){
    int cols = info_state.current_cols;
    int rows = info_state.current_rows;
    int level = info_state.current_level;

    SDL_Rect tile;
    SDL_RenderClear(ren);

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){

            int x1 = (j * LARGURA_TELA) / cols;
            int x2 = ((j+1) * LARGURA_TELA) / cols;
            int y1 = (i * ALTURA_TELA) / rows;
            int y2 = ((i+1) * ALTURA_TELA) / rows;

            tile.x = x1;
            tile.y = y1;
            tile.w = x2 - x1;
            tile.h = y2 - y1;

            int v = info_state.current_level_map[i][j];
            if (v == 0) SDL_SetRenderDrawColor(ren,168,145,50,255);
            else if (v == 2) SDL_SetRenderDrawColor(ren,112,77,49,255);
            else if (v == 1) SDL_SetRenderDrawColor(ren,50,121,168,255);
            //else SDL_SetRenderDrawColor(ren,168,145,50,255);

            SDL_RenderFillRect(ren, &tile);
        }
    }

    SDL_SetRenderDrawColor(ren, 255,255,255,255);
    SDL_RenderFillRect(ren, &info_state.obj_list.player_rect);

    if(info_state.obj_list.exit_rect.w>0){
        SDL_SetRenderDrawColor(ren,255,0,0,255);
        SDL_RenderFillRect(ren, &info_state.obj_list.exit_rect);
    }

    if(info_state.obj_list.treasure_rect.w>0){
        SDL_SetRenderDrawColor(ren,0,0,255,255);
        SDL_RenderFillRect(ren, &info_state.obj_list.treasure_rect);
    }

    for (int i = 0; i < NUM_DIV_Y; i++){
        for (int j = 0; j < NUM_DIV_X; j++){

            if (info_state.control_minimap[level][i][j] == 0){
                tile.x = j * LARGURA_TELA  / NUM_DIV_X;
                tile.y = i * ALTURA_TELA   / NUM_DIV_Y;
                tile.w = LARGURA_TELA  / NUM_DIV_X;
                tile.h = ALTURA_TELA   / NUM_DIV_Y;
                SDL_SetRenderDrawColor(ren, 0,0,0,150);
                SDL_RenderFillRect(ren, &tile);
            }
        }
    }

    SDL_RenderPresent(ren);
}

void Map_update(SDL_Renderer *ren){
    minimap_render(ren);
}

void destroy_minimap_textures(){ }

void Minimap_quit(){
    for (int i = 0; i< info_state.max_levels; i++){
        free_grid_int (info_state.control_minimap[i], NUM_DIV_Y);
    }
    free(info_state.control_minimap);
    info_state.control_minimap = NULL;
}
