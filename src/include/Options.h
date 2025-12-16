#ifndef OPTIONS_H
#define OPTIONS_H

#include "Global.h"
#include "Effects.h"
#include "Play.h"

int options_get_game_mode ();
enum State Options_next_state (SDL_Event *evt, SDL_Renderer *ren);
void Options_update(SDL_Renderer *ren);
void destroy_options_textures();

#endif

