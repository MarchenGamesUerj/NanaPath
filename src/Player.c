#include "include/Player.h"
#include "include/Play.h" //notificar morte/gameover 

typedef struct{
	int score;
	int collected_artifacts_counter;
	int collected_potions_counter;
	int collected_treasures_counter;
	int consumed_artifacts_counter;
} PlayerStats;

struct Player{
	int hp;
	int extra_life;
	enum RANK rank;
	int qtd_items_inventory;
	PlayerStats stats;
	
	SDL_Rect position;
	enum DIRECTION_STATE dir;
	enum MOV_STATE mov;
};

typedef struct {
	char *rank_name;
	int min_score;
	int max_hp;
	int max_extra_life;
	int max_items_inventory;
} RankInfo;

static int check_xlife_limits(Player *player);
static void check_hp_limits(Player *player);
static void check_map_limits (Player *player, int rows, int cols);
static RankInfo get_rank_info (enum RANK rank);
static void set_new_rank(Player *player, enum RANK new_rank);
static enum RANK check_player_rank (int player_score);
static void set_RankInfo();

static RankInfo escoteiro_info, aventureiro_info, pesquisador_info;
static int init_flag = 0;

Player *create_player() {
	if (!init_flag) set_RankInfo();
	
	Player *player = malloc(sizeof(Player));
	
    *player = (Player){
        .hp = escoteiro_info.max_hp,
        .extra_life = escoteiro_info.max_extra_life,
        .rank = ESCOTEIRO,
		.qtd_items_inventory = 0,
		.stats = { 0,0,0,0,0 },
		.position = { 0,0, TILE_SIZE_GAME, TILE_SIZE_GAME },
        .dir = DOWN,
        .mov = IDLE
    };
    return player;
}

void update_player_position (Player *player, Point pos){
	player->position.x = pos.x * TILE_SIZE_GAME;
	player->position.y = pos.y * TILE_SIZE_GAME;
}

void update_player_coord (Player *player, enum Type_terrain terrain, enum DIRECTION_STATE direction, int rows, int cols){
	int v;
	
	if (terrain == TERRAIN_GROUND) v = 10;
	else if (terrain == TERRAIN_MUD) v = 5;
	else v = 0;
	
	if (direction == UP) update_player_y (player, -v);
	else if (direction == DOWN) update_player_y (player, v);
	else if (direction == RIGHT) update_player_x (player, v);
	else if (direction == LEFT) update_player_x (player, -v);
	
	check_map_limits (player, rows, cols);

}

static void check_map_limits (Player *player, int rows, int cols){
	if (player->position.x < 0) player->position.x = 0;
	if (player->position.x > ((cols-1) * TILE_SIZE_GAME)) player->position.x = (cols-1) * TILE_SIZE_GAME;
	if (player->position.y < 0) player->position.y = 0;
	if (player->position.y > ((rows-1) * TILE_SIZE_GAME)) player->position.y = (rows-1) * TILE_SIZE_GAME;
}

void update_player_x (Player *player, int x){
	player->position.x += x;
}

void update_player_y (Player *player, int y){
	player->position.y += y;
}

void update_player_hp (Player *player, int amount){
	player->hp += amount;
	check_hp_limits(player);
}



static int check_xlife_limits(Player *player){
	RankInfo rank_data = get_rank_info(player->rank);
	
	if (player->extra_life > rank_data.max_extra_life)		player->extra_life = rank_data.max_extra_life;
	if (player->extra_life < 0){
		player->extra_life = 0;
		PlayerMsg msg = { .id_msg = PLAYER_GAME_OVER };
		Game_Update_Player (&msg);
		return 0;
	}
	return 1;
}

static void check_hp_limits(Player *player){
	RankInfo rank_data = get_rank_info(player->rank);
	
	if (player->hp > rank_data.max_hp) player->hp = rank_data.max_hp;
	if (player->hp <= 0){
		PlayerMsg msg = { .id_msg = PLAYER_DEATH };
		Game_Update_Player (&msg);
		player->extra_life--;
		if (check_xlife_limits(player)) player->hp = rank_data.max_hp;
		else player->hp = 0; //acabou, n precisa resetar
	}
}

static RankInfo get_rank_info (enum RANK rank){
	if (rank == ESCOTEIRO) return escoteiro_info;
	else if (rank == AVENTUREIRO) return aventureiro_info;
	return pesquisador_info;
}
	

