#ifndef END_H
#define END_H

#include "Global.h"
#include "Effects.h"
#include "Font_Utils.h"
#include "Play.h"

enum internalState{
	STATE_BAD,
	STATE_HAPPY
};

extern SDL_Texture **tiny_table;

void endgame_init (SDL_Renderer *ren);
enum State Endgame_next_state (SDL_Event *evt, SDL_Renderer *ren);
void Endgame_update(SDL_Renderer *ren);
void endgame_display_stats (enum internalState new_state, int final_score, int collected_artifacts, int consumed_artifacts, int collected_potions, int visited_area);
void destroy_endgame_textures();
void endgame_quit();


#endif
