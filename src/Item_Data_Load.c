#include "include/Item_Data_Load.h"

struct ItemInfo{
	enum Type_selection type;
	int id; 
	char name[30];
	int value;
	SDL_Texture *Texture;
	int already_opened;
	union{
		struct{
			int on;
			int speed_bonus;
			int duration;
		} Duration_Params; // efeitos com duração de tempo
		
		struct{ 
			int hp_effect;
		} HP_Params; //poções, Morte instântanea e recuperar metade do hp faltante
		
		struct{
			int num_parts_revealed;
		} Reveal_Params; //revela partes do mapa;
		
		struct{
			int extra_life;
		} Life_Params;
	};
};

static void potions_init();
static int *shuffle_numbers(int num);
static void artifacts_init();

static int ItemData_get_value (int type, int id);
static void Item_update_flag (int id_artifact);
static void potions_render (SDL_Renderer *ren, int id, SDL_Rect *screen_rect);
static void artifacts_render (SDL_Renderer *ren, int id, SDL_Rect *screen_rect);
static void load_itens_textures(SDL_Renderer *ren);


static ItemInfo potions_type[5];
static ItemInfo artifacts_type[8];
static SDL_Texture *potions_textures[5];
//static SDL_Texture *artifacts_textures[8];
static SDL_Texture *artifacts_texture;
static SDL_Texture *treasure_texture;
static SDL_Texture *exit_texture;

static void potions_init(){
	int *rand_number = shuffle_numbers(5);
	
	potions_type[0] = (ItemInfo){
        .type = T_POTION,
        .id = 0,
        .name = "Pocao de cura",
        .value = 2,
        .Texture = potions_textures[rand_number[0]],
		.already_opened = 0,
        .HP_Params = { .hp_effect = 10 }
    };

    potions_type[1] = (ItemInfo){
        .type = T_POTION,
        .id = 1,
        .name = "Pocao de cura",
        .value = 2,
        .Texture = potions_textures[rand_number[1]],
		.already_opened = 0,
        .HP_Params = { .hp_effect = 5 }
    };

    potions_type[2] = (ItemInfo){
        .type = T_POTION,
        .id = 2,
        .name = "Pocao de cura",
        .value = 2,
        .Texture = potions_textures[rand_number[2]],
		.already_opened = 0,
        .HP_Params = { .hp_effect = 3 }
    };

    potions_type[3] = (ItemInfo){
        .type = T_POTION,
        .id = 3,
        .name = "Pocao de cura",
        .value = 2,
        .Texture = potions_textures[rand_number[3]],
		.already_opened = 0,
        .HP_Params = { .hp_effect = 1 }
    };

    potions_type[4] = (ItemInfo){
        .type = T_POTION,
        .id = 4,
        .name = "Pocao estragada",
        .value = 20,
        .Texture = potions_textures[rand_number[4]],
		.already_opened = 0,
        .HP_Params = { .hp_effect = -10 }
    };
	
	free(rand_number);
	rand_number = NULL;
}

static int *shuffle_numbers(int num){
	int *sorteio = malloc(num * sizeof(int));
    if (!sorteio) return NULL;
	int steps = 3*num, j, aux, k;
	
	for (int i=0; i<num; i++){
		sorteio[i] = i;
	}
	
	k=0;
	for (int i=0; i<steps; i++){
		j = rand()%num;
		aux = sorteio[k];
		sorteio[k] = sorteio[j];
		sorteio[j] = aux;
		k++;
		if (k == num) k = 0;
	}
	
	return sorteio;
}
	
