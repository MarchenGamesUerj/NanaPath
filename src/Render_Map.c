#include "include/Render_Map.h"

#define NORTH 1
#define WEST  64
#define EAST  4
#define SOUTH 16

#define NORTH_WEST 128
#define NORTH_EAST 2
#define SOUTH_WEST 32
#define SOUTH_EAST 8

#define SHEET_TILE_SIZE 32
#define SHEET_COLS 8 

typedef struct {
	int init_flag;
	SDL_Texture *ground_texture;
	SDL_Texture *mud_texture;
	int tile_index_table[256];
} InfoMap;

static InfoMap info_map;

static int check_terrain (int **map, int x, int y, int type, int rows, int cols);
static int get_tile_value(int **map, int x, int y, int rows, int cols, int type);
static void table_init();
static SDL_Rect get_texture_rect (int sprite_index, int tile_width, int tile_height, int sprite_cols);
static void destroy_map_textures();
static void load_map_textures(SDL_Renderer *ren);
static int is_ground(int type);

void map_autotile_init(SDL_Renderer *ren){
	info_map.init_flag = 1;
	load_map_textures(ren);
	table_init();
}

void map_autotile_quit(){
	destroy_map_textures();
}

static void destroy_map_textures(){
	SDL_DestroyTexture(info_map.ground_texture);
	SDL_DestroyTexture(info_map.mud_texture);
}

