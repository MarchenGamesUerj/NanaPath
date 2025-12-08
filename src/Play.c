#include "include/Play.h"

#define HUD_HEIGHT 100

enum State_Water_Click{
	STATE_READY,
	STATE_CLICKING
};

typedef struct{
	SDL_Rect entrada_rect;
	SDL_Rect exit_rect;
	SDL_Rect treasure_rect;
} CurrentLevelObjects;

typedef struct {
	int init;
	
	CamInfo *cam;
	int rows;
	int cols;
	
	int current_level_index;
	int game_mode;
	
	int **control_itens; //0 disponível, 1 coletado
	int *control_treasures; //-1 nao gerado, x>0 disponível, -2 coletado
	DroppedItem **drop_list;
	InfoLevel *levels_info;
	CurrentLevelObjects level_objects;
	SeedInfo *seeds;
} InfoState;

static InfoState info_state;
static Player *player; //isolado
static HudInfo *hud;
static int count = 0;

static void play_init (SDL_Renderer *ren);
static void Play_init_level(int level_index);
static enum State play_evt_handler(SDL_Event* evt, SDL_Renderer* ren);
static void set_level_objects (Point entrada, Point saida, Point *tesouro, int level);
static void Spread_Player_Updates (Player *player);
static void play_render(SDL_Renderer *ren);
static int check_collision_water (SDL_Rect rect, int **map, int rows, int cols, enum DIRECTION_STATE dir);
static int check_collision_mud (SDL_Rect rect, int **map, int rows, int cols);
static int check_colision_itens();
static int check_colision_treasures (SDL_Rect player_rect, int current_level, int max_treasures);
static int check_colision_droppedItens(DroppedItem **head);
static int check_colision_exit (SDL_Rect player_rect, SDL_Rect exit_rect);
static void free_DroppedItem_list(DroppedItem *head);
static int check_drop_spot (DroppedItem *head, int x, int y);
static int play_pick_drop_spot (Player *player, int **map, DroppedItem *head, Point *pos_item, int rows, int cols);
static void play_update_droplist (Player *player, int **map, DroppedItem **head, int id, int rows, int cols);
static int AUX_ClickTimeoutCount(Uint32* last_click_time, Uint32 timeout_ms);

static void play_init (SDL_Renderer *ren){
	info_state.init = 1;
	info_state.game_mode = options_get_game_mode(); //essa informação precisa vir da Options
	ItemData_Init(ren); //ISSO TEM QUE SAIR DAQUI

	
	info_state.cam = Camera_Init(LARGURA_TELA, (ALTURA_TELA - HUD_HEIGHT), TILE_SIZE_GAME);
	info_state.seeds = set_seeds(info_state.game_mode, MAX_LEVEL);
	
	// Inicializa todos com NULL como flag para nao inicializar 2x
	info_state.control_itens = malloc(MAX_LEVEL * sizeof(int*)); 
	info_state.drop_list = malloc(MAX_LEVEL * sizeof (DroppedItem*));
	for(int i=0; i<MAX_LEVEL;i++){
		info_state.control_itens[i] = NULL;
		info_state.drop_list[i] = NULL;
	}
	
	info_state.control_treasures = malloc(MAX_TREASURES * sizeof(int));
	for(int i=0; i<MAX_TREASURES;i++)
		info_state.control_treasures[i] = -1;
	info_state.control_treasures[MAX_TREASURES - 1] = MAX_LEVEL - 1;
	
	player = create_player();
	minimap_init (MAX_LEVEL);
	invent_init (25, 15, 0, LARGURA_TELA, ALTURA_TELA);
	hud = init_hud (LARGURA_TELA, ALTURA_TELA, get_player_hp(player), get_rank_player_name (player), get_player_max_hp(player), 0, get_player_extra_life(player));
	Play_init_level(0);
}

