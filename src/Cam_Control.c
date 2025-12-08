#include "include/Cam_Control.h"

struct CamInfo{
	int map_w_real;
	int map_h_real;
	int tile_size_game;
	SDL_Rect viewport;
};

CamInfo *Camera_Init(int largura_tela, int altura_tela, int tile_size_game) {
	CamInfo *cam =malloc(sizeof(CamInfo));
	cam->viewport.w = largura_tela;
    cam->viewport.h = altura_tela;
    cam->viewport.x = 0;
    cam->viewport.y = 0;
	cam->tile_size_game = tile_size_game;
    
	return cam;
}

void Camera_update_level (CamInfo *cam, int rows, int cols){
	cam->map_w_real = cols * cam->tile_size_game;
    cam->map_h_real = rows * cam->tile_size_game;
}
	

// Centraliza player e checa limites(bordas)
void Camera_Update(CamInfo *cam, int x, int y) {
    cam->viewport.x = x - (cam->viewport.w/2);
    cam->viewport.y = y - (cam->viewport.h/2);

    if (cam->viewport.x < 0) cam->viewport.x = 0;
    if (cam->viewport.y < 0) cam->viewport.y = 0;

    if (cam->viewport.x > cam->map_w_real - cam->viewport.w)
        cam->viewport.x = cam->map_w_real - cam->viewport.w;
    
	if (cam->viewport.y > cam->map_h_real - cam->viewport.h)
        cam->viewport.y = cam->map_h_real - cam->viewport.h;
}

void Camera_Apply_Rect(CamInfo *cam, SDL_Rect *rect, SDL_Rect *screen_rect) {
    screen_rect->x = rect->x - cam->viewport.x;
    screen_rect->y = rect->y - cam->viewport.y;
    screen_rect->w = rect->w;
    screen_rect->h = rect->h;
}

int Camera_get_viewport_x (CamInfo *cam){
	return cam->viewport.x;
}

int Camera_get_viewport_y (CamInfo *cam){
	return cam->viewport.y;
}

SDL_Rect get_cam_viewport_rect (CamInfo *cam){
	return cam->viewport;
}

void Camera_quit(CamInfo *cam){
	free(cam);
}