static void artifacts_init(){
	artifacts_type[0] = (ItemInfo){
        .type = T_ARTIFACT,
        .id = 0,
        .name = "Item 1",
        .value = 2,
        .Texture = artifacts_texture,
		.already_opened = 0,
        .Duration_Params = { .on = 0, .speed_bonus = 5, .duration = 10 }
    };

    artifacts_type[1] = (ItemInfo){
        .type = T_ARTIFACT,
        .id = 1,
        .name = "Item 2",
        .value = 5,
        .Texture = artifacts_texture,
		.already_opened = 0,
        .Reveal_Params = { .num_parts_revealed = 5 }
    };
	
	artifacts_type[2] = (ItemInfo){
        .type = T_ARTIFACT,
        .id = 2,
        .name = "Item 3",
        .value = 5,
        .Texture = artifacts_texture,
		.already_opened = 0,
        .Life_Params = { .extra_life = 1 }
    };
	
	artifacts_type[3] = (ItemInfo){
        .type = T_ARTIFACT,
        .id = 3,
        .name = "Item 4",
        .value = 5,
        .Texture = artifacts_texture,
		.already_opened = 0,
        .Duration_Params = { .on = 0, .speed_bonus = 0, .duration = 10 }
    };
	
	artifacts_type[4] = (ItemInfo){
        .type = T_ARTIFACT,
        .id = 4,
        .name = "Item 5",
        .value = 5,
        .Texture = artifacts_texture,
		.already_opened = 0,
        .Duration_Params = { .on = 0, .speed_bonus = 0, .duration = 10 }
    };
	
	artifacts_type[5] = (ItemInfo){
        .type = T_ARTIFACT,
        .id = 5,
        .name = "Item 6",
        .value = 5,
        .Texture = artifacts_texture,
		.already_opened = 0,
        .Reveal_Params = { .num_parts_revealed = 0 } //não precisa desse campo
    };
	
	artifacts_type[6] = (ItemInfo){
        .type = T_ARTIFACT,
        .id = 6,
        .name = "Item 7",
        .value = 5,
        .Texture = artifacts_texture,
		.already_opened = 0,
        .Reveal_Params = { .num_parts_revealed = 0 } //não precisa desse campo
    };
	
	artifacts_type[7] = (ItemInfo){
        .type = T_ARTIFACT,
        .id = 7,
        .name = "Item 8",
        .value = 5,
        .Texture = artifacts_texture,
		.already_opened = 0,
        .Life_Params = { .extra_life = 0 } //não precisa desse campo
    };
}

void ItemData_Init(SDL_Renderer *ren) {
	load_itens_textures(ren);
    potions_init();
    artifacts_init();
}

static void load_itens_textures(SDL_Renderer *ren){
	potions_textures[0] = IMG_LoadTexture(ren, "assets/img/itens/potion_green.png");
    assert(potions_textures[0] != NULL);
	potions_textures[1] = IMG_LoadTexture(ren, "assets/img/itens/potion_red.png");
    assert(potions_textures[1] != NULL);
	potions_textures[2] = IMG_LoadTexture(ren, "assets/img/itens/potion_pink.png");
    assert(potions_textures[2] != NULL);
	potions_textures[3] = IMG_LoadTexture(ren, "assets/img/itens/potion_orange.png");
    assert(potions_textures[3] != NULL);
	potions_textures[4] = IMG_LoadTexture(ren, "assets/img/itens/potion_blue.png");
    assert(potions_textures[4] != NULL);
	
	artifacts_texture = IMG_LoadTexture(ren, "assets/img/itens/chest.png");
    assert(artifacts_texture != NULL);
	treasure_texture = IMG_LoadTexture(ren, "assets/img/itens/chest1.png");
    assert(artifacts_texture != NULL);
	exit_texture = IMG_LoadTexture(ren, "assets/img/itens/door.png");
    assert(exit_texture != NULL);

}

void destroy_itens_textures(){
	for (int i=0; i<5; i++){
		SDL_DestroyTexture(potions_textures[i]);
	}
	SDL_DestroyTexture(artifacts_texture);
}

static int ItemData_get_value (int type, int id){
	ItemInfo* info;
	if (type == T_POTION)
		info = ItemDatabase_GetPotionInfo(id);
	
	else if (type == T_ARTIFACT)
		info = ItemDatabase_GetArtifactInfo(id);
	
	return info->value;	
}

static void Item_update_flag (int id_artifact){
	ItemInfo* info = ItemDatabase_GetArtifactInfo (id_artifact);
	if (info->already_opened == 0){
		info->already_opened = 1;
	}
}

ItemInfo* ItemDatabase_GetPotionInfo(int id) {
    return &potions_type[id];
}

SDL_Texture* ItemDatabase_GetPotionTexture(int id) {
    return potions_textures[id];
}

