#ifndef MAP_H
#define MAP_H

#include "Global.h"

void map_autotile_init(SDL_Renderer *ren);
void autotiling_render_map (SDL_Renderer *ren, int **current_map, int cam_viewport_x0, int cam_viewport_y0, int tile_size_game, int hud_height, int rows, int cols);
void render_map_minimap(SDL_Renderer *ren, int **map, int rows, int cols, int largura_tela, int altura_tela);//usada pela Minimap.c
void map_autotile_quit();

#endif