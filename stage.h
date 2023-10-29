#ifndef _STAGE_H_
#define _STAGE_H_

#define INPUT_FILE_INDEX 2
#define OUTPUT_FILE_INDEX 3
#define START_LON_INDEX 4
#define START_LAT_INDEX 5
#define END_LON_INDEX 6
#define END_LAT_INDEX 7

typedef struct dataPoint dataPoint_t;
typedef struct node node_t;

void perform_stage_3(FILE *output, quadtreeNode_t *root);
void perform_stage_4(FILE *output, quadtreeNode_t *root);

#endif