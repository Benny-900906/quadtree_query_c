#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "data.h"
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

struct node {
  dataPoint_t *data_point;
  node_t *next;
};

struct dataPoint {
  data_t *data;
  point2D_t *location;
};

/* 
the function creates an empty node with mem allocated data_point and data pointers
*/
node_t *create_empty_node() {
  node_t *node = (node_t *)malloc(sizeof(node_t));
  node->next = NULL;
  node->data_point = (dataPoint_t *)malloc(sizeof(dataPoint_t));
  node->data_point->data = (data_t *)malloc(sizeof(data_t));
  node->data_point->location = NULL;
  assert(node && node->data_point && node->data_point->data);
  return node;
}

/* 
copy all the attributes from a data source to a data desination, this function is used 
to create an identical data point but with different location/point. Since for each node we stored in 
the dictionary, we need to transfer it into two separate quadtreeNodes. One with start (lon/lat) point, another
one with end (lon/lat) point.
*/
void copy_data(data_t *des, data_t *src) {
  des->footpath_id = src->footpath_id;
  des->address = strdup(src->address);
  des->clue_sa = strdup(src->clue_sa);
  des->asset_type = strdup(src->asset_type);
  des->deltaz = src->deltaz;
  des->distance = src->distance;
  des->gradelin = src->gradelin;
  des->mcc_id = src->mcc_id;
  des->mccid_int = src->mccid_int;
  des->rlmax = src->rlmax;
  des->rlmin = src->rlmin;
  des->segside = strdup(src->segside);
  des->statusid = src->statusid;
  des->streetid = src->streetid;
  des->street_group = src->street_group;
  des->start_lat = src->start_lat;
  des->start_lon = src->start_lon;
  des->end_lat = src->end_lat;
  des->end_lon = src->end_lon;
}

/*
create a node that uses end (lon/lat) as its location/point
*/
node_t *create_end_point_node(node_t *node) {
  node_t *ptr = (node_t *)malloc(sizeof(node_t));
  assert(ptr);
  ptr->next = NULL;
  ptr->data_point = (dataPoint_t *)malloc(sizeof(dataPoint_t));
  assert(ptr->data_point);
  ptr->data_point->data = (data_t *)malloc(sizeof(data_t));
  assert(ptr->data_point->data);
  copy_data(ptr->data_point->data, node->data_point->data);
  ptr->data_point->location = create_point(node->data_point->data->end_lon, node->data_point->data->end_lat);
  return ptr;
}

void fill_node(char **str_array, data_t *ptr) {
  ptr->footpath_id = atoi(str_array[0]);
  ptr->address = strdup(str_array[1]);
  ptr->clue_sa = strdup(str_array[2]);
  ptr->asset_type = strdup(str_array[3]);
  sscanf(str_array[4], "%lf", &ptr->deltaz);
  sscanf(str_array[5], "%lf", &ptr->distance);
  sscanf(str_array[6], "%lf", &ptr->gradelin);
  ptr->mcc_id = atoi(str_array[7]);
  ptr->mccid_int = atoi(str_array[8]);
  sscanf(str_array[9], "%lf", &ptr->rlmax);
  sscanf(str_array[10], "%lf", &ptr->rlmin);
  ptr->segside = strdup(str_array[11]);
  ptr->statusid = atoi(str_array[12]);
  ptr->streetid = atoi(str_array[13]);
  ptr->street_group = atoi(str_array[14]);
  sscanf(str_array[15], "%lf", &ptr->start_lat);
  sscanf(str_array[16], "%lf", &ptr->start_lon);
  sscanf(str_array[17], "%lf", &ptr->end_lat);
  sscanf(str_array[18], "%lf", &ptr->end_lon);
}

void free_str_array(char **str_array, int n) {
  for (int i = 0; i < n; i ++) {
    free(str_array[i]);
  }
  free(str_array);
  str_array = NULL;
}

void complete_node(char *line, dataPoint_t *ptr) {
  /* create an empty (pointer to str) array */
  char **str_array = (char **)malloc(sizeof(char *) * NUM_OF_FIELDS);
  assert(str_array);
  int index = 0;
  char *tok = line, *end = line;
  /* tokenize line (separate by ,) */
  while (tok != NULL) {
    strsep(&end, ",");
    /* if the token is empty */
    if ((strcmp(tok, "")) == 0) {
      str_array[index] = strdup("");
      index ++;
    /* if token starts with the " character, capture the token without the " character */
    } else if (tok[0] == '\"') {
      char tok_str[strlen(tok)];
      strncpy(tok_str, &tok[1], strlen(tok) -1);
      tok_str[strlen(tok) - 1] = '\0';
      str_array[index] = strdup(tok_str);
    /* if token ends with the " character, capture the token without the " character,
    and concat with the previous token that starts with the " character */
    } else if (tok[strlen(tok) - 1] == '\"') {
      char new_str[strlen(tok) + strlen(str_array[index]) + 1];
      char tok_str[strlen(tok)];
      strcpy(new_str, str_array[index]);
      /* capture token without the " character */
      strncpy(tok_str, &tok[0], strlen(tok) - 1);
      tok_str[strlen(tok) - 1] = '\0';
      /* concat previous token that starts with the " character with , and the current token */
      strcat(new_str, ",");
      strcat(new_str, tok_str);
      /* str_array[index] is strdup(ed) the previous token that starts with the " character
      free it before applying the strdup function */
      free(str_array[index]);
      str_array[index] = strdup(new_str);
      index ++;
    /* if the token neither empty nor contains ", store the entire token */
    } else {
      str_array[index] = strdup(tok);
        index ++;
      } 
      tok = end;
  }
  assert(index == NUM_OF_FIELDS);
    
  fill_node(str_array, ptr->data);
  ptr->location = create_point(ptr->data->start_lon, ptr->data->start_lat); 

  free_str_array(str_array, NUM_OF_FIELDS);
}

/* 
the function prints the data_t attributes to the specified output stream
*/
void print_data_to_file(FILE *output, data_t *data) {
  char *specifier = "--> footpath_id: %d || address: %s || clue_sa: %s || asset_type: %s || deltaz: %.2lf ||"
  " distance: %.2lf || grade1in: %.1lf || mcc_id: %d || mccid_int: %d || rlmax: %.2lf || rlmin: %.2lf || segside: %s ||"
  " statusid: %d || streetid: %d || street_group: %d || start_lat: %.6lf || start_lon: %.6lf || end_lat: %.6lf ||"
  " end_lon: %.6lf ||\n";
  fprintf(output, specifier, data->footpath_id, data->address, data->clue_sa, data->asset_type, data->deltaz, data->distance, data->gradelin,
  data->mcc_id, data->mccid_int, data->rlmax, data->rlmin, data->segside, data->statusid, data->streetid, data->street_group, data->start_lat,
  data->start_lon, data->end_lat, data->end_lon);
}

/* 
the function frees the linked list that was used store nodes + datapoints 
*/
void free_list(node_t *head) {
  node_t *ptr = NULL;
  while (head != NULL) {
    ptr = head;
    head = head->next;
    free(ptr->data_point);
    free(ptr);
    ptr = NULL;
  }
}