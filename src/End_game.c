#include "include/End_Game.h"

#define INFO_RECT 350 
#define RESTART_RECT_W 450
#define RESTART_RECT_H 125
#define LOGO_RECT_W 600
#define LOGO_RECT_H 250

typedef struct{
    SDL_Texture *background;
	SDL_Texture *info;
    SDL_Texture *logo;
	SDL_Texture *button_restart;
} EndGameTextures;

typedef struct{
	SDL_Rect background_rect;
	SDL_Rect info_rect;
	SDL_Rect restart_rect;
	SDL_Rect logo_rect;
} EndGameLayout;

typedef struct{
	SDL_Rect *str_title;
	SDL_Rect *str_final_score;
	SDL_Rect *str_collected_artifacts;
	SDL_Rect *str_consumed_artifacts;
	SDL_Rect *str_collected_potions;
	SDL_Rect *str_visited_area;
	
	SDL_Rect *str_data_final_score;
	SDL_Rect *str_data_collected_artifacts;
	SDL_Rect *str_data_consumed_artifacts;
	SDL_Rect *str_data_collected_potions;
	SDL_Rect *str_data_visited_area;
} ScoreStrLayout;

typedef struct{
	char str_info_final_score[5];
	char str_info_collected_artifacts[5];
	char str_info_consumed_artifacts[5];
	char str_info_collected_potions[5];
	char str_info_visited_area[5];
} ScoreInfo;

typedef struct {
	int init_flag;
	EndGameTextures happy_ending_textures;
	EndGameTextures bad_ending_textures;
	EndGameLayout end_game_rects;
	ScoreStrLayout str_rects;
	ScoreInfo stats_info;
	enum internalState internal_state;
} InfoState;

typedef struct{
	int width;
	int height;
} TextStyle;

static const int gap_between = 10; //espacamento
static const TextStyle title = { .width = 2*gap_between, .height = 3*gap_between };
static const TextStyle others = { .width = gap_between, .height = 15 }; //profecia: esses 2/3 vao dar problema de truncamento em algum momento (deram!!!!!)

InfoState info_state;

static void draw_rects (int info_size, int logo_width, int logo_height, int restart_width, int restart_height);
static void draw_rects_str();
static enum State endgame_handler(SDL_Event* evt, SDL_Renderer* ren);
static void endgame_render(SDL_Renderer *ren);
static void load_bad_textures(SDL_Renderer *ren);
static void load_happy_textures(SDL_Renderer *ren);
static void destroy_bad_textures();
static void destroy_happy_textures();

void endgame_init (SDL_Renderer *ren){
	if(info_state.init_flag) return;
	
	info_state.init_flag = 1;
	info_state.internal_state = STATE_HAPPY; //tirar depois
	
	load_bad_textures(ren);
	load_happy_textures(ren);
	draw_rects(INFO_RECT, LOGO_RECT_W, LOGO_RECT_H, RESTART_RECT_W, RESTART_RECT_H);
	draw_rects_str();
}

static void draw_rects (int info_size, int logo_width, int logo_height, int restart_width, int restart_height){
    info_state.end_game_rects.background_rect = (SDL_Rect){0,0,LARGURA_TELA, ALTURA_TELA};
	info_state.end_game_rects.info_rect = (SDL_Rect){ (LARGURA_TELA - info_size)/2, (ALTURA_TELA - info_size)/2 + 50, info_size, info_size };
	int info_y0 = info_state.end_game_rects.info_rect.y;
	int info_h = info_state.end_game_rects.info_rect.h;
	info_state.end_game_rects.restart_rect = (SDL_Rect){(LARGURA_TELA - restart_width)/2, (info_y0 + info_h + 2*gap_between), restart_width, restart_height};
	info_state.end_game_rects.logo_rect = (SDL_Rect){(LARGURA_TELA - logo_width)/2, (info_y0 - 2*gap_between - logo_height), logo_width, logo_height};
}

