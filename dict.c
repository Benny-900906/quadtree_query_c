#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "data.h"
#include "dict.h"
#include "quadtree.h"

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

struct dataPoint {
  data_t *data;
  point2D_t *location;
};

struct node {
  dataPoint_t *data_point;
  node_t *next;
};

struct list {
  node_t *head;
  node_t *tail;
  int size;
};

/* 
create an empty dictionary
*/
dict_t *create_empty_dictionary() {
  dict_t *dict = (dict_t *)malloc(sizeof(dict_t));
  assert(dict);
  dict->head = dict->tail = NULL;
  dict->size = 0;
  return dict;
}

/* 
make a node from the given line, and insert into the dict 
*/
void insert_to_dict(char *line, dict_t *dict) {
  /* an empty node, next == NULL, data_point == malloc(ed) */
  node_t *ptr = create_empty_node();

  /* complete node with Start (lon/lat) Point */
  complete_node(line, ptr->data_point);

  /* insert into an empty dictionary */
  if (dict->size == 0) {
    dict->head = dict->tail = ptr;
    (dict->size) ++;
  } else {
    dict->tail->next = ptr;
    dict->tail = ptr;
    (dict->size) ++;
  }

  node_t *end_point_node = create_end_point_node(ptr);

  dict->tail->next = end_point_node;
  dict->tail = end_point_node;
  (dict->size) ++;
}

/* 
read lines from the input stream, make nodes based on those lines
and insert the nodes into the dict 
*/
void make_dictionary(FILE *input, dict_t *dict) {
  char line[MAX_LINE_CHAR + 1] = "";

  /* getting rid of the header line */
  fscanf(input, "%[^\n] ", line);

  while ((fscanf(input, "%[^\n] ", line)) == 1) {
    insert_to_dict(line, dict);
  }
}

/*
free the entire dict
*/
void free_dictionary(dict_t *dict) {
  if (dict == NULL) {
    return;
  }

  /* empty dict */
  if (dict->head == NULL) {
    free(dict);
    return;
  }
    
  while (dict->head != NULL) {
    node_t *ptr = dict->head;
    dict->head = dict->head->next;
    
    free(ptr->data_point->data->address);
    free(ptr->data_point->data->clue_sa);
    free(ptr->data_point->data->segside);
    free(ptr->data_point->data->asset_type);
    free(ptr->data_point->data);
    ptr->data_point->data = NULL;
    
    free(ptr->data_point->location);
    free(ptr->data_point);
    free(ptr);
    ptr = NULL;
  }
  dict->head = dict->tail = NULL;
  free(dict);
}