static void load_map_textures(SDL_Renderer *ren){
	info_map.ground_texture = IMG_LoadTexture(ren, "assets/img/map/sprite-blob.jpg");
    assert(info_map.ground_texture != NULL);
	info_map.mud_texture = IMG_LoadTexture(ren, "assets/img/map/sprite-blob-lama.png");
    assert(info_map.mud_texture != NULL);
	
	SDL_SetTextureBlendMode(info_map.ground_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(info_map.mud_texture, SDL_BLENDMODE_BLEND);
}

static int check_terrain (int **map, int x, int y, int type, int rows, int cols){
    if (x < 0 || x >= cols || y < 0 || y >= rows) return 1;

    if (type == 0) {
        return (map[y][x] == 0 || map[y][x] == 2 || map[y][x] > 2);
    }

    if (type == 2) {
        return (map[y][x] == 2);
    }

    return (map[y][x] == type);
}


static int get_tile_value(int **map, int x, int y, int rows, int cols, int type) {
    int aux = 0;

    int n = check_terrain(map, x, y - 1, type, rows, cols);
    int w = check_terrain(map, x - 1, y, type, rows, cols);
    int e = check_terrain(map, x + 1, y, type, rows, cols);
    int s = check_terrain(map, x, y + 1, type, rows, cols);
	
    if (n) aux = aux | NORTH;
    if (w) aux = aux | WEST;
    if (e) aux = aux | EAST;
    if (s) aux = aux | SOUTH;

    if (n && w){
        if (check_terrain(map, x - 1, y - 1, type, rows, cols))
            aux |= NORTH_WEST;
    }

    if (n && e){
        if (check_terrain(map, x + 1, y - 1, type, rows, cols))
            aux = aux | NORTH_EAST;
    }

    if (s && w){
        if (check_terrain(map, x - 1, y + 1, type, rows, cols))
            aux = aux | SOUTH_WEST;
    }

    if (s && e){
        if (check_terrain(map, x + 1, y + 1, type, rows, cols))
            aux = aux | SOUTH_EAST;
    }

    return aux;
}


static void table_init(){
	for (int i=0; i< 256; i++){
		info_map.tile_index_table[i] = 0;
	}
	info_map.tile_index_table[0]  =	0; 
	info_map.tile_index_table[1]  = 1;
	info_map.tile_index_table[5]  = 2;
	info_map.tile_index_table[7]  = 3;
	info_map.tile_index_table[17] = 4;
	info_map.tile_index_table[21] = 5;
	info_map.tile_index_table[23] = 6;
	info_map.tile_index_table[29] = 7;
	
	info_map.tile_index_table[31] =	8;
	info_map.tile_index_table[85] = 9;
	info_map.tile_index_table[87] = 10;
	info_map.tile_index_table[95] = 11;
	info_map.tile_index_table[119] = 12;
	info_map.tile_index_table[127] = 13;
	info_map.tile_index_table[4] = 	14;
	info_map.tile_index_table[16] = 15;
	
	info_map.tile_index_table[64]  = 16;
	info_map.tile_index_table[20]  = 17;
	info_map.tile_index_table[80]  = 18;
	info_map.tile_index_table[65]  = 19;
	info_map.tile_index_table[28]  = 20;
	info_map.tile_index_table[112] = 21;
	info_map.tile_index_table[193] = 22;
	info_map.tile_index_table[68]  = 23;
	
	info_map.tile_index_table[84]  = 24;
	info_map.tile_index_table[81]  = 25;
	info_map.tile_index_table[69]  = 26;
	info_map.tile_index_table[92]  = 27;
	info_map.tile_index_table[113] = 28;
	info_map.tile_index_table[197] = 29;
	info_map.tile_index_table[116] = 30;
	info_map.tile_index_table[209] = 31;
	
	info_map.tile_index_table[71]  = 32;
	info_map.tile_index_table[124] = 33;
	info_map.tile_index_table[241] = 34;
	info_map.tile_index_table[199] = 35;
	info_map.tile_index_table[93]  = 36;
	info_map.tile_index_table[117] = 37;
	info_map.tile_index_table[213] = 38;
	info_map.tile_index_table[125] = 39;
	
	info_map.tile_index_table[245] = 40;
	info_map.tile_index_table[215] = 41;
	info_map.tile_index_table[221] = 42;
	info_map.tile_index_table[253] = 43;
	info_map.tile_index_table[247] = 44;
	info_map.tile_index_table[223] = 45;
	info_map.tile_index_table[255] = 46;
}
	
SDL_Rect get_texture_rect (int sprite_index, int tile_width, int tile_height, int sprite_cols){
	SDL_Rect aux;
	
	aux.w = tile_width;
	aux.h = tile_height;	
	aux.x = (sprite_index % sprite_cols) * tile_width;
	aux.y = (sprite_index / sprite_cols) * tile_height;

	return aux;
}

static int is_ground(int type){
	return (type == 0 || type > 2); //considerar as marcacoes de itens como chão!
}	

void autotiling_render_map (SDL_Renderer *ren, int **current_map, int cam_viewport_x0, int cam_viewport_y0, int tile_size_game, int hud_height, int rows, int cols){
	SDL_Rect tile = {0, 0, tile_size_game, tile_size_game};
	SDL_Rect tile_tex;

	int start_col = cam_viewport_x0 / tile_size_game;
	int start_row = cam_viewport_y0 / tile_size_game;
	int end_col = (cam_viewport_x0 + LARGURA_TELA) / tile_size_game + 2;
	int end_row = (cam_viewport_y0 + (ALTURA_TELA - hud_height)) / tile_size_game + 2;

	if (start_col < 0) start_col = 0;
	if (start_row < 0) start_row = 0;
	if (end_col > cols) end_col = cols;
	if (end_row > rows) end_row = rows;
		
	for (int i = start_row; i < end_row; i++) {
		for (int j = start_col; j < end_col; j++) {
			tile.x = j * tile_size_game - cam_viewport_x0;
			tile.y = i * tile_size_game - cam_viewport_y0;
			int type = current_map[i][j];
			
			if (type == 1){ //água
				tile_tex = get_texture_rect(0, SHEET_TILE_SIZE, SHEET_TILE_SIZE, SHEET_COLS);
				SDL_RenderCopy(ren, info_map.ground_texture, &tile_tex, &tile);
			}
			if (is_ground(type) || type == 2){
				int bit_value = get_tile_value(current_map,j,i,rows,cols,0);
				int sprite_id;
				if (bit_value == 0 && type == 0)
					sprite_id = 47;
				else sprite_id = info_map.tile_index_table[bit_value];
				tile_tex = get_texture_rect(sprite_id, SHEET_TILE_SIZE, SHEET_TILE_SIZE, SHEET_COLS);
				SDL_RenderCopy(ren, info_map.ground_texture, &tile_tex, &tile);
			}
			if (type == 2){ //lama
					int bit_value = get_tile_value(current_map,j,i,rows,cols,2);
					int sprite_id = info_map.tile_index_table[bit_value];
					tile_tex = get_texture_rect(sprite_id, SHEET_TILE_SIZE, SHEET_TILE_SIZE, SHEET_COLS);
					SDL_RenderCopy(ren, info_map.mud_texture, &tile_tex, &tile);
			}
		}
	}
}
//adaptacao para o minimap.c
void render_map_minimap(SDL_Renderer *ren, int **map, int rows, int cols, int largura_tela, int altura_tela) {
	SDL_Rect tile;
	SDL_Rect tile_tex;
	SDL_RenderClear(ren);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int x1 = (j * largura_tela) / cols;
            int x2 = ((j+1) * largura_tela) / cols;
            int y1 = (i * altura_tela) / rows;
            int y2 = ((i+1) * altura_tela) / rows;

            tile.x = x1;
            tile.y = y1;
            tile.w = x2 - x1;
            tile.h = y2 - y1;

            int type = map[i][j];
            if (type == 1) { 
                tile_tex = get_texture_rect(0, SHEET_TILE_SIZE, SHEET_TILE_SIZE, SHEET_COLS);
                SDL_RenderCopy(ren, info_map.ground_texture, &tile_tex, &tile);
            }
            
            if (is_ground(type) || type == 2) {
                int bit_value = get_tile_value(map, j, i, rows, cols, 0);
                int sprite_id;
                
                if (bit_value == 0 && type == 0)
                    sprite_id = 47;
                else 
                    sprite_id = info_map.tile_index_table[bit_value];
                
                tile_tex = get_texture_rect(sprite_id, SHEET_TILE_SIZE, SHEET_TILE_SIZE, SHEET_COLS);
                SDL_RenderCopy(ren, info_map.ground_texture, &tile_tex, &tile);
            }

            if (type == 2) { 
                int bit_value = get_tile_value(map, j, i, rows, cols, 2);
                int sprite_id = info_map.tile_index_table[bit_value];
                
                tile_tex = get_texture_rect(sprite_id, SHEET_TILE_SIZE, SHEET_TILE_SIZE, SHEET_COLS);
                SDL_RenderCopy(ren, info_map.mud_texture, &tile_tex, &tile);
            }
        }
    }
}