void update_player_life (Player *player, int amount){
	player->extra_life += amount;
	check_xlife_limits(player);
}

void update_player_rank (Player *player){
	enum RANK new_rank = check_player_rank (player->stats.score);
	if (player->rank != new_rank){
		set_new_rank(player, new_rank);
		PlayerMsg msg = { .id_msg = PLAYER_RANK_UPDATE };
		Game_Update_Player (&msg);
	}
}

void update_player_score (Player *player, int amount){
	player->stats.score += amount;
	update_player_rank(player);
}

void update_collected_artifacts(Player *player){
	player->stats.collected_artifacts_counter++;
}

void update_collected_potions(Player *player){
	player->stats.collected_potions_counter++;
}

void update_collected_treasures(Player *player){
	player->stats.collected_treasures_counter++;
	if (player->stats.collected_treasures_counter >= MAX_TREASURES - 1){
		PlayerMsg msg = { .id_msg = PLAYER_VICTORY };
		Game_Update_Player (&msg);
	}
}

static void check_player_victory(Player *player){
	RankInfo rank_data = get_rank_info(player->rank);
	
	if (player->hp > rank_data.max_hp) player->hp = rank_data.max_hp;
	if (player->hp <= 0){
		PlayerMsg msg = { .id_msg = PLAYER_DEATH };
		Game_Update_Player (&msg);
		player->extra_life--;
		if (check_xlife_limits(player)) player->hp = rank_data.max_hp;
		else player->hp = 0; //acabou, n precisa resetar
	}
}

void update_consumed_artifacts(Player *player){
	player->stats.consumed_artifacts_counter++;
}

int get_player_x (Player *player){
	return player->position.x;
}

int get_player_y (Player *player){
	return player->position.y;
}

int get_player_width (Player *player){
	return player->position.w;
}

int get_player_height (Player *player){
	return player->position.h;
}

SDL_Rect get_player_rect (Player *player){
	return player->position;
}

int get_player_score (Player *player){
	return player->stats.score;
}

void destroy_player(Player *player){
	free(player);
}

int get_player_hp(Player *player){
	return player->hp;
}

int get_player_max_hp(Player *player){
	RankInfo info = get_rank_info (player->rank);
	return info.max_hp;
}

char *get_rank_player_name (Player *player){
	RankInfo info = get_rank_info (player->rank);
	return info.rank_name;
}

int get_player_extra_life(Player *player){
	return player->extra_life;
}

static void set_new_rank(Player *player, enum RANK new_rank){
	player->rank = new_rank;
	
	switch(player->rank){
		case AVENTUREIRO:
			player->hp = aventureiro_info.max_hp;
			if (player->extra_life < aventureiro_info.max_extra_life) player->extra_life++;
			break;
			
		case PESQUISADOR:
			player->hp = pesquisador_info.max_hp;
			if (player->extra_life < pesquisador_info.max_extra_life) player->extra_life++;
			break;
	}
}

static enum RANK check_player_rank (int player_score){
	enum RANK rank = ESCOTEIRO;
	
	if (player_score >= aventureiro_info.min_score) rank = AVENTUREIRO;
	if (player_score >= pesquisador_info.min_score) rank = PESQUISADOR;
	
	return rank;
}

static void set_RankInfo(){
	escoteiro_info = (RankInfo){
		.rank_name = "Escoteiro",
		.min_score = 0,
		.max_hp = 17,
		.max_extra_life = 3,
		.max_items_inventory = 15
	};
	
	aventureiro_info = (RankInfo){
		.rank_name = "Escavador",
		.min_score = 50,
		.max_hp = 22,
		.max_extra_life = 4,
		.max_items_inventory = 20
	};
	
	pesquisador_info = (RankInfo){
		.rank_name = "Perito",
		.min_score = 100,
		.max_hp = 30,
		.max_extra_life = 5,
		.max_items_inventory = 25
	};
	
	init_flag = 1;
}

void player_render(Player *player, SDL_Renderer *ren, int cam_x, int cam_y){
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	
	SDL_Rect player_screen = {
		player->position.x - cam_x,
		player->position.y - cam_y,
		player->position.w,
		player->position.h
	};
	
	SDL_RenderFillRect(ren, &player_screen);
}	