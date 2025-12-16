#include "include/Inventory.h"
#define MENU_ROWS 3
#define MENU_COLS 2

enum InternalState{
	ABRIR,
	DROPAR
};

typedef struct{
	SDL_Rect *str_open_mode; //ABRIR
	SDL_Rect *str_drop_mode; //DROPAR
	SDL_Rect *str_itens_menu[3][2]; 
} StrRects;

typedef struct{
	SDL_Rect extern_rect;
	SDL_Rect internal_rect;
	StrRects txt_rect;
	SDL_Rect matriz_menu_itens[3][2]; //placeholder, inicializar depois
} LayoutRects;

typedef struct{
	int i;
	int j;
	int i_array;
} NavInfo;

typedef struct {
	int init;	
	int max_tam;
	int tam_atual;
	int current_level_index;
	int empty_slots;
	int *array_itens;
	LayoutRects layout;
	NavInfo nav_indexes;
} InventState;

typedef struct{
	int width;
	int height;
} TextStyle;

static const int gap_between = 10;
static const char *str_unknown = "?????";
static const int MAX_LIMIT = 25; 
static const TextStyle title = { .width = 4*gap_between, .height = 6*gap_between };
static const TextStyle menu_style = { .width = 4*gap_between, .height = 6*gap_between };

static enum InternalState internal_state_ = ABRIR;
static InventState *invent; //1 inventário para o jogo inteiro
static int control_render[3][2];
static SDL_Texture *icon_menu;

static enum State inventory_evt_handler(SDL_Event* evt, SDL_Renderer* ren);
static void invent_draw_rects(int screen_width, int screen_height);
static int *invent_array_init(int tam);
static void invent_control_render_init ();
static void invent_send_msg (int id_item);
static void update_menu_itens (int di, int dj);
static int invent_get_id_pos (int pos);
static int invent_get_content_array (int logic_index);
static int invent_check_index(int logic_index);
static void select_content_menu (int select_x, int max);
static void inventory_render(SDL_Renderer *ren);
static void invent_load_texture(SDL_Renderer *ren);
static void invent_destroy_texture();

void invent_init (SDL_Renderer *ren, int max_tam, int tam_atual, int current_level_index, int screen_width, int screen_height){
	invent = malloc (sizeof(InventState));
	invent_load_texture(ren);
	
	invent->init = 1;
	invent->max_tam = max_tam;
	invent->tam_atual = tam_atual;
	invent->current_level_index = current_level_index;
	invent->empty_slots = tam_atual;
	invent->array_itens = invent_array_init(invent->max_tam);
	invent_draw_rects(screen_width, screen_height);
	invent->nav_indexes.i = 0;
	invent->nav_indexes.j = 0;
	invent->nav_indexes.i_array = 0;
	
	invent_control_render_init();
	Add_hover_effect(&invent->layout.matriz_menu_itens[0][0]);
	select_content_menu(0, invent->tam_atual);
	//update_menu_itens(0,0);
	
}

static int *invent_array_init(int tam){
	int *array = malloc(tam * sizeof(int));
	for (int i=0; i<tam; i++){
		array[i] = -1;
	}
	return array;
}

static void invent_control_render_init (){
	for (int i=0; i<MENU_ROWS; i++){
		for(int j=0; j<MENU_COLS;j++){
			control_render[i][j] = -1;
		}
	}
}

int invent_add_item (int id_item){
	if (invent->empty_slots == 0){
		printf("Inventario cheio!\n"); //substituir por algo visual
		return 0;
	}
	for (int i=0; i<invent->max_tam; i++){
		if (invent->array_itens[i] == -1){
			invent->array_itens[i] = id_item;
			invent->empty_slots--;
			printf("Item added ao inventario!\n"); //tirar depois
			update_menu_itens(0,0);
			return 1;
		}
	}
}


static void invent_send_msg (int id_item){
	InventMsg msg;
	
	switch (internal_state_){
		case ABRIR:
			msg = (InventMsg) { .id_msg = ITEM_OPEN, .id_item = id_item };
			break;
		case DROPAR:
			msg = (InventMsg) { .id_msg = ITEM_DROP, .id_item = id_item };
			break;
	}
	Game_Update_Inventory (&msg);
}

void invent_remove_item (int id_array){
	int real_pos = invent_get_id_pos (id_array);
	invent_send_msg(invent->array_itens[real_pos]);
	invent->array_itens[real_pos] = -1;
	invent->empty_slots++;
	
	Remove_hover_effect(&invent->layout.matriz_menu_itens[invent->nav_indexes.i][invent->nav_indexes.j]);
	invent->nav_indexes.i = 0;
	invent->nav_indexes.j = 0;
	invent->nav_indexes.i_array = 0;
	Add_hover_effect(&invent->layout.matriz_menu_itens[invent->nav_indexes.i][invent->nav_indexes.j]);
	select_content_menu(0, invent->tam_atual - invent->empty_slots);
}

void invent_update_rank(int new_tam){
	if (new_tam>invent->max_tam) return;
	int qtd_itens = invent->tam_atual - invent->empty_slots;
	invent->tam_atual = new_tam;
	invent->empty_slots = invent->tam_atual - qtd_itens;
	printf("Inventario aumentou de tamanho! Tamanho atual: %d \n", new_tam);
}

