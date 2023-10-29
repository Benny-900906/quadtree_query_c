#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "data.h"
#include "dict.h"
#include "quadtree.h"
#include "stage.h"

int main(int argc, char **argv) {
  FILE *input = fopen(argv[INPUT_FILE_INDEX], "r");
  assert(input);
  FILE *output = fopen(argv[OUTPUT_FILE_INDEX], "w");
  assert(output);

  dict_t *dict = create_empty_dictionary();
  make_dictionary(input, dict);

  // x of bottomLeft for Node Area
  long double start_lon = strtold(argv[START_LON_INDEX], NULL);
  // y of bottomLeft for Node Area
  long double start_lat = strtold(argv[START_LAT_INDEX], NULL);
  // x of upperRight for Node Area
  long double end_lon = strtold(argv[END_LON_INDEX], NULL);
  // y of upperRight for Node Area
  long double end_lat = strtold(argv[END_LAT_INDEX], NULL);

  point2D_t *bottom_left = create_point(start_lon, start_lat);
  point2D_t *upper_right = create_point(end_lon, end_lat);
  rectangle2D_t *node_area = create_rectangle(bottom_left, upper_right);

  quadtreeNode_t *root = create_empty_quadtree(node_area);
  make_quadtree(root, dict);

  // stage 4
  perform_stage_4(output, root);
   
  free_quadtree(root);
  free_dictionary(dict);
  bottom_left = upper_right = NULL;
  node_area = NULL;
  fclose(input);
  fclose(output);
  return 0;
}