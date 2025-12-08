#include "include/Hud.h"


typedef struct{
	SDL_Rect avatar_player;
	SDL_Rect life_bar;
	SDL_Rect level_number;
	SDL_Rect extra_life[5];
	SDL_Rect *str_rank;
	SDL_Rect *str_name;
	SDL_Rect *str_rank_atual;
} HudDataRects;

typedef struct{
	SDL_Rect hud_rect;
	SDL_Rect first_rect;
	SDL_Rect second_rect;
	SDL_Rect level_rect;
} LayoutRects;	

typedef struct{
	int qtd_xlife;
	int max_hp;
	char str_rank_name[12];
	char str_current_level[4];
} DisplayInfo;

struct HudInfo{
	int x0;
	int y0;
	int width;
	int height;
	int max_tam_life_bar;
	DisplayInfo display_data;
	LayoutRects hud_layout;
	HudDataRects hud_stats;
};

static const int gap_between = 10; //funciona como espacamento
static const int max_xlife_game = 5; //rects do display

static char str_player[] = "Player";
static char str_rank[] = "Rank:";

static void hud_init_camps(HudInfo *hud, int player_hp, char *current_rank_name, int player_max_hp, int current_level, int current_xlife);
static void hud_draw_rects(HudInfo *hud);
static void hud_update_life (HudInfo *hud, int hp);
static void hud_update_rank(HudInfo *hud, char *rank_stats, int new_max_hp);
static void hud_update_extra_life(HudInfo *hud, int number);
static void hud_data_rects (SDL_Renderer *ren, HudInfo *hud);

HudInfo *init_hud (int screen_width, int screen_height, int player_hp, char *current_rank_name, int player_max_hp, int current_level, int current_xlife){
	HudInfo *hud = malloc (sizeof (HudInfo));
	
	hud->width = screen_width;
	hud->height = screen_height/8;
	hud->x0 = 0;
	hud->y0 = screen_height - hud->height;
	
	hud_init_camps(hud, player_hp, current_rank_name, player_max_hp, current_level, current_xlife);
	hud_draw_rects(hud);
	
	return hud;

}

static void hud_init_camps(HudInfo *hud, int player_hp, char *current_rank_name, int player_max_hp, int current_level, int current_xlife){
	hud_update_rank(hud, current_rank_name, player_max_hp);
	hud_update_life (hud, player_hp);
	hud_update_level (hud, current_level);
	hud_update_extra_life(hud, current_xlife);
}

static void hud_draw_rects(HudInfo *hud){
	int levelboxwidth = 10*gap_between;
	hud->hud_layout.hud_rect = (SDL_Rect) { 0,
									hud->y0,
									hud->width,
									hud->height };
									
	hud->hud_layout.first_rect = (SDL_Rect) {gap_between,
									hud->y0 + gap_between,
									hud->width/2 - 2*gap_between - levelboxwidth/2,
									hud->height - 2*gap_between};
									
	hud->hud_layout.second_rect = (SDL_Rect) {(hud->width + levelboxwidth)/2 + gap_between,
									hud->y0 + gap_between,
									hud->width/2 - 2*gap_between - levelboxwidth/2,
									hud->height - 2*gap_between};
									
	hud->hud_layout.level_rect = (SDL_Rect) {(hud->width - levelboxwidth)/2,
									hud->y0 + gap_between,
									levelboxwidth,
									hud->height - 2*gap_between};
	
	hud->hud_stats.avatar_player = (SDL_Rect) { hud->hud_layout.first_rect.x + gap_between,
										hud->hud_layout.first_rect.y + gap_between ,
										6*gap_between,
										6*gap_between };
										
	hud->max_tam_life_bar = hud->hud_layout.first_rect.w - hud->hud_stats.avatar_player.w - 5*gap_between;
	hud->hud_stats.life_bar = (SDL_Rect) { hud->hud_stats.avatar_player.x + hud->hud_stats.avatar_player.w + 2*gap_between,
									hud->hud_layout.first_rect.y + hud->hud_layout.first_rect.h - gap_between - 2*gap_between,
									hud->max_tam_life_bar,
									2*gap_between };
									
	
	hud->hud_stats.str_name = font_rect_txt (str_player, hud->hud_stats.avatar_player.x + hud->hud_stats.avatar_player.w + 2*gap_between,
											hud->hud_layout.first_rect.y + gap_between, 2*gap_between, 3*gap_between);
	
	hud->hud_stats.level_number = (SDL_Rect) { hud->hud_layout.level_rect.x + 3*gap_between,
									hud->hud_layout.level_rect.y + 1*gap_between,
									hud->hud_layout.level_rect.w - 6*gap_between,
									hud->hud_layout.level_rect.h - 2*gap_between };
	
	int square_x_life = 2*gap_between;
	int x0_x_life = hud->hud_layout.second_rect.x + gap_between;
	for(int i=0; i<max_xlife_game; i++)
		hud->hud_stats.extra_life[i] = (SDL_Rect) {x0_x_life + i*square_x_life,
											hud->hud_layout.second_rect.y + hud->hud_layout.second_rect.h - square_x_life - gap_between,
											square_x_life,
											square_x_life };

	hud->hud_stats.str_rank = font_rect_txt (str_rank, hud->hud_layout.second_rect.x + gap_between,
											hud->hud_layout.second_rect.y + gap_between, 2*gap_between, 3*gap_between);
											
	hud->hud_stats.str_rank_atual = font_rect_txt (hud->display_data.str_rank_name, hud->hud_stats.str_rank[0].x + hud->hud_stats.str_rank[0].w*strlen(str_rank),
											hud->hud_layout.second_rect.y + gap_between, 2*gap_between, 3*gap_between);
}

