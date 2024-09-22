#ifndef UTILS_H
#define UTILS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define NUM_RECORDS 30000
#define THRESHOLD_CA_MIN 0.25f
#define THRESHOLD_CB_MAX 0.75f
#define CHUNK_SIZE 1000000


typedef struct{
    char id[6];
    float value;
} Record;

typedef struct{
    Record* record1;
    Record* record2;
    float combinedValue;
} RecordCx;

typedef struct{
    RecordCx* recordCa;
    RecordCx* recordCb;
    float combinedValue;
} RecordCab;

void load_records(const char* filename, Record* records);
void load_ids(const char* filename, char (*ids)[6]);
char* combine_ids(const char* id1, const char* id2);

#endif