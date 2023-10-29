#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#define SW 0
#define STR_SW "SW"
#define NW 1
#define STR_NW "NW"
#define NE 2
#define STR_NE "NE"
#define SE 3
#define STR_SE "SE"
#define FALSE 0
#define TRUE 1
#define EPSILON 10e-13

typedef struct point2D point2D_t;
typedef struct rectangle2D rectangle2D_t;
typedef struct dataPoint dataPoint_t;
typedef struct node node_t;
typedef struct data data_t;
typedef struct list list_t;
typedef list_t dict_t;
typedef struct quadtreeNode quadtreeNode_t;

int in_rectangle(rectangle2D_t *rec, point2D_t *point);
quadtreeNode_t *create_empty_quadtree(rectangle2D_t *node_area);
int rectangle_overlap(rectangle2D_t *rec1, rectangle2D_t *rec2);
int in_quadrant(rectangle2D_t *rec, point2D_t *point, int quadrant_code);
int determine_quadrant(rectangle2D_t *rec, point2D_t *point);
point2D_t *create_point(double x, double y);
rectangle2D_t *create_rectangle(point2D_t *bottom_left, point2D_t *upper_right);
quadtreeNode_t *create_leaf_node(dataPoint_t * data_point, rectangle2D_t *rectangle);
quadtreeNode_t *move_to_leaf_node(node_t *head, rectangle2D_t *rectangle);
rectangle2D_t *get_quadrant(rectangle2D_t *rec, int quadrant);
quadtreeNode_t *insert_to_quadtree(quadtreeNode_t *root, dataPoint_t *data_point, rectangle2D_t *rectangle);
double lfabs(double value);
int within_epsilon(double val1, double val2);
int compare_point(point2D_t *p1, point2D_t *p2);
node_t *search_quadtree_node_by_point(quadtreeNode_t *root, point2D_t *point, int show_search_path);
void sorted_insert_data_point(node_t **head, dataPoint_t *data_point);
void make_quadtree(quadtreeNode_t *root, dict_t *dict);
void range_query(quadtreeNode_t *root, rectangle2D_t *range_rectangle, node_t **head);
void remove_duplicate(node_t *head);
void free_quadtree(quadtreeNode_t *root);

#endif