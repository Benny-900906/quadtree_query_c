#ifndef _DICT_H_
#define _DICT_H_

typedef struct data data_t;
typedef struct dataPoint dataPoint_t;
typedef struct node node_t;
typedef struct list list_t;
typedef list_t dict_t;

dict_t *create_empty_dictionary();
void insert_to_dict(char *line, dict_t *dict);
void make_dictionary(FILE *input, dict_t *dict);
void free_dictionary(dict_t *dict);

#endif