int has_empty_slots(){
	return (invent->empty_slots > 0);
}

enum State Invent_next_state (SDL_Event *evt, SDL_Renderer *ren){
	if (!invent) return STATE_PLAY;
	enum State new_state = inventory_evt_handler(evt, ren);
	
	inventory_render(ren);
	return new_state;
}

static enum State inventory_evt_handler(SDL_Event* evt, SDL_Renderer* ren) {
	enum State new_state = STATE_INVENTORY;
    
	if(!invent->init) printf("Erro ao iniciar o inventário.\n");
	
	if(evt->type == SDL_QUIT)
		new_state = STATE_QUIT;
	
	if(evt->type == SDL_KEYDOWN){
		switch(evt->key.keysym.sym){
			//eventos que causam mudança de estado
			case SDLK_s:
				internal_state_ = ABRIR; //sempre começar abrindo
				new_state = STATE_PLAY;
				break;
			
			case SDLK_d:
				internal_state_ = (internal_state_ == ABRIR ? DROPAR : ABRIR); //alterna entre ABRIR/DROPAR
				break;
	
			//eventos que afetam o jogo
			case SDLK_UP: update_menu_itens(-1, 0); break;
			case SDLK_DOWN: update_menu_itens(1, 0); break;
			case SDLK_LEFT: update_menu_itens(0, -1); break;
			case SDLK_RIGHT: update_menu_itens(0, 1); break;
		
			//o que acontece na selecao depende do estado interno
			case SDLK_a:
				invent_remove_item(invent->nav_indexes.i_array); //remocao eh a mesma para open/drop, mas a msg enviada eh diferente
			break;
		}
	}
	
    return new_state;
}

static int start_row = 0;
static void update_menu_itens (int di, int dj){
    Remove_hover_effect(&invent->layout.matriz_menu_itens[
        invent->nav_indexes.i][invent->nav_indexes.j]);
	
    int jump = di*2 + dj;
    int next_id = invent->nav_indexes.i_array + jump;

    int qtd_itens_validos = invent->tam_atual - invent->empty_slots;

    if (dj != 0) {
        if (invent->nav_indexes.j == 0 && dj < 0)
            next_id = invent->nav_indexes.i_array;
        if (invent->nav_indexes.j == 1 && dj > 0)
            next_id = invent->nav_indexes.i_array;
    }

    int moved = 0;

    if (next_id >= 0 && next_id < qtd_itens_validos) {
        invent->nav_indexes.i_array = next_id;
        moved = 1;

        select_content_menu(invent->nav_indexes.i_array,
                            qtd_itens_validos);

        int relative_index = invent->nav_indexes.i_array - start_row;
        if (relative_index < 0) relative_index = 0;

        invent->nav_indexes.i = relative_index / 2;
        invent->nav_indexes.j = relative_index % 2;
    }
	
	Add_hover_effect(&invent->layout.matriz_menu_itens[
        invent->nav_indexes.i][invent->nav_indexes.j]);
    
}

static int invent_get_id_pos (int logic_index){
	int counter=0;
	for (int i=0; i < invent->max_tam; i++){
		if (invent->array_itens[i] != -1){
			if (counter == logic_index) return i;	
			counter++;
		}
	}
	return -1;
}

static int invent_get_content_array (int logic_index){
	int counter=0;
	for (int i=0; i < invent->max_tam; i++){
		if (invent->array_itens[i] != -1){
			if (counter == logic_index) return invent->array_itens[i];	
			counter++;
		}
	}
	return -1;
}

// checa se menu deve exibir ????, o id_item ou nada
static int invent_check_index(int logic_index){
	int id_item = invent_get_content_array (logic_index);
	if (id_item == -1) return -1; //nao renderiza
	
	int flag_item = Item_get_flag_info (id_item);
	if (flag_item == 0) return -2; // renderiza conteúdo padrão
	else return id_item; // renderizar o conteúdo do id_item
}


static void select_content_menu (int select_index, int max){
    int itens_por_tela = MENU_ROWS * MENU_COLS;
    int fim_tela = start_row + itens_por_tela - 1;

    if (select_index < start_row) {
        start_row = select_index;
    } 
    else if (select_index > fim_tela) {
        start_row = select_index - (itens_por_tela - MENU_COLS);
    }

    if ((start_row % 2) != 0) {
        start_row--;
    }

    if (start_row < 0) start_row = 0;
    if (start_row >= max) start_row = max - 1;

    if ((start_row % 2) != 0) start_row--;

    for (int i=0; i<MENU_ROWS; i++){
        int idx1 = start_row + (i * 2);
        int idx2 = idx1 + 1;            

        control_render[i][0] = invent_check_index(idx1);
        control_render[i][1] = invent_check_index(idx2);
    }
}

static void invent_load_texture(SDL_Renderer *ren){
	icon_menu = IMG_LoadTexture(ren, "assets/img/invent/gift_icon.png");
    assert(icon_menu != NULL);
}
static void invent_destroy_texture(){
	SDL_DestroyTexture(icon_menu);
}
	
