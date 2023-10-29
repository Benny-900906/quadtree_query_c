#ifndef _DATA_H_
#define _DATA_H_

#define MAX_NUMBER_LENGTH 25
#define MAX_LINE_CHAR 512
#define MAX_FIELD_CHAR 128
#define NUM_OF_FIELDS 19

typedef struct data data_t;
typedef struct node node_t;
typedef struct dataPoint dataPoint_t;

node_t *create_empty_node();
void copy_data(data_t *des, data_t *src);
node_t *create_end_point_node(node_t *node);
void fill_node(char **str_array, data_t *ptr);
void free_str_array(char **str_array, int n);
void complete_node(char *line, dataPoint_t *ptr);
void print_data_to_file(FILE *output, data_t *data);
void free_list(node_t *head);

#endif