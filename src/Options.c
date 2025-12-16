#include "include/Options.h"

#define BUTTON_WIDTH 250
#define BUTTON_HEIGHT 100
#define LOGO_WIDTH 500
#define LOGO_HEIGHT 200
#define NUM_BUTTONS 3

typedef struct{
	int init;
	int game_mode;
} InfoState;

typedef struct{
    SDL_Texture *background;
    SDL_Texture *logo_options;
	SDL_Texture *button_random_mode;
	SDL_Texture *button_fixed_mode;
	SDL_Texture *button_back;
} OptionsTextures;

typedef struct{
	SDL_Rect buttons[NUM_BUTTONS];
	SDL_Rect background_rect;
	SDL_Rect back_rect;
	SDL_Rect logo_rect;
} OptionsLayout;

static InfoState info_state = { 0, 1 }; // 1 random(padrao), 0 fixed (se não inicializar, começa com zero!)
static OptionsTextures options_textures;
static OptionsLayout options_rects;

static void options_init (SDL_Renderer *ren);
static void load_options_textures(SDL_Renderer *ren);
static void place_rects (int button_width, int button_height, int logo_width, int logo_height);
static enum State options_handler(SDL_Event* evt, SDL_Renderer* ren);
static void options_render(SDL_Renderer *ren);

static void options_init (SDL_Renderer *ren){
	info_state.init = 1;
	load_options_textures(ren);
	place_rects(BUTTON_WIDTH,BUTTON_HEIGHT, LOGO_WIDTH, LOGO_HEIGHT);
}

static void load_options_textures(SDL_Renderer *ren){
	options_textures.background = IMG_LoadTexture(ren, "assets/img/game_mode/background_game_mode.jpg");
    assert(options_textures.background != NULL);
	options_textures.logo_options = IMG_LoadTexture(ren, "assets/img/game_mode/logo_game_mode.png");
    assert(options_textures.logo_options != NULL);
	options_textures.button_random_mode = IMG_LoadTexture(ren, "assets/img/game_mode/button_random.png");
    assert(options_textures.button_random_mode != NULL);
	options_textures.button_fixed_mode = IMG_LoadTexture(ren, "assets/img/game_mode/button_fixed.png");
    assert(options_textures.button_fixed_mode != NULL);
	options_textures.button_back = IMG_LoadTexture(ren, "assets/img/game_mode/button_back.png");
    assert(options_textures.button_back != NULL);
}

static void place_rects (int button_width, int button_height, int logo_width, int logo_height){
	int y0_button = 300, gap_button = 30;
    options_rects.buttons[0] = (SDL_Rect){ (LARGURA_TELA - button_width)/2, y0_button, button_width, button_height };
    options_rects.buttons[1] = (SDL_Rect){ (LARGURA_TELA - button_width)/2, y0_button + (button_height + gap_button), button_width, button_height };
    options_rects.buttons[2] = (SDL_Rect){ (LARGURA_TELA - button_width)/2, y0_button + 2*(button_height + gap_button), button_width, button_height };
        
    options_rects.back_rect = (SDL_Rect){0,0,LARGURA_TELA, ALTURA_TELA};
    options_rects.logo_rect = (SDL_Rect){ (LARGURA_TELA - logo_width)/2, y0_button - 2*button_height - gap_button, logo_width, logo_height };
}

int options_get_game_mode (){
	return info_state.game_mode;
}

enum State Options_next_state (SDL_Event *evt, SDL_Renderer *ren){
	enum State new_state = options_handler(evt, ren);
	options_render(ren);
	return new_state;
}

static enum State options_handler(SDL_Event* evt, SDL_Renderer* ren) {
	static int i;
	enum State new_state = STATE_OPTIONS;
	
	if (info_state.init != 1){
		options_init(ren);
		i = 0;
		Add_hover_effect(&options_rects.buttons[i]);
	}
    
	if (evt->type == SDL_QUIT)
		new_state = STATE_QUIT;
	
    else if (evt->type == SDL_KEYDOWN) {
        if (evt->key.keysym.sym == SDLK_a || evt->key.keysym.sym == SDLK_DOWN || evt->key.keysym.sym == SDLK_UP)
            Remove_hover_effect(&options_rects.buttons[i]);
		if (evt->key.keysym.sym == SDLK_UP){
			i--;
			if (i<0) i=0;
			Add_hover_effect(&options_rects.buttons[i]);
		}
			
		else if (evt->key.keysym.sym == SDLK_DOWN){
			i++;
			if (i == NUM_BUTTONS) i = 0;
			Add_hover_effect(&options_rects.buttons[i]);
		}
								
		else if (evt->key.keysym.sym == SDLK_a){
			if (i == 0){
				info_state.game_mode = 1;
				play_reset();
				new_state = STATE_OPTIONS;
			}
			else if (i == 1){
				info_state.game_mode = 0;
				play_reset();
				new_state = STATE_OPTIONS; //implementar a mudança do game_mode
			}
			else new_state = STATE_MENU;
			Add_hover_effect(&options_rects.buttons[i]); //retirar depois de pronto
		}
	}

    return new_state;
}

static void options_render(SDL_Renderer *ren){
	SDL_RenderClear(ren);
    SDL_RenderCopy(ren, options_textures.background, NULL, &options_rects.back_rect);
    SDL_RenderCopy(ren, options_textures.logo_options, NULL, &options_rects.logo_rect);
    SDL_RenderCopy(ren, options_textures.button_random_mode, NULL, &options_rects.buttons[0]);
    SDL_RenderCopy(ren, options_textures.button_fixed_mode, NULL, &options_rects.buttons[1]);
    SDL_RenderCopy(ren, options_textures.button_back, NULL, &options_rects.buttons[2]);
	SDL_RenderPresent(ren);
}

void Options_update(SDL_Renderer *ren){
	options_render(ren);
}

void destroy_options_textures(){
	SDL_DestroyTexture(options_textures.background);
	SDL_DestroyTexture(options_textures.logo_options);
	SDL_DestroyTexture(options_textures.button_random_mode);
	SDL_DestroyTexture(options_textures.button_fixed_mode);
	SDL_DestroyTexture(options_textures.button_back);
}


