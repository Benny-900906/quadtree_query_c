#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "data.h"
#include "dict.h"
#include "quadtree.h"

struct point2D {
  double x;
  double y;
};

struct rectangle2D {
  point2D_t *bottom_left;
  point2D_t *upper_right;
};

struct dataPoint {
  data_t *data;
  point2D_t *location;
};

struct node {
  dataPoint_t *data_point;
  node_t *next;
};

struct data {
  int footpath_id;
  char *address;
  char *clue_sa;
  char *asset_type;
  double deltaz;
  double distance;
  double gradelin;
  int mcc_id;
  int mccid_int;
  double rlmax;
  double rlmin;
  char *segside;
  int statusid;
  int streetid;
  int street_group;
  double start_lat;
  double start_lon;
  double end_lat;
  double end_lon;
};

struct list {
  node_t *head;
  node_t *tail;
  int size;
};

struct quadtreeNode {
  /* head == head of the linked list; used to store datapoint(s) with sharing location (in point2D_t); */
  node_t *head;
  rectangle2D_t *rectangle;
  quadtreeNode_t *sw;
  quadtreeNode_t *nw;
  quadtreeNode_t *ne;
  quadtreeNode_t *se;
};


/* 
tests whether a given 2D point lies within the rectangle and returns 1 (True) if it does. Otherwise, 
return 0 (False) 
*/
int in_rectangle(rectangle2D_t *rec, point2D_t *point) {
  double x = point->x;
  double y = point->y;
  double left_bound = rec->bottom_left->x;
  double bot_bound = rec->bottom_left->y;
  double right_bound = rec->upper_right->x;
  double up_bound = rec->upper_right->y;
  if (x >= left_bound && x <= right_bound && y >= bot_bound && y <= up_bound) {
    return TRUE;
  }
  return FALSE;
}

/* 
create an empty quadtree node as the root of the quadtree
*/
quadtreeNode_t *create_empty_quadtree(rectangle2D_t *node_area) {
  quadtreeNode_t *tree = (quadtreeNode_t *)malloc(sizeof(quadtreeNode_t));
  assert(tree);
  tree->rectangle = node_area;
  tree->head = NULL;
  tree->sw = tree->nw = tree->ne = tree->se = NULL;
  return tree;
}

/* 
tests whether two given rectangles overlap each other and returns TRUE (1) if they do. Otherwise, return FALSE (0) 
*/
int rectangle_overlap(rectangle2D_t *rec1, rectangle2D_t *rec2) {
  if (rec1->upper_right->y < rec2->bottom_left->y || rec1->bottom_left->y > rec2->upper_right->y) {
    return FALSE;
  }
  if (rec1->upper_right->x < rec2->bottom_left->x || rec1->bottom_left->x > rec2->upper_right->x) {
    return FALSE;
  } 
  return TRUE;
}

/*  
check if the point is located inside the selected quadrant of the given rectangle
*/
int in_quadrant(rectangle2D_t *rec, point2D_t *point, int quadrant_code) {
  assert(quadrant_code >= SW && quadrant_code <= SE);
  int result = FALSE;
  rectangle2D_t *quadrant = get_quadrant(rec, quadrant_code);
  /* check if the point is in the quadrant */
  if (in_rectangle(quadrant, point)) {
    result = TRUE;
  }
  free(quadrant->bottom_left);
  free(quadrant->upper_right);
  free(quadrant);
  return result;
}

/* 
given a rectangle, determine which quadrant the given point belongs to 
*/
int determine_quadrant(rectangle2D_t *rec, point2D_t *point) {
  if (in_quadrant(rec, point, SW)) {
    return SW;
  } else if (in_quadrant(rec, point, NW)) {
    return NW;
  } else if (in_quadrant(rec, point, NE)) {
    return NE;
  } else if (in_quadrant(rec, point, SE)) {
    return SE;
  }
  /* the assignment specification assumed point(s) will always be in bound, 
  so this function will never reach -1 */
  return -1;
}

/*
given two double values, create a point, where we can use it as the location of a datapoint or as an vertex
of a rectangle
*/
point2D_t *create_point(double x, double y) {
  point2D_t *point = (point2D_t *)malloc(sizeof(point2D_t));
  assert(point);
  point->x = x;
  point->y = y;
  return point;
}

