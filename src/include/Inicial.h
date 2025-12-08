#ifndef INICIAL_H
#define INICIAL_H

#include "Global.h"
#include "Effects.h"

enum State Inicial_next_state (SDL_Event *evt, SDL_Renderer *ren);
void Inicial_update(SDL_Renderer *ren);
void destroy_inicial_textures();

#endif