static void Play_init_level(int level_index) {
	if (info_state.levels_info != NULL) free_level(info_state.levels_info);
    info_state.levels_info = level_init(level_index, info_state.seeds);
	if (get_level_has_treasure(level_index, info_state.seeds) && info_state.control_treasures[0] == -1)
		info_state.control_treasures[0] = level_index;
	
	info_state.rows = info_state.levels_info->rows;
	info_state.cols = info_state.levels_info->cols;

    info_state.current_level_index = level_index;
	//minimap_load_level(info_state.current_level_index, info_state.levels_info->map, info_state.rows, info_state.cols);
	Camera_update_level (info_state.cam, info_state.rows, info_state.cols);
	
	if(info_state.control_itens[level_index] == NULL){
		int qtd = info_state.levels_info->qtd_itens;
		info_state.control_itens[level_index] = calloc(qtd, sizeof(int));
	}
	
	set_level_objects(info_state.levels_info->entrada, info_state.levels_info->saida, info_state.levels_info->tesouro, level_index);
	
	update_player_position (player, info_state.levels_info->entrada);
	SDL_Rect player_info = get_player_rect(player);
	Camera_Update(info_state.cam, player_info.x + player_info.w/2, player_info.y + player_info.h/2);
	
	Point saida;
	if(info_state.level_objects.exit_rect.w == 0) saida = (Point) {-1,-1};
	else saida = (Point) { info_state.level_objects.exit_rect.x/TILE_SIZE_GAME, info_state.level_objects.exit_rect.y/TILE_SIZE_GAME };
	
	Point treasure;
	if(info_state.level_objects.treasure_rect.w == 0) treasure = (Point) { -1, -1 };
	else treasure = (Point) { info_state.level_objects.treasure_rect.x/TILE_SIZE_GAME, info_state.level_objects.treasure_rect.y/TILE_SIZE_GAME };
	
	minimap_load_level(info_state.current_level_index, info_state.levels_info->map, info_state.rows, info_state.cols, saida, treasure);
	update_minimap (player_info.x/TILE_SIZE_GAME, player_info.y/TILE_SIZE_GAME);
	hud_update_level(hud, info_state.current_level_index + 1);
}

static void set_level_objects (Point entrada, Point saida, Point *tesouro, int level){
	info_state.level_objects.entrada_rect = (SDL_Rect) { entrada.x * TILE_SIZE_GAME, entrada.y * TILE_SIZE_GAME, TILE_SIZE_GAME, TILE_SIZE_GAME };
	if (level != (MAX_LEVEL - 1))
		info_state.level_objects.exit_rect = (SDL_Rect) { saida.x * TILE_SIZE_GAME, saida.y * TILE_SIZE_GAME, TILE_SIZE_GAME, TILE_SIZE_GAME };
	else 
		info_state.level_objects.exit_rect = (SDL_Rect) { 0, 0, 0, 0 }; //just in case (w = 0 sendo usado como flag)
	
	info_state.level_objects.treasure_rect = (SDL_Rect) { 0, 0, 0, 0 };
	for (int i=0; i<MAX_TREASURES; i++){
		if (tesouro != NULL && info_state.control_treasures[i] == level){
			info_state.level_objects.treasure_rect = (SDL_Rect) { tesouro->x * TILE_SIZE_GAME, tesouro->y * TILE_SIZE_GAME, TILE_SIZE_GAME, TILE_SIZE_GAME };
			break;
		}
	}
}

enum State Play_next_state (SDL_Event *evt, SDL_Renderer *ren){
	enum State new_state = play_evt_handler(evt, ren);
	
	play_render(ren);
	return new_state;
}

static enum State play_evt_handler(SDL_Event* evt, SDL_Renderer* ren) {
	enum State new_state = STATE_PLAY;
	static int current_dir = NONE;
	
	if (info_state.init != 1){
		play_init(ren);
	}
    
	if(evt->type == SDL_QUIT)
		new_state = STATE_QUIT;
	
