#include "include/Global.h"
#include "include/Effects.h"


void Add_hover_effect (SDL_Rect* button){
	int num = 30;
	
	button->y -= num/2;
	button->x -= num/2;
	button->w += num;
	button->h += num;
}

void Remove_hover_effect (SDL_Rect *button){
	int num = 30;
	
	button->y += num/2;
	button->x += num/2;
	button->w -= num;
	button->h -= num;
}