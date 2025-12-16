#ifndef PLAY_H
#define PLAY_H

#include "Global.h"
#include "Options.h"
#include "Build_Map_Utils.h"
#include "Minimap.h"
#include "Find_Largest_Island.h"
#include "Item_Data_Load.h"
#include "Grid_Utils.h"
#include "Cam_Control.h"
#include "Player.h"
#include "Hud.h"
#include "Inventory.h"
#include "End_Game.h"
#include "Render_Map.h"

typedef struct InventMsg InventMsg;
typedef struct PlayerMsg PlayerMsg;

enum State Play_next_state(SDL_Event *evt, SDL_Renderer *ren);
void Play_update(SDL_Renderer *ren);
void Game_Update_Player (PlayerMsg *update_msg);
void Game_Update_Inventory (InventMsg *update_msg);
void play_quit();
void play_reset();

#endif