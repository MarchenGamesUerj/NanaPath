#ifndef CAM_CONTROL_H
#define CAM_CONTROL_H

#include "Global.h"

typedef struct CamInfo CamInfo;

CamInfo *Camera_Init(int largura_tela, int altura_tela, int tile_size_game);
void Camera_update_level (CamInfo *cam, int rows, int cols);
void Camera_Update(CamInfo *cam, int x, int y);
void Camera_Apply_Rect(CamInfo *cam, SDL_Rect *rect, SDL_Rect *screen_rect);

int Camera_get_viewport_x (CamInfo *cam);
int Camera_get_viewport_y (CamInfo *cam);
SDL_Rect Camera_get_viewport_rect (CamInfo *cam);

void Camera_quit(CamInfo *cam);

#endif