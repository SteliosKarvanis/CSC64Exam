#include "utils.h"


void loadA(const char* filename, Record* records, int& count){
    count = 0;
    FILE* file = fopen(filename, "r");
    if(!file){
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", filename);
        exit(1);
    }
    for(int i = 0; i < NUM_RECORDS; ++i){
        fscanf(file, "%f", &records[count].value);
        if(records[count].value > THRESHOLD_CA_MIN){
            records[count].idIdx = count;
            count++;
        }
    }
    fclose(file);
}

void loadB(const char* filename, Record* records, int& count){
    count = 0;
    FILE* file = fopen(filename, "r");
    if(!file){
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", filename);
        exit(1);
    }
    for(int i = 0; i < NUM_RECORDS; ++i){
        fscanf(file, "%f", &records[count].value);
        if(records[count].value < THRESHOLD_CB_MAX){
            records[count].idIdx = count;
            count++;
        }
    }
    fclose(file);
}

void load_ids(const char* filename, char (*ids)[6]){
    FILE* file = fopen(filename, "r");
    if(!file){
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", filename);
        exit(1);
    }
    for(int i = 0; i < NUM_RECORDS; ++i){
        fscanf(file, "%s", ids[i]);
    }
    fclose(file);
}

char* combine_ids(const char* id1, const char* id2){
    static char result[6];
    result[0] = id1[0];
    result[1] = id2[1];
    result[2] = id1[2];
    result[3] = id2[3];
    result[4] = id1[4];
    result[5] = '\0';
    return result;
}