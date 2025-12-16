#ifndef FIND_LARGEST_H
#define FIND_LARGEST_H
#include"Global.h"
#include "Grid_Utils.h"

Element* new_Element (int id, int x0, int xf, int y0, int yf, int tam, int type);
Element* find_Element (Element** head, int id);
Point new_point (int x, int y);
Element* BFS_mapping (int **grid, int **visited, int id, int x, int y, int rows, int cols);
int** mapping_the_map (int **grid, Element** list, int *counter, int rows, int cols);
int find_largest_island (Element* head);
void free_Element_list(Element *head);

#endif
