#include "include/Global.h"
#include "include/Menu.h"
#include "include/Inicial.h"
#include "include/Play.h"
#include "include/Minimap.h"
#include "include/Aux_.h"
#include "include/Options.h"
#include "include/Font_Utils.h"
#include "include/Inventory.h"

SDL_Texture **tiny_table;

int main(void){
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* win = SDL_CreateWindow("NanaPath",
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         LARGURA_TELA, ALTURA_TELA, SDL_WINDOW_SHOWN
                      );
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, 0);
	TTF_Init();
	TTF_Font *tiny = TTF_OpenFont("tiny.ttf", 30);
	assert(tiny != NULL);
	SDL_Color tiny_color = {255,242,61,255};
	tiny_table = font_load_table (ren, tiny, tiny_color);

    enum State state_ = STATE_INICIAL, next_state;
	Uint32 wait = TIMEOUT;
    while(1){
		SDL_Event evt;
        int isevt = AUX_WaitEventTimeoutCount(&evt, &wait);
		if (isevt){
			switch (state_){
				case STATE_INICIAL:		next_state = Inicial_next_state (&evt, ren);	break;
				case STATE_MENU:		next_state = Menu_next_state (&evt, ren);		break;
				case STATE_OPTIONS:		next_state = Options_next_state (&evt, ren);	break;
				case STATE_PLAY:		next_state = Play_next_state(&evt, ren);		break;
				case STATE_PAUSE:		//next_state = Pause_next_state(&evt, ren);		break;
				case STATE_MAP:			next_state = Map_next_state(&evt, ren);			break;
				case STATE_INVENTORY:	next_state = Invent_next_state(&evt, ren);		break;
			}
			if (next_state != state_)
				state_ = next_state;
		}
		else{
			switch (state_){
				case STATE_INICIAL:		Inicial_update (ren);	break;
				case STATE_MENU:		Menu_update (ren);		break;
				case STATE_OPTIONS:		Options_update (ren);	break;
				case STATE_PLAY:		Play_update(ren);		break;
				case STATE_PAUSE:		//next_state = Pause_update(ren);		break;
				case STATE_MAP:			Map_update(ren);			break;
				case STATE_INVENTORY:	Inventory_update(ren);		break;
			}
		}
		if (state_ == STATE_QUIT) break;
	}

	if (STATE_QUIT){ //temporario
		destroy_menu_textures();
		destroy_inicial_textures();
		Minimap_quit();
	}
	
	font_destroy_table(tiny_table);
	TTF_CloseFont(tiny);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

