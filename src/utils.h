#ifndef UTILS_H
#define UTILS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NUM_RECORDS 3000
// #define NUM_RECORDS 30
#define THRESHOLD_CA_MIN 0.25f
#define THRESHOLD_CB_MAX 0.75f
#define CHUNK_SIZE 10000
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct{
    int idIdx;
    float value;
} Record;

void loadA(const char* filename, Record* records, int& count);
void loadB(const char* filename, Record* records, int& count);
void load_ids(const char* filename, char (*ids)[6]);
char* combine_ids(const char* id1, const char* id2);

#endif