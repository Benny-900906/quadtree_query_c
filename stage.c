#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "data.h"
#include "dict.h"
#include "quadtree.h"
#include "stage.h"

struct dataPoint {
  data_t *data;
  point2D_t *location;
};

struct node {
  dataPoint_t *data_point;
  node_t *next;
};

/*
the function executes s3 program. it accepts co-ordinate pair(s) from stdin and search for the node at that 
position in the quadtree
*/
void perform_stage_3(FILE *output, quadtreeNode_t *root) {
  long double point_query_x = 0.0;
  long double point_query_y = 0.0;
  char str_point_query_x[MAX_NUMBER_LENGTH] = "";
  char str_point_query_y[MAX_NUMBER_LENGTH] = "";

  while ((scanf("%s %s", str_point_query_x, str_point_query_y)) == 2) {
    printf("%s %s -->", str_point_query_x, str_point_query_y);
    fprintf(output, "%s %s\n", str_point_query_x, str_point_query_y);
    point_query_x = strtold(str_point_query_x, NULL);
    point_query_y = strtold(str_point_query_y, NULL);
    point2D_t *target = create_point(point_query_x, point_query_y);
    node_t *head = search_quadtree_node_by_point(root, target, TRUE);
    printf("\n");
      
    if (head != NULL) {
      node_t *ptr = head;
      while (ptr != NULL) {
        print_data_to_file(output, ptr->data_point->data);
        ptr = ptr->next;
      }
    }
    free(target);
  }
}

/* 
the function executes s4 program. it accepts two pair of co-ordinates representing bottom left and 
upper right vertex respectively, it constructs a range rectangle and searches for all the nodes that lie
within the range rectangle in the quadtree
*/
void perform_stage_4(FILE *output, quadtreeNode_t *root) {
  long double bot_left_x = 0.0;
  long double bot_left_y = 0.0;
  long double up_right_x = 0.0;
  long double up_right_y = 0.0;
    char str_bot_left_x[MAX_NUMBER_LENGTH] = "";
    char str_bot_left_y[MAX_NUMBER_LENGTH] = "";
    char str_up_right_x[MAX_NUMBER_LENGTH] = "";
    char str_up_right_y[MAX_NUMBER_LENGTH] = "";

    while ((scanf("%s %s %s %s", str_bot_left_x, str_bot_left_y, str_up_right_x, str_up_right_y)) == 4) {
      bot_left_x = strtold(str_bot_left_x, NULL);
      bot_left_y = strtold(str_bot_left_y, NULL);
      up_right_x = strtold(str_up_right_x, NULL);
      up_right_y = strtold(str_up_right_y, NULL);
      printf("%s %s %s %s -->", str_bot_left_x, str_bot_left_y, str_up_right_x, str_up_right_y);
      fprintf(output, "%s %s %s %s\n", str_bot_left_x, str_bot_left_y, str_up_right_x, str_up_right_y);

      point2D_t *range_bot_left = create_point(bot_left_x, bot_left_y);
      point2D_t *range_up_right = create_point(up_right_x, up_right_y);
      rectangle2D_t *range_rectangle = create_rectangle(range_bot_left, range_up_right);

      node_t *head = (node_t *)malloc(sizeof(node_t));
      assert(head);
      head->data_point = NULL;
      head->next = NULL;
      range_query(root, range_rectangle, &head);
      remove_duplicate(head);

      printf("\n");

      node_t *ptr = head;
      while (ptr != NULL && ptr->data_point != NULL) {
        print_data_to_file(output, ptr->data_point->data);
        ptr = ptr->next;
      }

      node_t *h = NULL;
      while (head != NULL) {
        h = head;
        head = head->next;
        free(h);
      }
      free(range_bot_left);
      free(range_up_right);
      free(range_rectangle);
    }
}