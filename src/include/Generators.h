#ifndef GEN_H
#define GEN_H

#include "Global.h"
#include "Find_Largest_Island.h"

Point allocate_item (int x0, int xf, int y0, int yf, int **map, int **regions, int largest_id, int type_item);
int how_many_itens (int level, int tam_map);
enum Type_selection pick_random_item_type ();
int pick_random_item (int level, enum Type_selection select_);
void set_size_cols_rows (int *num_rows, int *num_cols, int level);

#endif