	if(evt->type == SDL_KEYDOWN){
	//eventos que causam mudança de estado
		if (evt->key.keysym.sym == SDLK_s) new_state = STATE_INVENTORY;
		else if (evt->key.keysym.sym == SDLK_d) new_state = STATE_MAP;
		else if (evt->key.keysym.sym == SDLK_SPACE) new_state = STATE_PAUSE;
		else if (evt->key.keysym.sym == SDLK_ESCAPE) new_state = STATE_MENU;
	
	//eventos que afetam o jogo
		else if (evt->key.keysym.sym == SDLK_UP){ current_dir = UP;}
		else if (evt->key.keysym.sym == SDLK_DOWN){ current_dir = DOWN; }
		else if (evt->key.keysym.sym == SDLK_LEFT){ current_dir = LEFT;}
		else if (evt->key.keysym.sym == SDLK_RIGHT){ current_dir = RIGHT;}
		
		else if (evt->key.keysym.sym == SDLK_F6){
			if(info_state.current_level_index + 1 < MAX_LEVEL){
				Play_init_level(info_state.current_level_index + 1);
			}
		}
		else if (evt->key.keysym.sym == SDLK_F5){
			if(info_state.current_level_index - 1 >= 0){
				Play_init_level(info_state.current_level_index - 1);
			}
		}
	}
	
	else if (evt->type == SDL_KEYUP) {
			if (evt->key.keysym.sym == SDLK_UP && current_dir == UP)       		current_dir = NONE;
			else if (evt->key.keysym.sym == SDLK_DOWN && current_dir == DOWN)	current_dir = NONE;
			else if (evt->key.keysym.sym == SDLK_LEFT && current_dir == LEFT) 	current_dir = NONE;
			else if (evt->key.keysym.sym == SDLK_RIGHT && current_dir == RIGHT) current_dir = NONE;
	}
	
	enum Type_terrain terrain = TERRAIN_GROUND;
	if (check_collision_mud (get_player_rect(player), info_state.levels_info->map, info_state.levels_info->rows, info_state.levels_info->cols)) terrain = TERRAIN_MUD;
	if (!check_collision_water (get_player_rect(player), info_state.levels_info->map, info_state.levels_info->rows, info_state.levels_info->cols, current_dir)){
		update_player_coord(player, terrain, current_dir, info_state.levels_info->rows, info_state.levels_info->cols);
	}
	
	check_colision_itens();
	check_colision_droppedItens(&info_state.drop_list[info_state.current_level_index]);
	check_colision_treasures (get_player_rect(player), info_state.current_level_index, MAX_TREASURES);
	check_colision_exit (get_player_rect(player), info_state.level_objects.exit_rect);
	Spread_Player_Updates (player);
	
    return new_state;
}

static void play_render(SDL_Renderer *ren){
	SDL_RenderClear(ren);
	int cam_x = Camera_get_viewport_x(info_state.cam);
	int cam_y = Camera_get_viewport_y(info_state.cam);
	
	render_map (ren, info_state.levels_info->map, cam_x, cam_y, TILE_SIZE_GAME, HUD_HEIGHT, info_state.rows, info_state.cols);
	itens_render(ren, LARGURA_TELA, ALTURA_TELA - HUD_HEIGHT, info_state.current_level_index, info_state.levels_info->qtd_itens, info_state.control_itens, info_state.levels_info->itens, info_state.levels_info->rects_itens, info_state.cam);
	render_DroppedItens (ren, info_state.cam, info_state.drop_list[info_state.current_level_index]);
	render_Exit (ren, info_state.cam, info_state.level_objects.exit_rect, info_state.current_level_index, MAX_LEVEL);
	render_Treasure (ren, info_state.cam, info_state.level_objects.treasure_rect);
	player_render(player, ren, cam_x, cam_y);
	hud_render(ren, hud);
	
	SDL_RenderPresent(ren);
}

void Play_update(SDL_Renderer *ren){
	play_render(ren);
}

