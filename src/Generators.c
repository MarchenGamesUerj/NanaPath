#include "include/Generators.h"


Point allocate_item (int x0, int xf, int y0, int yf, int **map, int **regions, int largest_id, int type_item){
	Point point;
	int x, y, aux=1, limit=100, counter_limit=0;
	
	while(aux && counter_limit<limit){
		x = rand() % (xf - x0 + 1) + x0;
		y = rand() % (yf - y0 + 1) + y0;
	
		if (map[y][x] == 0 && regions[y][x] == largest_id){
			map[y][x] = type_item;
			point = new_point(x,y);
			aux=0;
		}
		counter_limit++;
	}
	return point;
}
	
int how_many_itens (int level, int tam_map){
	float tam_base = 0.02*tam_map;   // 2% do mapa
    float level_coeficient = 1.0f - (level * 0.1f); // diminui 10% por nível
    
    int qtd = (int)(tam_base*level_coeficient);
    
    return qtd;
}

enum Type_selection pick_random_item_type (){
	enum Type_selection select;
	int num_select = rand() % 100 + 1;
	
	if(num_select <= 50) select = T_POTION;
	else select = T_ARTIFACT;
	
	return select;
}

int pick_random_item (int level, enum Type_selection select_){
	int w = rand() % 100 + 1;
	
	//weight[level][item]
	int weight_potion[5][5] = {
		{10,25,25,30,10},
		{10,25,20,30,15},
		{10,20,20,30,20},
		{10,20,20,20,30},
		{10,15,25,20,30}
	};
	
	int weight_artifacts[5][8] = {
		{25,20,15,15,5,0,0,20}, //nao pode Unfall e morte instantanea
		{20,22,8,15,5,8,7,15},
		{15,15,10,15,15,7,8,15},
		{15,15,10,15,10,10,10,15},
		{10,15,10,10,10,15,15,15}
	};
	
	int acc = 0;
	
	if (select_ == T_POTION){
		for(int i=0; i<5; i++){
			acc+=weight_potion[level][i];
			if(w <= acc) return i;
		}
	}
	
	else if (select_ == T_ARTIFACT){
		for(int i=0; i<8; i++){
			acc+=weight_artifacts[level][i];
			if(w <= acc) return i;
		}
	}
	
	return -1;
}

//Tamanho do mapa cresce 20% por nível 
void set_size_cols_rows (int *num_rows, int *num_cols, int level){
	int min_size = 80;
	
	*num_rows = min_size + 0.25*min_size*level;
	*num_cols = *num_rows;

}

	
