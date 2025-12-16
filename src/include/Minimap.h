#include "Global.h"
#include "Grid_Utils.h"
#include "Play.h"
#include "Render_Map.h"

extern int **current_level_map;

int update_minimap(int x, int y);
void minimap_load_level(int level, int **map, int rows, int cols, Point exit, Point treasure);
enum State Map_next_state (SDL_Event *evt, SDL_Renderer *ren);
void Map_update(SDL_Renderer *ren);
int minimap_get_area_counter ();
void destroy_minimap_textures();
void Minimap_quit();
void minimap_init (int max_levels, SDL_Renderer *ren);