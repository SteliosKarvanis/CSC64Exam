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
    int countA = 0;
    int countB = 0;
    for(int i = 0; i < NUM_RECORDS; i++){
        if(recordsA[i].value > THRESHOLD_CA_MIN){
            countA++;
        }
        if(recordsB[i].value < THRESHOLD_CB_MAX){
            countB++;
        }
    }
    printf("countA: %d\n", countA);
    printf("countB: %d\n", countB);
    // A Cross JOIN
    RecordCx* recordCa = (RecordCx*)malloc(countA * countA * sizeof(RecordCx));
#pragma omp parallel for
    for(int i = 0; i < countA; i++){
#pragma omp parallel for
        for(int j = 0; j < countA; j++){
            recordCa[i * countA + j].record1 = &recordsA[i];
            recordCa[i * countA + j].record2 = &recordsA[j];
            if(recordCa[i * countA + j].record1->value < recordCa[i * countA + j].record2->value){
                recordCa[i * countA + j].combinedValue = recordCa[i * countA + j].record1->value;
            }
            else{
                recordCa[i * countA + j].combinedValue = recordCa[i * countA + j].record2->value;
            }
        }
    }
#pragma omp barrier
    // RecordCx* recordCb = (RecordCx*)malloc(countB * countB * sizeof(RecordCx));
// #pragma omp parallel for
//     for(int i = 0; i < countB; i++){
// #pragma omp parallel for
//         for(int j = 0; j < countB; j++){
//             recordCb[i * countB + j].record1 = &recordsB[i];
//             recordCb[i * countB + j].record2 = &recordsB[j];
//             if(recordCb[i * countB + j].record1->value < recordCb[i * countB + j].record2->value){
//                 recordCb[i * countB + j].combinedValue = recordCb[i * countB + j].record1->value;
//             }
//             else{
//                 recordCb[i * countB + j].combinedValue = recordCb[i * countB + j].record2->value;
//             }
//         }
//     }

    return 0;
}