void invent_update_new_tam (int new_tam){
	if (new_tam > invent->max_tam){
		new_tam = invent->max_tam;
		printf ("ERRO: Limite do inventario nao ajustado.\n");
	} else{
		int qtd_itens = invent->tam_atual - invent->empty_slots;
		invent->tam_atual = new_tam;
		invent->empty_slots = invent->tam_atual - qtd_itens;
	}
}

static void invent_draw_rects(int screen_width, int screen_height){ //ficou bagunçado
	invent->layout.extern_rect = (SDL_Rect) { 0, 0, screen_width, 300 };
	invent->layout.internal_rect = (SDL_Rect) { gap_between, gap_between, screen_width - 2*gap_between, invent->layout.extern_rect.h - 2*gap_between };
	SDL_Rect *font_rect_txt (char *txt, int x, int y, int width, int height);
	invent->layout.txt_rect.str_open_mode = font_rect_txt("ABRIR", invent->layout.internal_rect.x + (invent->layout.internal_rect.w - title.width*strlen("ABRIR"))/2,
													invent->layout.internal_rect.y + 2*gap_between,
													title.width, title.height);
	invent->layout.txt_rect.str_drop_mode = font_rect_txt("DESCARTAR", invent->layout.internal_rect.x + (invent->layout.internal_rect.w - title.width*strlen("DESCARTAR"))/2,
													invent->layout.internal_rect.y + 2*gap_between,
													title.width, title.height);
	
	/* menu interno */
	// Primeira coluna (alinhado em 2 gaps a partir do rect interno)
	int x0 = invent->layout.internal_rect.x + 2*gap_between;
	int x1 = invent->layout.internal_rect.x + invent->layout.internal_rect.w/2 + 2*gap_between;
	int y0 = invent->layout.internal_rect.y + 4*gap_between + title.height;
	for (int i=0; i<3; i++){
		invent->layout.matriz_menu_itens[i][0] = (SDL_Rect) { x0, y0 + i*(4*gap_between + 2*gap_between), 4*gap_between, 4*gap_between };
		invent->layout.txt_rect.str_itens_menu[i][0] = font_rect_txt2 (6, x0 + 4*gap_between + 3*gap_between, y0 - gap_between + i*(4*gap_between + 2*gap_between), menu_style.width, menu_style.height);
	}
	// Segunda coluna (alinhado em 2 gaps a partir do meio do rect interno)
	for (int i=0; i<3; i++){
		invent->layout.matriz_menu_itens[i][1] = (SDL_Rect) { x1, y0 + i*(4*gap_between + 2*gap_between), 4*gap_between, 4*gap_between };
		invent->layout.txt_rect.str_itens_menu[i][1] = font_rect_txt2 (6, x1 + 4*gap_between + 3*gap_between, y0 - gap_between + i*(4*gap_between + 2*gap_between), menu_style.width, menu_style.height);
	}
	
}


void invent_quit(){
	if (!invent) return;
	free(invent->array_itens);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			free(invent->layout.txt_rect.str_itens_menu[i][j]);
		}
	}
	
	free(invent->layout.txt_rect.str_open_mode);
	free(invent->layout.txt_rect.str_drop_mode);
	invent_destroy_texture();
	
	free(invent);
	invent = NULL;
}
	
static void inventory_render(SDL_Renderer *ren){
	SDL_SetRenderDrawColor(ren, 79, 52, 235, 255);
	SDL_RenderFillRect(ren, &invent->layout.extern_rect);
	
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderFillRect(ren, &invent->layout.internal_rect);
	
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	
	if (internal_state_ == ABRIR)
		font_render_txt (ren, invent->layout.txt_rect.str_open_mode, "ABRIR", tiny_table);
	else
		font_render_txt (ren, invent->layout.txt_rect.str_drop_mode, "DESCARTAR", tiny_table);
	
	for (int i=0; i<MENU_ROWS; i++){
		for (int j=0; j<MENU_COLS; j++){
			int str_status = control_render[i][j];
			
			if (str_status == -2){ //flag = 0
				//SDL_RenderFillRect(ren, &invent->layout.matriz_menu_itens[i][j]);
				SDL_RenderCopy(ren, icon_menu, NULL, &invent->layout.matriz_menu_itens[i][j]);
				font_render_txt (ren, invent->layout.txt_rect.str_itens_menu[i][j], "??????", tiny_table);
			}
			else if (str_status == -1){
				//nao renderiza
			}
			else{ //flag = id
				char *str_name_item = Item_get_artifact_name(str_status);
				//SDL_RenderFillRect(ren, &invent->layout.matriz_menu_itens[i][j]);
				SDL_RenderCopy(ren, icon_menu, NULL, &invent->layout.matriz_menu_itens[i][j]);
				font_render_txt (ren, invent->layout.txt_rect.str_itens_menu[i][j], str_name_item, tiny_table);
			}
		}
	}
	
	SDL_RenderPresent(ren);
}

void Inventory_update(SDL_Renderer *ren){
	inventory_render(ren);
}
	


