#include "include/Find_Largest_Island.h"

static void insert_Element (Element** head, Element* new_element);
static NodePoint *new_point_node (int x, int y);
static void enqueue_Point (NodePoint *point, queueSearch *q);
static NodePoint* dequeue_Point (queueSearch *q);
static queueSearch *new_queueSearch ();
static void free_queueSearch (queueSearch *q);
static int queue_is_empty( queueSearch *q);	
static int greater_than (int num1, int num2);
static int less_than (int num1, int num2);


Element* new_Element (int id, int x0, int xf, int y0, int yf, int tam, int type){
	Element* element = (Element*) malloc(sizeof(Element));
	element->id = id;
	element->x0 = x0;
	element->xf = xf;
	element->y0 = y0;
	element->yf = yf;
	element->tam = tam;
	element->type = type;
	element->next = NULL;
	
	return element;
}

static void insert_Element (Element** head, Element* new_element){
	new_element ->next = *head;
	*head = new_element;
}

Element* find_Element (Element** head, int id){
	Element* wanted = *head;
	while (wanted != NULL){
		if(wanted->id == id)
			return wanted;
		wanted = wanted->next;
	}
	return NULL;
}

void free_Element_list(Element *head){
    Element *aux;
    while (head != NULL) {
        aux = head;
        head = head->next;
        free(aux);
    }
}

Point new_point (int x, int y){
	Point p;
	p.x = x;
	p.y = y;
	
	return p;
}

static NodePoint *new_point_node (int x, int y){
	NodePoint *np = malloc(sizeof(NodePoint));
    if (!np) return NULL;

    np->pos = new_point(x, y);
    np->next = NULL;

    return np;
}

static void enqueue_Point (NodePoint *point, queueSearch *q){
	if (!point) return;
	point ->next = NULL;
	if(q->head == NULL) q->head = point;
	else q->tail->next = point;
	q->tail = point;
	q->tam++;
}

static NodePoint* dequeue_Point (queueSearch *q){
	NodePoint *last_point;
	last_point = q->head;
	if (last_point == NULL) return NULL; //fila vazia
	q->head = last_point->next;
	q->tam--;
	return last_point;
}

static queueSearch *new_queueSearch (){
	queueSearch *new;
	new = malloc (sizeof(queueSearch));
	if (new == NULL) return NULL;
	new->head = NULL;
	new->tail = NULL;
	new->tam = 0;
	return new;
}

static void free_queueSearch (queueSearch *q){
	NodePoint *aux, *next;
	aux = q->head;
	while (aux != NULL){
		next = aux->next;
		free(aux);
		aux = next;
	}
	free(q);
}

static int queue_is_empty( queueSearch *q) {
   return q->head == NULL;
}
		
static int greater_than (int num1, int num2){
	if (num1>=num2) return num1;
	else return num2;
}

static int less_than (int num1, int num2){
	if (num1<=num2) return num1;
	else return num2;
}

Element* BFS_mapping (int **grid, int **visited, int id, int x, int y, int rows, int cols){
	int type = grid[y][x], vizinho_x, vizinho_y;
	NodePoint *point = new_point_node (x, y);
	queueSearch *queue = new_queueSearch();
	enqueue_Point(point, queue);
	visited[y][x]=id;
	
	//int h[8] = {-1, -1, -1,  0, 0, 1, 1, 1};
    //int v[8] = {-1,  0,  1, -1, 1,-1, 0, 1};
	int h[4] = {-1,1,0,0};
	int v[4] = {0,0,-1,1};
	
	int i, j, counter = 1, x0 = x, xf = x, y0 = y, yf = y;
	NodePoint *aux;
	while(!queue_is_empty(queue)){
		aux = dequeue_Point(queue);
		i = aux->pos.y;
		j = aux->pos.x;
		
		for (int k = 0; k<4; k++){
			vizinho_x = j+h[k];
			vizinho_y = i+v[k];
			if (vizinho_x>=0 && (vizinho_x) < cols && vizinho_y >= 0 && vizinho_y < rows
			&& grid[vizinho_y][vizinho_x] == type && visited[vizinho_y][vizinho_x] == 0){
				visited[vizinho_y][vizinho_x] = id;
				counter++;
				point = new_point_node (vizinho_x, vizinho_y);
				enqueue_Point(point, queue);
				
				x0 = less_than(x0, vizinho_x);
				xf = greater_than(xf, vizinho_x);
				y0 = less_than(y0, vizinho_y);
				yf = greater_than(yf, vizinho_y);
			}
		}
		free(aux);
	}
	
	Element* new_elem = new_Element (id, x0, xf, y0, yf, counter, type);
	free_queueSearch(queue);
	
	return new_elem;
}

int** mapping_the_map (int **grid, Element** list, int *counter, int rows, int cols){
	int id=0;
	int **visited = allocate_grid_int (rows, cols);
	reset_grid_int (visited, rows, cols); //zera matriz
		
	for(int i=0; i<rows; i++){
		for (int j=0; j<cols; j++){
			if (visited[i][j] == 0){
				id++;
				Element* new_region = BFS_mapping (grid, visited, id, j, i, rows, cols);
				insert_Element(list, new_region);
			}
		}
	}
	*counter = id;
	return visited; 
}

int find_largest_island (Element* head){
	int largest_island_id = -1;
    int largest_island_size = 0; 
	Element* aux = head;

	while (aux != NULL){
		if(aux->type == 0){
			if (aux->tam > largest_island_size){
				largest_island_size = aux->tam;
                largest_island_id = aux->id;
			}
		}
		aux = aux->next;
	}
    
	return largest_island_id;
}