static void draw_rects_str(){
	SDL_Rect base = info_state.end_game_rects.info_rect;
	info_state.str_rects.str_title = font_rect_txt("SCORE BOARD", base.x + (base.w - title.width*strlen("SCORE BOARD"))/2,
													base.y + 7*gap_between,
													title.width, title.height);
	
	int y_base = info_state.str_rects.str_title[0].y + info_state.str_rects.str_title[0].h;
	info_state.str_rects.str_final_score = font_rect_txt("PONTUACAO FINAL: ", base.x + 5*gap_between,
													y_base + 4*gap_between,
													others.width, others.height);
													
	info_state.str_rects.str_data_final_score = font_rect_txt(info_state.stats_info.str_info_final_score,
															info_state.str_rects.str_final_score[0].x + info_state.str_rects.str_final_score[0].w*strlen("PONTUACAO FINAL: "),
															y_base + 4*gap_between,
															others.width, others.height);
													
	y_base = info_state.str_rects.str_final_score[0].y + info_state.str_rects.str_final_score[0].h;
	info_state.str_rects.str_collected_artifacts = font_rect_txt("PRESENTES COLETADOS: ", base.x + 5*gap_between,
													y_base + gap_between,
													others.width, others.height);
													
	info_state.str_rects.str_data_collected_artifacts = font_rect_txt(info_state.stats_info.str_info_collected_artifacts,
															info_state.str_rects.str_collected_artifacts[0].x + info_state.str_rects.str_collected_artifacts[0].w*strlen("PRESENTES COLETADOS: "),
															y_base + gap_between,
															others.width, others.height);
	
	y_base = info_state.str_rects.str_collected_artifacts[0].y + info_state.str_rects.str_collected_artifacts[0].h;
	info_state.str_rects.str_consumed_artifacts = font_rect_txt("PRESENTES USADOS: ", base.x + 5*gap_between,
													y_base + gap_between,
													others.width, others.height);
													
	info_state.str_rects.str_data_consumed_artifacts = font_rect_txt(info_state.stats_info.str_info_consumed_artifacts,
															info_state.str_rects.str_consumed_artifacts[0].x + info_state.str_rects.str_consumed_artifacts[0].w*strlen("PRESENTES USADOS: "),
															y_base + gap_between,
															others.width, others.height);
													
	y_base = info_state.str_rects.str_consumed_artifacts[0].y + info_state.str_rects.str_consumed_artifacts[0].h;
	info_state.str_rects.str_collected_potions = font_rect_txt("POCOES COLETADAS: ", base.x + 5*gap_between,
													y_base + gap_between,
													others.width, others.height);
													
	info_state.str_rects.str_data_collected_potions = font_rect_txt(info_state.stats_info.str_info_collected_potions,
															info_state.str_rects.str_collected_potions[0].x + info_state.str_rects.str_collected_potions[0].w*strlen("POCOES COLETADAS: "),
															y_base + gap_between,
															others.width, others.height);
													
	y_base = info_state.str_rects.str_collected_potions[0].y + info_state.str_rects.str_collected_potions[0].h;
	info_state.str_rects.str_visited_area = font_rect_txt("AREAS VISITADAS: ", base.x + 5*gap_between,
													y_base + gap_between,
													others.width, others.height);
													
	info_state.str_rects.str_data_visited_area = font_rect_txt(info_state.stats_info.str_info_visited_area,
															info_state.str_rects.str_visited_area[0].x + info_state.str_rects.str_visited_area[0].w*strlen("AREAS VISITADAS: "),
															y_base + gap_between,
															others.width, others.height);
}

enum State Endgame_next_state (SDL_Event *evt, SDL_Renderer *ren){
	enum State new_state = endgame_handler(evt, ren);
	endgame_render(ren);
	return new_state;
}

static enum State endgame_handler(SDL_Event* evt, SDL_Renderer* ren){
	enum State new_state = STATE_END;
	
	if (info_state.init_flag != 1){
		endgame_init(ren);
	}
	
	if (evt->type == SDL_QUIT)
				new_state = STATE_QUIT;
			
