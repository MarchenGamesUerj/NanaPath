#include "Global.h"

SDL_Texture **font_load_table (SDL_Renderer *ren, TTF_Font *fnt, SDL_Color clr); //main.c
SDL_Texture *font_char_convert(char letra, SDL_Texture **table);
SDL_Rect *font_rect_txt (char *txt, int x, int y, int width, int height);
SDL_Rect *font_rect_txt2 (int tam, int x, int y, int width, int height);
void font_render_txt (SDL_Renderer *ren, SDL_Rect *rect_char, char *str, SDL_Texture **table);
void font_render_txt2 (SDL_Renderer *ren, SDL_Rect *rect_char, char *str, int tam, SDL_Texture **table);
void font_destroy_table(SDL_Texture **table); //main.c