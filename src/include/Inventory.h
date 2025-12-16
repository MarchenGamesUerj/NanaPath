#ifndef INVENTORY_H
#define INVENTORY_H

#include "Global.h"
#include "Effects.h"
#include "Font_Utils.h"
#include "Play.h" //avisar drop/open de item
#include "Item_Data_Load.h" //receber flag de uso de artefato

enum TYPE_MSG_INVENTORY{
	ITEM_DROP,
	ITEM_OPEN
};

typedef struct InventMsg{
	enum TYPE_MSG_INVENTORY id_msg;
	int id_item;
}InventMsg;


extern SDL_Texture **tiny_table;

void invent_init (SDL_Renderer *ren, int max_tam, int tam_atual, int current_level_index, int screen_width, int screen_height);
enum State Invent_next_state (SDL_Event *evt, SDL_Renderer *ren);
int invent_add_item (int id_item);
int has_empty_slots();
void invent_remove_item (int id_array);
void invent_update_rank(int new_tam);
void invent_update_index (int new_tam);
void invent_update_new_tam (int new_tam);
void invent_quit();
void Inventory_update(SDL_Renderer *ren);
	
#endif


