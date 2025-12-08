#ifndef MENU_H
#define MENU_H

#include "Global.h"
#include "Effects.h"

enum State Menu_next_state (SDL_Event *evt, SDL_Renderer *ren);
void Menu_update(SDL_Renderer *ren);
void destroy_menu_textures();

#endif