static void hud_update_life (HudInfo *hud, int hp){
	int new_life_bar_width = (hp*hud->max_tam_life_bar)/hud->display_data.max_hp;
	hud->hud_stats.life_bar.w = new_life_bar_width;
}

static void hud_update_rank(HudInfo *hud, char *rank_stats, int new_max_hp){
	strncpy(hud->display_data.str_rank_name, rank_stats, 11); //rever esse limite de 12 se der tempo
	hud->display_data.str_rank_name[11] = '\0';
	hud->display_data.max_hp = new_max_hp;
}

void hud_update_level (HudInfo *hud, int current_level) {
	snprintf(hud->display_data.str_current_level, 4, "%d", current_level);
}

static void hud_update_extra_life(HudInfo *hud, int number){
	hud->display_data.qtd_xlife = number;
}

void hud_update_info (HudInfo *hud, int player_hp, int player_xlife){
	hud_update_life (hud, player_hp);
	hud_update_extra_life(hud, player_xlife);
}

void hud_update_rank_player (HudInfo *hud, char *current_rank_name, int player_max_hp, int current_xlife){
	hud_update_rank(hud, current_rank_name, player_max_hp);
	hud_update_extra_life(hud, current_xlife);
}

void hud_render (SDL_Renderer *ren, HudInfo *hud){
	SDL_SetRenderDrawColor(ren, 79, 52, 235, 255);
	SDL_RenderFillRect(ren, &hud->hud_layout.hud_rect);
	
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderFillRect(ren, &hud->hud_layout.first_rect);
	SDL_RenderFillRect(ren, &hud->hud_layout.second_rect);
	SDL_RenderFillRect(ren, &hud->hud_layout.level_rect);
	
	SDL_SetRenderDrawColor(ren, 255,255,255,255);
	SDL_RenderFillRect(ren, &hud->hud_stats.avatar_player);
	SDL_RenderFillRect(ren, &hud->hud_stats.life_bar);
	font_render_txt (ren, hud->hud_stats.str_name, str_player, tiny_table);

	//SDL_RenderFillRect(ren, &hud->hud_stats.level_number);
	
	for(int i=0; i < hud->display_data.qtd_xlife; i++){
		SDL_RenderFillRect(ren, &hud->hud_stats.extra_life[i]);
	}
	font_render_txt (ren, hud->hud_stats.str_rank, str_rank, tiny_table);
	font_render_txt (ren, hud->hud_stats.str_rank_atual, hud->display_data.str_rank_name, tiny_table);
	font_render_txt (ren, &hud->hud_stats.level_number, hud->display_data.str_current_level, tiny_table);
}

void hud_quit(HudInfo *hud){
	free(hud->hud_stats.str_rank_atual);
	free(hud->hud_stats.str_rank);
	free(hud->hud_stats.str_name);
	free(hud);
}
	

static void hud_data_rects (SDL_Renderer *ren, HudInfo *hud){
	SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
}