/*
given two points; a bottom_left vertex and an upper_right vertex, create a rectangle
*/
rectangle2D_t *create_rectangle(point2D_t *bottom_left, point2D_t *upper_right) {
  rectangle2D_t *rectangle = (rectangle2D_t *)malloc(sizeof(rectangle2D_t));
  assert(rectangle);
  rectangle->bottom_left = bottom_left;
  rectangle->upper_right = upper_right;
  return rectangle;
}

/* 
create a leaf node where its subqudrants sw, nw, ne, sw are initially NULL, with data_point inserted 
into node->head->data_point (head is node_t*, it acts like a linked list to store data points that 
share the same region/point)
*/
quadtreeNode_t *create_leaf_node(dataPoint_t * data_point, rectangle2D_t *rectangle) {
  quadtreeNode_t *node = (quadtreeNode_t *)malloc(sizeof(quadtreeNode_t));
  assert(node);
  node->sw = node->nw = node->ne = node->se = NULL;
  node->rectangle = rectangle;
  node->head = (node_t *)malloc(sizeof(node_t));
  assert(node->head);
  node->head->data_point = data_point;
  node->head->next = NULL;
  return node;
}

/* 
this function is used when an insertion operation is being performed and node A reaches a leaf node/quadrant that is
occupied by node B. We first make the leaf node to an internal node, and push node B down to a suitable
subquadrant using this function. We then perform node A insertion until an empty lead node/quadrant is reached.
*/
quadtreeNode_t *move_to_leaf_node(node_t *head, rectangle2D_t *rectangle) {
  quadtreeNode_t *node = (quadtreeNode_t *)malloc(sizeof(quadtreeNode_t));
  assert(node);
  node->sw = node->nw = node->ne = node->se = NULL;
  node->rectangle = rectangle;
  node->head = head;
  return node;
}

/* 
given the quadrant we are trying to get, reture the rectangle pointer to that quadrant
*/
rectangle2D_t *get_quadrant(rectangle2D_t *rec, int quadrant) {
  assert(quadrant >= SW && quadrant <= SE);
  point2D_t *bottom_left = NULL;
  point2D_t *upper_right = NULL;
  switch (quadrant) {
    case SW:
      bottom_left = create_point(rec->bottom_left->x, rec->bottom_left->y);
      upper_right = create_point((rec->upper_right->x + rec->bottom_left->x) / 2, (rec->upper_right->y + rec->bottom_left->y) / 2);
      break;
    case NW:
      bottom_left = create_point(rec->bottom_left->x, (rec->bottom_left->y + rec->upper_right->y) / 2);
      upper_right = create_point((rec->upper_right->x + rec->bottom_left->x) / 2, rec->upper_right->y);
      break;
    case NE:
      bottom_left = create_point((rec->bottom_left->x + rec->upper_right->x) / 2, (rec->bottom_left->y + rec->upper_right->y) / 2);
      upper_right = create_point(rec->upper_right->x, rec->upper_right->y);
      break;
    case SE:
      bottom_left = create_point((rec->bottom_left->x + rec->upper_right->x) / 2, rec->bottom_left->y);
      upper_right = create_point(rec->upper_right->x, (rec->upper_right->y + rec->bottom_left->y) / 2);
      break;
  }
  assert(bottom_left != NULL && upper_right != NULL);
  return create_rectangle(bottom_left, upper_right);
}

