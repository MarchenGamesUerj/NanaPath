#ifndef PLAYER_H
#define PLAYER_H

#include "Global.h"

enum DIRECTION_STATE{
	NONE,
	UP,
	DOWN,
	RIGHT,
	LEFT
};

enum MOV_STATE{
	IDLE,
	WALKING,
	RUNNING,
	SLEEPING,
	TAKING_DMG
};

enum RANK{
	ESCOTEIRO,
	AVENTUREIRO,
	PESQUISADOR
};

enum TYPE_MSG_PLAYER{
	PLAYER_RANK_UPDATE,
	PLAYER_DEATH,
	PLAYER_GAME_OVER,
	PLAYER_VICTORY
};

typedef struct PlayerMsg{
	enum TYPE_MSG_PLAYER id_msg;
} PlayerMsg;

typedef struct Player Player;

Player *create_player();

void update_player_position (Player *player, Point pos);
void update_player_coord (Player *player, enum Type_terrain terrain, enum DIRECTION_STATE direction, int rows, int cols);
void update_player_x (Player *player, int x);
void update_player_y (Player *player, int y);
void update_player_hp (Player *player, int amount);
void update_player_life (Player *player, int amount);
void update_player_rank (Player *player);
void update_player_score (Player *player, int amount);
void update_collected_artifacts(Player *player);
void update_collected_potions(Player *player);
void update_collected_treasures(Player *player);
void update_consumed_artifacts(Player *player);
void player_render(Player *player, SDL_Renderer *ren, int cam_x, int cam_y);
int get_player_x (Player *player);
int get_player_y (Player *player);
int get_player_width (Player *player);
int get_player_height (Player *player);
SDL_Rect get_player_rect (Player *player);
int get_player_score (Player *player);
int get_player_hp(Player *player);
int get_player_max_hp(Player *player);
char *get_rank_player_name (Player *player);
int get_player_extra_life(Player *player);
void destroy_player(Player *player);
	
#endif