void play_quit(){
	Camera_quit(info_state.cam);
	destroy_player(player);
	invent_quit();
	hud_quit(hud);
	Minimap_quit();
	//ItemDatabase_Quit(); //considerar usar quando tiver texturas
	
	free(info_state.seeds);
	free_level(info_state.levels_info);
	free(info_state.control_treasures);
	
	
	if (info_state.control_itens != NULL) {
        for (int i = 0; i < MAX_LEVEL; i++) {
            if (info_state.control_itens[i] != NULL) {
                free(info_state.control_itens[i]);
            }
        }
        free(info_state.control_itens);
    }
	
	for (int i=0; i<MAX_LEVEL; i++)
		free_DroppedItem_list(info_state.drop_list[i]);
	free(info_state.drop_list);
	
	info_state.seeds = NULL;
    info_state.levels_info = NULL;
}

void play_reset(){
	if(info_state.init == 1){
		info_state.init = 0;
		play_quit();
		printf("Partida reiniciada! Modo de jogo alterado.\n");
	}
}
	



// ----------------------------------------------------------
// Fcs de suporte (retirar quando der?)

static int check_collision_water (SDL_Rect rect, int **map, int rows, int cols, enum DIRECTION_STATE dir){
    int start_i, end_i, start_j, end_j;

    switch (dir){
        case UP:
            start_i = rect.x / TILE_SIZE_GAME;
            end_i   = (rect.x + rect.w - 1) / TILE_SIZE_GAME;
            start_j = end_j = (rect.y - 1) / TILE_SIZE_GAME;
            break;
        case DOWN:
            start_i = rect.x / TILE_SIZE_GAME;
            end_i   = (rect.x + rect.w - 1) / TILE_SIZE_GAME;
            start_j = end_j = (rect.y + rect.h) / TILE_SIZE_GAME;
            break;
        case LEFT:
            start_i = end_i = (rect.x - 1) / TILE_SIZE_GAME;
            start_j = rect.y / TILE_SIZE_GAME;
            end_j   = (rect.y + rect.h - 1) / TILE_SIZE_GAME;
            break;
        case RIGHT:
            start_i = end_i = (rect.x + rect.w) / TILE_SIZE_GAME;
            start_j = rect.y / TILE_SIZE_GAME;
            end_j   = (rect.y + rect.h - 1) / TILE_SIZE_GAME;
            break;
        default: return 0;
    }

    for (int j = start_j; j <= end_j; j++) {
        for (int i = start_i; i <= end_i; i++) {
            if (i >= 0 && i < cols && j >= 0 && j < rows) {
                if (map[j][i] == 1) return 1;
            }
        }
    }
	return 0;
}

		
			
			
static int check_collision_mud (SDL_Rect rect, int **map, int rows, int cols){
		
		int current_i = rect.x / TILE_SIZE_GAME;
		int start_i = current_i - 1;
		int end_i = current_i + 1;
		
		int current_j = rect.y / TILE_SIZE_GAME;
		int start_j = current_j - 1;
		int end_j = current_j + 1;
		
		if (start_i < 0) start_i = 0;
		if (end_i >= cols) end_i = cols -1;
		
		if (start_j < 0) start_j = 0;
		if (end_j >= rows) end_j = rows -1;
		
		for(int i = start_i; i<=end_i; i++){
			for(int j = start_j; j<=end_j; j++){
				SDL_Rect teste = {i*TILE_SIZE_GAME, j*TILE_SIZE_GAME, TILE_SIZE_GAME, TILE_SIZE_GAME};
				if (SDL_HasIntersection(&rect, &teste) && map[j][i] == 2)
					return 1;
			}
		}
		
		return 0;
}

static void free_DroppedItem_list(DroppedItem *head){
    DroppedItem *aux;
    while (head != NULL) {
        aux = head;
        head = head->next;
        free(aux);
    }
}

