#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <assert.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 800
#define ROWS 80
#define COLS 80

#define MAX_LEVEL 5
#define MAX_TREASURES 2
#define MAP_TYPE M_CELLULAR_AUTOMATA
#define OCTAVES 5
#define PERSISTENCE 0.55f 
#define CA_STEPS 12
#define CA_CHANCE 0.45
#define TILE_SIZE_GAME 30
#define TILE_SIZE_MINIMAP 10

#define TIMEOUT 200

enum State {
	STATE_INICIAL,
    STATE_MENU,
	STATE_OPTIONS,
    STATE_PLAY,
    STATE_QUIT,
	STATE_PAUSE,
	STATE_MAP,
	STATE_INVENTORY
};

//Build_Map_Utils e Item_Data_Load
enum Type_selection{
	T_POTION,
	T_ARTIFACT
};

enum Game_Mode{
	RANDOM_MODE,
	FIXED_MODE
};

enum Type_terrain{
	TERRAIN_GROUND,
	TERRAIN_WATER,
	TERRAIN_MUD
};

typedef struct Element{
	int id;
	int x0;
	int xf;
	int y0;
	int yf;
	int tam;
	int type;
	struct Element *next;
} Element;

typedef struct{
	int x,y;
} Point;

typedef struct DroppedItem{
	int id;
	Point pos_item;
	struct DroppedItem *next;
} DroppedItem;	

typedef struct NodePoint{
	Point pos;
	struct NodePoint *next;
} NodePoint;

typedef struct{
	NodePoint *head;
	NodePoint *tail;
	int tam;
} queueSearch;

typedef struct{
	int type;
	int id;
	Point pos_item;
} ItemBlock;

typedef struct{ 
	int level;
	int **map;
	int rows;
	int cols;
	
	Point entrada;
	Point saida;
	Point *tesouro;
	
	ItemBlock *itens;
	SDL_Rect *rects_itens;
	int qtd_itens;
} InfoLevel;


#endif
