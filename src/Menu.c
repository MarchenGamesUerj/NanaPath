#include "include/Global.h"
#include "include/Effects.h"
#include "include/Menu.h"

#define BUTTON_WIDTH 250
#define BUTTON_HEIGHT 100
#define LOGO_WIDTH 500
#define LOGO_HEIGHT 200
#define NUM_BUTTONS 3

typedef struct{
	int init;
} InfoState;

typedef struct{
    SDL_Texture *background;
    SDL_Texture *logo_menu;
	SDL_Texture *button_play;
	SDL_Texture *button_options;
	SDL_Texture *button_quit;
} MenuTextures;

typedef struct{
	SDL_Rect buttons[NUM_BUTTONS];
	SDL_Rect background_rect;
	SDL_Rect back_rect;
	SDL_Rect logo_rect;
} MenuLayout;

static InfoState info_state;
static MenuTextures menu_textures;
static MenuLayout menu_rects;

static void menu_init (SDL_Renderer *ren);
static void load_menu_textures(SDL_Renderer *ren);
static void place_rects (int button_width, int button_height, int logo_width, int logo_height);
static enum State menu_handler(SDL_Event* evt, SDL_Renderer* ren);
static void menu_render(SDL_Renderer *ren);

static void menu_init (SDL_Renderer *ren){
	info_state.init = 1;
	load_menu_textures(ren);
	place_rects(BUTTON_WIDTH,BUTTON_HEIGHT, LOGO_WIDTH, LOGO_HEIGHT);
}

static void load_menu_textures(SDL_Renderer *ren){
	menu_textures.background = IMG_LoadTexture(ren, "assets/img/menu/background_menu.png");
    assert(menu_textures.background != NULL);
	menu_textures.logo_menu = IMG_LoadTexture(ren, "assets/img/menu/logo_menu.png");
    assert(menu_textures.logo_menu != NULL);
	menu_textures.button_play = IMG_LoadTexture(ren, "assets/img/menu/button_play.png");
    assert(menu_textures.button_play != NULL);
	menu_textures.button_options = IMG_LoadTexture(ren, "assets/img/menu/button_game_mode.png");
    assert(menu_textures.button_options != NULL);
	menu_textures.button_quit = IMG_LoadTexture(ren, "assets/img/menu/button_quit.png");
    assert(menu_textures.button_quit != NULL);
}

static void place_rects (int button_width, int button_height, int logo_width, int logo_height){
	int y0_button = 300, gap_button = 30;
    menu_rects.buttons[0] = (SDL_Rect){ (LARGURA_TELA - button_width)/2, y0_button, button_width, button_height };
    menu_rects.buttons[1] = (SDL_Rect){ (LARGURA_TELA - button_width)/2, y0_button + (button_height + gap_button), button_width, button_height };
    menu_rects.buttons[2] = (SDL_Rect){ (LARGURA_TELA - button_width)/2, y0_button + 2*(button_height + gap_button), button_width, button_height };
        
    menu_rects.back_rect = (SDL_Rect){0,0,LARGURA_TELA, ALTURA_TELA};
    menu_rects.logo_rect = (SDL_Rect){ (LARGURA_TELA - logo_width)/2, y0_button - 2*button_height - gap_button, logo_width, logo_height };
}

enum State Menu_next_state (SDL_Event *evt, SDL_Renderer *ren){
	enum State new_state = menu_handler(evt, ren);
	menu_render(ren);
	return new_state;
}

static enum State menu_handler(SDL_Event* evt, SDL_Renderer* ren) {
	static int i;
	enum State new_state = STATE_MENU;
	
	if (info_state.init != 1){
		menu_init(ren);
		i = 0;
		Add_hover_effect(&menu_rects.buttons[i]);
	}
    
	if (evt->type == SDL_QUIT)
		new_state = STATE_QUIT;
	
    else if (evt->type == SDL_KEYDOWN) {
        if (evt->key.keysym.sym == SDLK_a || evt->key.keysym.sym == SDLK_DOWN || evt->key.keysym.sym == SDLK_UP)
            Remove_hover_effect(&menu_rects.buttons[i]);
		if (evt->key.keysym.sym == SDLK_UP){
			i--;
			if (i<0) i=0;
			Add_hover_effect(&menu_rects.buttons[i]);
		}
			
		else if (evt->key.keysym.sym == SDLK_DOWN){
			i++;
			if (i == NUM_BUTTONS) i = 0;
			Add_hover_effect(&menu_rects.buttons[i]);
		}
								
		else if (evt->key.keysym.sym == SDLK_a){
			if (i == 0) new_state = STATE_PLAY;
			else if (i == 1) new_state = STATE_OPTIONS;
			else new_state = STATE_QUIT;
			Add_hover_effect(&menu_rects.buttons[i]); //retirar depois de pronto
		}
	}

    return new_state;
}

static void menu_render(SDL_Renderer *ren){
	SDL_RenderClear(ren);
    SDL_RenderCopy(ren, menu_textures.background, NULL, &menu_rects.back_rect);
    SDL_RenderCopy(ren, menu_textures.logo_menu, NULL, &menu_rects.logo_rect);
    SDL_RenderCopy(ren, menu_textures.button_play, NULL, &menu_rects.buttons[0]);
    SDL_RenderCopy(ren, menu_textures.button_options, NULL, &menu_rects.buttons[1]);
    SDL_RenderCopy(ren, menu_textures.button_quit, NULL, &menu_rects.buttons[2]);
	SDL_RenderPresent(ren);
}

void Menu_update(SDL_Renderer *ren){
	menu_render(ren);
}

void destroy_menu_textures(){
	SDL_DestroyTexture(menu_textures.background);
	SDL_DestroyTexture(menu_textures.logo_menu);
	SDL_DestroyTexture(menu_textures.button_play);
	SDL_DestroyTexture(menu_textures.button_options);
	SDL_DestroyTexture(menu_textures.button_quit);
}