//atualiza control_itens e stats do player
static int check_colision_itens(){
	InfoLevel *level = info_state.levels_info;
    int num = level->qtd_itens;

	for(int i=0; i<num; i++){
		if (info_state.control_itens[info_state.current_level_index][i] == 0) {
			SDL_Rect *item = &level->rects_itens[i];
			SDL_Rect player_rect = get_player_rect(player);
			if (SDL_HasIntersection(&player_rect, item)){
					info_state.control_itens[info_state.current_level_index][i] = 1;
					int item_value = Item_get_value (&level->itens[i]);
					update_player_score(player, item_value);
					printf("Player score: %d	|	Item Value: %d	| Type Item: %d		| Id Item: %d \n", get_player_score (player), item_value, level->itens[i].type, level->itens[i].id); //TESTE: TIRAR DEPOIS
					int type = level->itens[i].type;
					if (type == T_POTION) update_collected_potions(player);
					else if (type == T_ARTIFACT){
						int artifact_id = level->itens[i].id;
						if(invent_add_item (artifact_id)){
							update_collected_artifacts(player);
						}
						
					}
					return 1;
			}
		}
	}
	return 0;
}

static int check_colision_treasures (SDL_Rect player_rect, int current_level, int max_treasures){
	
	if (info_state.level_objects.treasure_rect.w ==0) return 0;
	
	if (SDL_HasIntersection(&player_rect, &info_state.level_objects.treasure_rect)){
	
		for (int i = 0; i<max_treasures; i++){
			if (info_state.control_treasures[i] == current_level){
				info_state.control_treasures[i] = -2; //marca como coletado
				update_player_score(player, 20);
				printf("Tesouro coletado! | Player score: %d\n", get_player_score (player));
				info_state.level_objects.treasure_rect = (SDL_Rect) { 0, 0, 0, 0 };
				update_collected_treasures(player);
				return 1;
			}
		}
	}
	return 0;
}

static int check_colision_exit (SDL_Rect player_rect, SDL_Rect exit_rect){
	if (SDL_HasIntersection(&player_rect, &exit_rect)){
		//se for a primeira vez, pode incrementar score em 10 pontos
		if(info_state.current_level_index + 1 < MAX_LEVEL){
			if (info_state.control_itens[info_state.current_level_index + 1] == NULL){ //gambiarra para saber se level seguinte já tinha sido desbloquado
				update_player_score(player, 15);
				printf("Desbloqueou novo mapa! | Player score: %d\n", get_player_score (player));
			}
			Play_init_level(info_state.current_level_index + 1);
		}
		return 1;
	}
	return 0;
}
	

static int check_colision_droppedItens(DroppedItem **head){
	DroppedItem *aux = *head;
	DroppedItem *prev = NULL;
	SDL_Rect player_rect = get_player_rect(player);
	
	while (aux != NULL){
		SDL_Rect item = { aux->pos_item.x * TILE_SIZE_GAME,
						aux->pos_item.y * TILE_SIZE_GAME,
						TILE_SIZE_GAME, TILE_SIZE_GAME };
		
		if (SDL_HasIntersection(&player_rect, &item)){
			if (invent_add_item (aux->id)){
				DroppedItem *aux_remove = aux;
				
				if (prev == NULL)
					*head = aux->next;
				else
					prev->next = aux->next;
				
				//aux = aux->next;
				free(aux_remove);
				return 1;
			}
		}
		prev = aux;
		aux = aux->next;
	}
	return 0;
}

// Play.c chama depois de atualizar player
static void Spread_Player_Updates (Player *player) {
	int hp = get_player_hp(player);
	int extra_life = get_player_extra_life(player);
	
	hud_update_info (hud, hp, extra_life);
	
	SDL_Rect player_info = get_player_rect(player);
	
	int x_center_player = player_info.x + player_info.w/2;
	int y_center_player = player_info.y + player_info.h/2;
	Camera_Update(info_state.cam, x_center_player, y_center_player);
	
	if(update_minimap(player_info.x/(float)TILE_SIZE_GAME, player_info.y/(float)TILE_SIZE_GAME)){ //tá truncando??
		update_player_score(player, 1);
		//printf("Área revelada! | Player score: %d\n", get_player_score (player));
	}
	
}

