#ifndef PLACE_ITENS_H
#define PLACE_ITENS_H

#include "Global.h"
#include "Cam_Control.h" //renderização 

typedef struct ItemInfo ItemInfo;

void ItemData_Init(SDL_Renderer *ren);
void ItemDatabase_Quit();
ItemInfo* ItemDatabase_GetPotionInfo(int id);
SDL_Texture* ItemDatabase_GetPotionTexture(int id);
ItemInfo* ItemDatabase_GetArtifactInfo(int id);
SDL_Texture* ItemDatabase_GetArtifactTexture(int id);

int Item_get_value (ItemBlock *item); //Play.c usa
int Item_get_flag_info (int id_artifact); //Inventory.c usa para consultar
char *Item_get_artifact_name (int id_artifact);
void Item_update_flag_info (int id_artifact); //Play.c usa quando player abre item
void itens_render(SDL_Renderer *ren, int largura_tela, int altura_tela, int level, int qtd_itens, int **itens_level_checklist, ItemBlock *itens_level, SDL_Rect *itens_rects_level, CamInfo *cam);
void render_DroppedItens (SDL_Renderer *ren, CamInfo *cam, DroppedItem *head);
void render_Exit (SDL_Renderer *ren, CamInfo *cam, SDL_Rect exit_rect, int current_level, int max_levels);
void render_Treasure (SDL_Renderer *ren, CamInfo *cam, SDL_Rect treasure_rect);


#endif