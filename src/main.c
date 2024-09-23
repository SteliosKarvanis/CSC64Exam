#include "utils.h"

#include <omp.h>

int main(){
    omp_set_nested(1);
    // INIT
    Record* recordsA = (Record*)malloc(NUM_RECORDS * sizeof(Record));
    Record* recordsB = (Record*)malloc(NUM_RECORDS * sizeof(Record));
    char(*ids)[6] = (char(*)[6])malloc(NUM_RECORDS * sizeof(*ids));

    if(!recordsA || !recordsB || !ids){
        fprintf(stderr, "Erro ao alocar memória.\n");
        exit(1);
    }

    load_records("../db/A.txt", recordsA);
    load_records("../db/B.txt", recordsB);
    load_ids("../db/ids.txt", ids); // Assuming ids are the same for both A and B

    FILE* output = fopen("../output.csv", "w");
    if(!output){
        fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
        free(recordsA);
        free(recordsB);
        free(ids);
        return 1;
    }

    // PROCESS
    // A Cross JOIN
    float minValues[NUM_RECORDS];
    for(int i = 0; i < NUM_RECORDS; i++){
        if(recordsA[i].value < THRESHOLD_CA_MIN)
            continue;
        for(int j = 0; j < NUM_RECORDS; j++){
            if(i == j || recordsA[j].value < THRESHOLD_CA_MIN)
                continue;
        // TODO
        }
    }
    return 0;
}
