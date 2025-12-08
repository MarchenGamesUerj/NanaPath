#ifndef HUD_H
#define HUD_H

#include "Global.h"
#include "Font_Utils.h"

typedef struct HudInfo HudInfo;

extern SDL_Texture **tiny_table;

HudInfo *init_hud (int screen_width, int screen_height, int player_hp, char *current_rank_name, int player_max_hp, int current_level, int current_xlife);
void hud_update_info (HudInfo *hud, int player_hp, int player_xlife);
void hud_update_level (HudInfo *hud, int current_level);
void hud_update_rank_player (HudInfo *hud, char *current_rank_name, int player_max_hp, int current_xlife);
void hud_render (SDL_Renderer *ren, HudInfo *hud);
void hud_quit(HudInfo *hud);

#endif