/*
insert a node into a quadtree 
*/
quadtreeNode_t *insert_to_quadtree(quadtreeNode_t *root, dataPoint_t *data_point, rectangle2D_t *rectangle) {
  if (root == NULL) {
    return create_leaf_node(data_point, rectangle);
  }

  /* if we reach a leaf node that is full, transfer the node to an internal node by spliting it */
  if (root->head != NULL) {
    node_t *temp_head = root->head;
    root->head = NULL;
    int leaf_node_new_quadrant = determine_quadrant(root->rectangle, temp_head->data_point->location);
    switch (leaf_node_new_quadrant) {
      case SW:
        root->sw = move_to_leaf_node(temp_head, get_quadrant(root->rectangle, SW));
        break;
      case NW:
        root->nw = move_to_leaf_node(temp_head, get_quadrant(root->rectangle, NW));
        break;
      case NE:
        root->ne = move_to_leaf_node(temp_head, get_quadrant(root->rectangle, NE));
        break;
      case SE:
        root->se = move_to_leaf_node(temp_head, get_quadrant(root->rectangle, SE));
        break;
    }
  }
  /* when we are trying to insert node A and reaches a leaf node B that is full, we tend to split node B
  and insert node B into a suitable subquadrant. We then perfrom node A insertion again with a suitable subquadrant 
  (rectangle) pointer. But if we reach a full leaf node in the next insertion, the subquadrant (rectangle) pointer from
  the last stage will be lost and hard to reach afterward, so we free the subquadrant pointer here */
  if (rectangle != root->rectangle) {
    free(rectangle->bottom_left);
    free(rectangle->upper_right);
    free(rectangle);
  }

  /* trying to insert the node until an empty quadrant/leaf node is reached */
  int new_quadrant = determine_quadrant(root->rectangle, data_point->location);
  switch (new_quadrant) {
    case SW:
      root->sw = insert_to_quadtree(root->sw, data_point, get_quadrant(root->rectangle, SW));
      break;
    case NW:
      root->nw = insert_to_quadtree(root->nw, data_point, get_quadrant(root->rectangle, NW));
      break;
    case NE:
      root->ne = insert_to_quadtree(root->ne, data_point, get_quadrant(root->rectangle, NE));
      break;
    case SE:
      root->se = insert_to_quadtree(root->se, data_point, get_quadrant(root->rectangle, SE));
      break;
  }
  return root;
}

/* 
the function returns an absolute number of the given value. |value|
*/
double lfabs(double value) {
  return (value < 0) ? -value : value;
}

/* 
the function checks if the difference betweenn two values is within EPSILON 10e-13 
*/
int within_epsilon(double val1, double val2) {
  if (lfabs(val1 - val2) < EPSILON) {
    return 1;
  }
  return 0;
}

/* 
checks if two points are close to each other. the differences between their x and y should 
be within EPSILON 
*/
int compare_point(point2D_t *p1, point2D_t *p2) {
  if (within_epsilon(p1->x, p2->x) && within_epsilon(p1->y, p2->y)) {
    return 0;
  }
  return 1;
}

/* 
the function searches the node from the quadtree using the given point. It compares with nodes in the 
quadtree and compare them. If they are considered the same, return the node.
the show_search_path parameter accepts either TRUE or FALSE, it identicates whether the search path 
should be printed along with the searching operation
*/
node_t *search_quadtree_node_by_point(quadtreeNode_t *root, point2D_t *point, int show_search_path) {
  assert(show_search_path == TRUE || show_search_path == FALSE);
  if (root == NULL) {
    return NULL;
  }

  /* check for equality when we reach every occupied leaf node */
  if (root->head != NULL) {
    if (compare_point(root->head->data_point->location, point) == 0) {
      return root->head;
    }
  }
    
  /* we keep traversing along the suitable quadrant until we reach an occupied node or
  until we reach a NULL */
  int quadrant = determine_quadrant(root->rectangle, point);
  switch (quadrant) {
    case SW:
      if (show_search_path == TRUE) {printf(" %s", STR_SW);}
      return search_quadtree_node_by_point(root->sw, point, show_search_path);
      break;
    case NW:
      if (show_search_path == TRUE) {printf(" %s", STR_NW);}
      return search_quadtree_node_by_point(root->nw, point, show_search_path);
      break;
    case NE:
      if (show_search_path == TRUE) {printf(" %s", STR_NE);}
      return search_quadtree_node_by_point(root->ne, point, show_search_path);
      break;
    case SE:
      if (show_search_path == TRUE) {printf(" %s", STR_SE);}
      return search_quadtree_node_by_point(root->se, point, show_search_path);
      break;
  }
  return NULL;
}

/*
the function inserts data point(s) into a linked list in a sorted order
*/
void sorted_insert_data_point(node_t **head, dataPoint_t *data_point) {
  node_t *node = (node_t *)malloc(sizeof(node_t));
  assert(node != NULL);
  node->data_point = data_point;
  node->next = NULL;

  // if data_point's footpathid samller than the head's footpathid, insert it at the front
  if (data_point->data->footpath_id < (*head)->data_point->data->footpath_id) {
    node->next = *head;
    *head = node;
  } else {
    node_t *ptr = (*head);
    while (ptr != NULL) {
      /* if is reaches the end of the node, append it at the last position */ 
      if (ptr->next == NULL) {
        ptr->next = node;
        return;
      /* find the suitable position to insert the node */
      } else {
        if (data_point->data->footpath_id >= ptr->data_point->data->footpath_id && data_point->data->footpath_id <= ptr->next->data_point->data->footpath_id) {
          node->next = ptr->next;
          ptr->next = node;
          return;
        }
      }
      ptr = ptr->next;
    }
  }
}


