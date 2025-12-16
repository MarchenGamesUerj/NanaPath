#include "include/Global.h"
#include "include/Effects.h"
#include "include/Inicial.h"

#define LOGO_WIDTH 500
#define LOGO_HEIGHT 200
#define BUTTON_WIDTH 250
#define BUTTON_HEIGHT 100

typedef struct{
	int init;
} InfoState;

typedef struct{
    SDL_Texture *background_inicial;
    SDL_Texture *button_start;
	SDL_Texture *logo_jogo;
} InicialTextures;

typedef struct{
	SDL_Rect title_rect;
	SDL_Rect inicio_rect;
	SDL_Rect back_rect;
} InicialLayout;

static InfoState info_state;
static InicialTextures inicial_textures;
static InicialLayout inicial_rects;

static void inicial_init (SDL_Renderer *ren);
static void load_inicial_textures(SDL_Renderer *ren);
static void place_rects (int button_width, int button_height, int logo_width, int logo_height);
static enum State inicial_handler(SDL_Event* evt, SDL_Renderer* ren);
static void inicial_render(SDL_Renderer *ren);

static void inicial_init (SDL_Renderer *ren){
	info_state.init = 1;
	load_inicial_textures(ren);
	place_rects(BUTTON_WIDTH,BUTTON_HEIGHT, LOGO_WIDTH, LOGO_HEIGHT);
}

static void load_inicial_textures(SDL_Renderer *ren){
	inicial_textures.background_inicial = IMG_LoadTexture(ren, "assets/img/inicial/background_inicial.jpg");
    assert(inicial_textures.background_inicial != NULL);
	inicial_textures.button_start = IMG_LoadTexture(ren, "assets/img/inicial/button_start.png");
    assert(inicial_textures.button_start != NULL);
	inicial_textures.logo_jogo = IMG_LoadTexture(ren, "assets/img/inicial/logo_jogo.png");
    assert(inicial_textures.button_start != NULL);
}

static void place_rects (int button_width, int button_height, int logo_width, int logo_height){
    inicial_rects.title_rect = (SDL_Rect){ (LARGURA_TELA - logo_width)/2, 100, logo_width, logo_height };
	inicial_rects.inicio_rect = (SDL_Rect){ (LARGURA_TELA - button_width)/2, 600, button_width, button_height };
	inicial_rects.back_rect = (SDL_Rect){0,0,LARGURA_TELA, ALTURA_TELA};
}

enum State Inicial_next_state (SDL_Event *evt, SDL_Renderer *ren){
	enum State new_state = inicial_handler(evt, ren);
	inicial_render(ren);
	return new_state;
}

static enum State inicial_handler(SDL_Event* evt, SDL_Renderer* ren){
	static int aux_effect;
	enum State new_state = STATE_INICIAL;
	
	if (info_state.init != 1){
		inicial_init(ren);
		aux_effect = 1;
	}
	
	if (evt->type == SDL_QUIT)
				new_state = STATE_QUIT;
			
	else if(evt->type == SDL_KEYDOWN){
			if (evt->key.keysym.sym == SDLK_SPACE) new_state = STATE_MENU;
	}
	return new_state;
}

static void inicial_render(SDL_Renderer *ren){
	SDL_RenderClear(ren);
    SDL_RenderCopy(ren, inicial_textures.background_inicial, NULL, &inicial_rects.back_rect);
	SDL_RenderCopy(ren, inicial_textures.button_start, NULL, &inicial_rects.inicio_rect);
	SDL_RenderCopy(ren, inicial_textures.logo_jogo, NULL, &inicial_rects.title_rect);
	SDL_RenderPresent(ren);
}

void Inicial_update(SDL_Renderer *ren){
	static int aux_effect;
	
	if (info_state.init != 1){
		inicial_init(ren);
		aux_effect = 1;
		Add_hover_effect(&inicial_rects.inicio_rect);
	}
	
	if (aux_effect) {
		Add_hover_effect(&inicial_rects.inicio_rect);
	}
	else {
		Remove_hover_effect(&inicial_rects.inicio_rect);
	}
	aux_effect = !aux_effect;
	
	inicial_render(ren);
}
	

void destroy_inicial_textures(){
	SDL_DestroyTexture(inicial_textures.background_inicial);
	SDL_DestroyTexture(inicial_textures.button_start);
	SDL_DestroyTexture(inicial_textures.logo_jogo);
}