ItemInfo* ItemDatabase_GetArtifactInfo(int id) {
    return &artifacts_type[id];
}

SDL_Texture* ItemDatabase_GetArtifactTexture(int id) {
    return artifacts_texture;
}

void ItemDatabase_Quit() {
    
}

int Item_get_value (ItemBlock *item){
	return ItemData_get_value (item->type, item->id);
}

//retorna se o artefato já foi usado no jogo (Inventory.c usa)
int Item_get_flag_info (int id_artifact){
	ItemInfo *item = ItemDatabase_GetArtifactInfo(id_artifact);
	return item->already_opened;
}

char *Item_get_artifact_name (int id_artifact){
	ItemInfo *item = ItemDatabase_GetArtifactInfo(id_artifact);
	return item->name;
}

void Item_update_flag_info (int id_artifact){
	Item_update_flag (id_artifact);
}
	

void itens_render(SDL_Renderer *ren, int largura_tela, int altura_tela, int level, int qtd_itens, int **itens_level_checklist, ItemBlock *itens_level, SDL_Rect *itens_rects_level, CamInfo *cam){
    SDL_Rect item_screen_pos;
	SDL_Rect screen_bounds = {0, 0, largura_tela, altura_tela};

    for(int i=0; i<qtd_itens; i++){
		if (itens_level_checklist[level][i] == 0){ //item disponivel
		
			ItemBlock *item = &itens_level[i];
			Camera_Apply_Rect(cam, &itens_rects_level[i], &item_screen_pos);
			if (SDL_HasIntersection(&item_screen_pos, &screen_bounds)) {
				if (item->type == T_POTION) {
					potions_render(ren, item->id, &item_screen_pos);
				}
				else if (item->type == T_ARTIFACT){
					artifacts_render(ren, item->id, &item_screen_pos);
				}
			}
		}
    }
}

void render_DroppedItens (SDL_Renderer *ren, CamInfo *cam, DroppedItem *head){
	DroppedItem* aux = head;
	SDL_Rect item_screen_pos;
	
	while (aux != NULL){
		SDL_Rect item = { aux->pos_item.x * TILE_SIZE_GAME,
						aux->pos_item.y * TILE_SIZE_GAME,
						TILE_SIZE_GAME, TILE_SIZE_GAME };
		Camera_Apply_Rect(cam, &item, &item_screen_pos);
		artifacts_render(ren, aux->id, &item_screen_pos);
		aux = aux->next;
	}
}

void render_Exit (SDL_Renderer *ren, CamInfo *cam, SDL_Rect exit_rect, int current_level, int max_levels){
	if (current_level != (max_levels - 1)){
		SDL_Rect exit_screen_pos;
		Camera_Apply_Rect(cam, &exit_rect, &exit_screen_pos);
		//SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); //pinta a saída de vermelho (testes)
		//SDL_RenderFillRect(ren, &exit_screen_pos);
		SDL_RenderCopy(ren, exit_texture, NULL, &exit_screen_pos);
	}
}

void render_Treasure (SDL_Renderer *ren, CamInfo *cam, SDL_Rect treasure_rect){
	SDL_Rect treasure_screen_pos;
	Camera_Apply_Rect(cam, &treasure_rect, &treasure_screen_pos);
	//SDL_SetRenderDrawColor(ren, 111, 66, 245, 255);
	//SDL_RenderFillRect(ren, &treasure_screen_pos);
	SDL_RenderCopy(ren, treasure_texture, NULL, &treasure_screen_pos);
}

static void potions_render (SDL_Renderer *ren, int id, SDL_Rect *screen_rect){
    SDL_RenderCopy(ren, potions_textures[id], NULL, screen_rect);
	/*SDL_SetRenderDrawColor(ren, 252, 3, 240, 255); //pinta todas as poções com rosa (testes)
    SDL_RenderFillRect(ren, screen_rect);*/
}

static void artifacts_render (SDL_Renderer *ren, int id, SDL_Rect *screen_rect){
	SDL_RenderCopy(ren, artifacts_texture, NULL, screen_rect);
    /*SDL_SetRenderDrawColor(ren, 3, 198, 252, 255); //pinta todas as poções com azul claro (testes)
    SDL_RenderFillRect(ren, screen_rect);*/
}