/* 
the function creates a quadtree using the data points stored in the dictionary
*/
void make_quadtree(quadtreeNode_t *root, dict_t *dict) {
  if (dict == NULL) {
    return;
  }
  /* instantiate the root of the quadtree, and insert the first (root) node manually */
  node_t *ptr = dict->head;
  root->head = (node_t *)malloc(sizeof(node_t));
  assert(root->head);
  root->head->data_point = ptr->data_point;
  root->head->next = NULL;
  ptr = ptr->next;

  while (ptr != NULL) {
    /* check if the given location in the quadtree is full. If head != NULL, then there are more than one points
    sharing the same region/location */
    node_t *head = search_quadtree_node_by_point(root, ptr->data_point->location, FALSE);
    /* no node with identical point/location found */
    if (head == NULL) {
      root = insert_to_quadtree(root, ptr->data_point, root->rectangle);
    /* found node with identical point/location, insert the node into the linked list that is utilized to 
    store nodes in sharing the same region */
    } else if (head->data_point != NULL) {
      sorted_insert_data_point(&head, ptr->data_point);
    }
    ptr = ptr->next;
  }
}

/* 
the function frees the entire quadtree
*/
void free_quadtree(quadtreeNode_t *root) {
  if (root == NULL) {
    return;
  }

  /* freeing a lead node (occupied node) */
  if (root->head != NULL) {
    node_t *ptr = NULL;
    while (root->head != NULL) {
      ptr = root->head;
      root->head = root->head->next;
      ptr->next = NULL;
      ptr->data_point = NULL;
      free(ptr);
    }
  }
  free(root->rectangle->bottom_left);
  free(root->rectangle->upper_right);
  free(root->rectangle);
  free(root->head);

  free_quadtree(root->sw);
  free_quadtree(root->nw);
  free_quadtree(root->ne);
  free_quadtree(root->se);

  free(root);
}

/* 
the function receives a node_t **head (linked list) and a rectangle (range query), and ascendingly store all the nodes that 
lies within the region into the head (linked list)
*/
void range_query(quadtreeNode_t *root, rectangle2D_t *range_rectangle, node_t **head) {
  if (rectangle_overlap(root->rectangle, range_rectangle) && root->head != NULL && in_rectangle(range_rectangle, root->head->data_point->location)) {
    node_t *ptr = root->head;
    
    while (ptr != NULL) {
      /* if the data_point is NULL in head */
      if ((*head)->data_point == NULL) {
        (*head)->data_point = ptr->data_point;
        (*head)->next = NULL;
      } else {
        sorted_insert_data_point(head, ptr->data_point);
      }
      ptr = ptr->next;
    }
    return;
  }

  /* recursively search in the suitable subquardrants */
  if (root->sw && rectangle_overlap(root->sw->rectangle, range_rectangle)) {
    printf(" %s", STR_SW);
    range_query(root->sw, range_rectangle, head);
  }

  if (root->nw && rectangle_overlap(root->nw->rectangle, range_rectangle)) {
    printf(" %s", STR_NW);
    range_query(root->nw, range_rectangle, head);
  }

  if (root->ne && rectangle_overlap(root->ne->rectangle, range_rectangle)) {
    printf(" %s", STR_NE);
    range_query(root->ne, range_rectangle, head);
  }

  if (root->se && rectangle_overlap(root->se->rectangle, range_rectangle)) {
    printf(" %s", STR_SE);
    range_query(root->se, range_rectangle, head);
  }  
}

/* 
when we perform a range query in a quadtree, there might be some duplicating nodes, since 
for each node we insert them twice with end (lon/lat) and start (lon/lat). Thus, the linked list
may contain a number of duplicates, so we remove duplicates using this function
*/
void remove_duplicate(node_t *head) {
  node_t *ptr = head;
  while (ptr != NULL && ptr->next != NULL) {
    if (ptr->data_point->data->footpath_id == ptr->next->data_point->data->footpath_id) {
      node_t *delete_node = ptr->next;
      ptr->next = delete_node->next;
      free(delete_node);
      delete_node = NULL;
    }
    ptr = ptr->next;
  }
}