// Inventory.c chama quando precisa avisar algo
void Game_Update_Inventory (InventMsg *update_msg){
	switch (update_msg->id_msg){
		case ITEM_DROP:
			//faz o que precisa fazer
			play_update_droplist (player, info_state.levels_info->map, &info_state.drop_list[info_state.current_level_index], update_msg->id_item, info_state.rows, info_state.cols);
			break;
		case ITEM_OPEN:
			//faz o que precisa fazer
			Item_update_flag_info (update_msg->id_item); // atualiza flag de uso do item
			update_consumed_artifacts(player); //atualiza stats player
			break;
	}
}

// Player.c chama quando precisa avisar algo
void Game_Update_Player (PlayerMsg *update_msg){
	switch (update_msg->id_msg){
		case PLAYER_RANK_UPDATE:
			/*int max_hp = get_player_max_hp(player);
			enum RANK new_rank = get_player_rank(player);
			int new_tam = get_player_max_itens (player);
			
			Inventory_update_rank (new_tam);*/
			hud_update_rank_player (hud, get_rank_player_name (player), get_player_max_hp(player), get_player_extra_life(player));
			
			break;
		
		case PLAYER_DEATH:
			//faz o que precisa fazer (animação? reposiciona no mapa?)
			break;
			
		case PLAYER_GAME_OVER:
			//faz o que precisa fazer (animação? Finaliza jogo)
			break;
			
		case PLAYER_VICTORY:
			//faz o que precisa fazer (animação? Finaliza jogo)
			printf("Você venceu!\n");
			break;
			
	}
}

static int check_drop_spot (DroppedItem *head, int x, int y){
    while (head != NULL) {
		if (head->pos_item.x == x && head->pos_item.y == y) return 0;
        head = head->next;
    }
	return 1;
}

static int play_pick_drop_spot (Player *player, int **map, DroppedItem *head, Point *pos_item, int rows, int cols){
	SDL_Rect player_rect = get_player_rect(player);
	int x_player = player_rect.x / TILE_SIZE_GAME;
	int y_player = player_rect.y / TILE_SIZE_GAME;
	
	int dx[] = {0,0,-2,2,-2,-2,2,2};
	int dy[] = {-2,2,0,0,-2,2,-2,2};
	
	for (int i = 0; i < 8; i++){
		int drop_x = x_player + dx[i];
		int drop_y = y_player + dy[i];
		
		if (drop_x >= 0 && drop_x < cols && drop_y>=0 && drop_y < rows){
			SDL_Rect teste = {drop_x, drop_y, TILE_SIZE_GAME, TILE_SIZE_GAME};
		
			if (!SDL_HasIntersection(&player_rect, &teste)){
				if (map[drop_y][drop_x] == 0 && check_drop_spot(head, drop_x, drop_y)){
					*pos_item = (Point) { drop_x, drop_y };
					return 1;
				}
			}
		}
		
	}
	return 0;
}

//funçao para ser usada quando o inventário avisar que houve drop
static void play_update_droplist (Player *player, int **map, DroppedItem **head, int id, int rows, int cols){
	Point pos_drop;
	if (play_pick_drop_spot (player, map, *head, &pos_drop, rows, cols)){
		DroppedItem *new_drop = malloc (sizeof(DroppedItem));
		new_drop->id = id;
		new_drop->pos_item = pos_drop;
		new_drop->next = *head;
		*head = new_drop;
	}
	else{
		invent_add_item(id);
	}
}

int AUX_ClickTimeoutCount(Uint32* last_click_time, Uint32 timeout_ms) {
    Uint32 delta = SDL_GetTicks() - *last_click_time;
    if (delta < timeout_ms) {
	*last_click_time = SDL_GetTicks();	
        return 1; 
    } else {
        *last_click_time = 0;
        return 0;
    }
}