	else if(evt->type == SDL_KEYDOWN){
		if (evt->key.keysym.sym == SDLK_SPACE){
			play_reset();
			new_state = STATE_INICIAL;
		}
		else if (evt->key.keysym.sym == SDLK_F5) info_state.internal_state = STATE_BAD; //teste
		else if (evt->key.keysym.sym == SDLK_F6) info_state.internal_state = STATE_HAPPY; //teste
	}
	return new_state;
}

static void endgame_render(SDL_Renderer *ren){
	SDL_RenderClear(ren);
	if (info_state.internal_state == STATE_HAPPY){
		SDL_RenderCopy(ren, info_state.happy_ending_textures.background, NULL, &info_state.end_game_rects.background_rect);
		SDL_RenderCopy(ren, info_state.happy_ending_textures.info, NULL, &info_state.end_game_rects.info_rect);
		SDL_SetTextureBlendMode(info_state.happy_ending_textures.info, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(info_state.happy_ending_textures.info, 200);
		SDL_RenderCopy(ren, info_state.happy_ending_textures.button_restart, NULL, &info_state.end_game_rects.restart_rect);
		SDL_SetTextureBlendMode(info_state.happy_ending_textures.button_restart, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(info_state.happy_ending_textures.button_restart, 200);
		SDL_RenderCopy(ren, info_state.happy_ending_textures.logo, NULL, &info_state.end_game_rects.logo_rect);
	}
	else if (info_state.internal_state == STATE_BAD){
		SDL_RenderCopy(ren, info_state.bad_ending_textures.background, NULL, &info_state.end_game_rects.background_rect);
		SDL_RenderCopy(ren, info_state.bad_ending_textures.info, NULL, &info_state.end_game_rects.info_rect);
		SDL_SetTextureBlendMode(info_state.bad_ending_textures.info, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(info_state.bad_ending_textures.info, 200);
		SDL_RenderCopy(ren, info_state.bad_ending_textures.button_restart, NULL, &info_state.end_game_rects.restart_rect);
		SDL_SetTextureBlendMode(info_state.bad_ending_textures.button_restart, SDL_BLENDMODE_BLEND);
		SDL_SetTextureAlphaMod(info_state.bad_ending_textures.button_restart, 200);
		SDL_RenderCopy(ren, info_state.bad_ending_textures.logo, NULL, &info_state.end_game_rects.logo_rect);
	}
	
	font_render_txt (ren, info_state.str_rects.str_title, "SCORE BOARD", tiny_table);
	font_render_txt (ren, info_state.str_rects.str_final_score, "PONTUACAO FINAL: ", tiny_table);
	font_render_txt (ren, info_state.str_rects.str_collected_artifacts, "PRESENTES COLETADOS: ", tiny_table);
	font_render_txt (ren, info_state.str_rects.str_consumed_artifacts, "PRESENTES USADOS: ", tiny_table);
	font_render_txt (ren, info_state.str_rects.str_collected_potions, "POCOES COLETADAS: ", tiny_table);
	font_render_txt (ren, info_state.str_rects.str_visited_area, "AREAS VISITADAS: ", tiny_table);
	
	font_render_txt (ren, info_state.str_rects.str_data_final_score, info_state.stats_info.str_info_final_score, tiny_table);
	font_render_txt (ren, info_state.str_rects.str_data_collected_artifacts, info_state.stats_info.str_info_collected_artifacts, tiny_table);
	font_render_txt (ren, info_state.str_rects.str_data_consumed_artifacts, info_state.stats_info.str_info_consumed_artifacts, tiny_table);
	font_render_txt (ren, info_state.str_rects.str_data_collected_potions, info_state.stats_info.str_info_collected_potions, tiny_table);
	font_render_txt (ren, info_state.str_rects.str_data_visited_area, info_state.stats_info.str_info_visited_area, tiny_table);
	
	SDL_RenderPresent(ren);
}

void Endgame_update(SDL_Renderer *ren){
	static int aux_effect;
	
	if (info_state.init_flag != 1){
		endgame_init(ren);
		aux_effect = 1;
		Add_hover_effect(&info_state.end_game_rects.restart_rect);
	}
	
	if (aux_effect) {
		Add_hover_effect(&info_state.end_game_rects.restart_rect);
	}
	else {
		Remove_hover_effect(&info_state.end_game_rects.restart_rect);
	}
	aux_effect = !aux_effect;
	
	endgame_render(ren);
}

void endgame_display_stats (enum internalState new_state, int final_score, int collected_artifacts, int consumed_artifacts, int collected_potions, int visited_area){
	info_state.internal_state = new_state;
	
	snprintf(info_state.stats_info.str_info_final_score, sizeof(info_state.stats_info.str_info_final_score), "%d", final_score);
	snprintf(info_state.stats_info.str_info_collected_artifacts, sizeof(info_state.stats_info.str_info_collected_artifacts), "%d", collected_artifacts);
	snprintf(info_state.stats_info.str_info_consumed_artifacts, sizeof(info_state.stats_info.str_info_consumed_artifacts), "%d", consumed_artifacts);
	snprintf(info_state.stats_info.str_info_collected_potions, sizeof(info_state.stats_info.str_info_collected_potions), "%d", collected_potions);
	snprintf(info_state.stats_info.str_info_visited_area, sizeof(info_state.stats_info.str_info_visited_area), "%d", visited_area);
}


static void load_bad_textures(SDL_Renderer *ren){
	if(info_state.bad_ending_textures.background) destroy_bad_textures();
	
	info_state.bad_ending_textures.background = IMG_LoadTexture(ren, "assets/img/end_game/bad_ending_background.jpg");
    assert(info_state.bad_ending_textures.background != NULL);
	info_state.bad_ending_textures.info = IMG_LoadTexture(ren, "assets/img/end_game/bad_ending_info.png");
    assert(info_state.bad_ending_textures.info != NULL);
	info_state.bad_ending_textures.logo = IMG_LoadTexture(ren, "assets/img/end_game/bad_ending_logo.png");
    assert(info_state.bad_ending_textures.logo != NULL);
	info_state.bad_ending_textures.button_restart = IMG_LoadTexture(ren, "assets/img/end_game/bad_ending_restart.png");
    assert(info_state.bad_ending_textures.button_restart != NULL);
}

static void load_happy_textures(SDL_Renderer *ren){
	if(info_state.happy_ending_textures.background) destroy_happy_textures();
	
	info_state.happy_ending_textures.background = IMG_LoadTexture(ren, "assets/img/end_game/happy_ending_background.jpg");
    assert(info_state.happy_ending_textures.background != NULL);
	info_state.happy_ending_textures.info = IMG_LoadTexture(ren, "assets/img/end_game/happy_ending_info.png");
    assert(info_state.happy_ending_textures.info != NULL);
	info_state.happy_ending_textures.logo = IMG_LoadTexture(ren, "assets/img/end_game/happy_ending_logo.png");
    assert(info_state.happy_ending_textures.logo != NULL);
	info_state.happy_ending_textures.button_restart = IMG_LoadTexture(ren, "assets/img/end_game/happy_ending_restart.png");
    assert(info_state.happy_ending_textures.button_restart != NULL);
}

static void destroy_bad_textures(){
	SDL_DestroyTexture(info_state.bad_ending_textures.background);
	SDL_DestroyTexture(info_state.bad_ending_textures.info);
	SDL_DestroyTexture(info_state.bad_ending_textures.logo);
	SDL_DestroyTexture(info_state.bad_ending_textures.button_restart);
}

static void destroy_happy_textures(){
	SDL_DestroyTexture(info_state.happy_ending_textures.background);
	SDL_DestroyTexture(info_state.happy_ending_textures.info);
	SDL_DestroyTexture(info_state.happy_ending_textures.logo);
	SDL_DestroyTexture(info_state.happy_ending_textures.button_restart);
}

void destroy_endgame_textures(){
	destroy_bad_textures();
	destroy_happy_textures();
}

void endgame_quit(){
	free(info_state.str_rects.str_title);
	free(info_state.str_rects.str_final_score);
}