#include "include/Font_Utils.h"
#define TAM (126 - 32 +1)

SDL_Texture **font_load_table (SDL_Renderer *ren, TTF_Font *fnt, SDL_Color clr){ //usada na main.c
	SDL_Texture **table = malloc (TAM * sizeof(SDL_Texture*));
	
	char letra = 32;
	char str[2];
	str[0] = letra, str[1] = '\0';
	
	for(int i=0; i<TAM; i++){
		SDL_Surface* sfc = TTF_RenderText_Blended(fnt, str, clr);
		assert(sfc != NULL);
		table[i] = SDL_CreateTextureFromSurface(ren, sfc);
		assert(table[i] != NULL);
		SDL_FreeSurface(sfc);
		letra++;
		str[0]=letra;
	}
	return table;
}

SDL_Texture *font_char_convert(char letra, SDL_Texture **table){ 
	int ponteiro = letra - 32;
	if (ponteiro<0 || ponteiro>=TAM) return table[63-32]; //?
	return table[ponteiro];
}

SDL_Rect *font_rect_txt (char *txt, int x, int y, int width, int height){ //quem chama tem que dar free()
	int tam = strlen(txt);
	SDL_Rect *rect = malloc(tam*sizeof(SDL_Rect));
	for (int i=0; i<tam; i++){
		rect[i] = (SDL_Rect) { x + i*width, y, width, height};
	}
	return rect;
}

SDL_Rect *font_rect_txt2 (int tam, int x, int y, int width, int height){
	SDL_Rect *rect = malloc(tam*sizeof(SDL_Rect));
	for (int i=0; i<tam; i++){
		rect[i] = (SDL_Rect) { x + i*width, y, width, height};
	}
	return rect;
}

void font_render_txt (SDL_Renderer *ren, SDL_Rect *rect_char, char *str, SDL_Texture **table){
	int tam = strlen(str);
	for(int i=0; i<tam; i++){
		SDL_Texture *txt = font_char_convert(str[i], table);
		SDL_RenderCopy(ren, txt, NULL, &rect_char[i]);
	}
}

void font_render_txt2 (SDL_Renderer *ren, SDL_Rect *rect_char, char *str, int tam, SDL_Texture **table){
	int str_tam = strlen(str);
	if (str_tam<tam) tam = str_tam;
	for(int i=0; i<tam; i++){
		SDL_Texture *txt = font_char_convert(str[i], table);
		SDL_RenderCopy(ren, txt, NULL, &rect_char[i]);
	}
}

void font_destroy_table(SDL_Texture **table){ //usada na main.c
	for (int i=0; i<TAM; i++){
		SDL_DestroyTexture(table[i]);
	}
	free(table);
}
