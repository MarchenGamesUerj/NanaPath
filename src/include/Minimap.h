#include "Global.h"
#include "Grid_Utils.h"
#include "Play.h"

extern int **current_level_map;

int update_minimap(int x, int y);
void minimap_load_level(int level, int **map, int rows, int cols, Point exit, Point treasure);
enum State Map_next_state (SDL_Event *evt, SDL_Renderer *ren);
void Map_update(SDL_Renderer *ren);
void Minimap_quit();
void minimap_init(int max_levels);