#include "utils.h"

#include <omp.h>
#include <map>
#include <string>

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

    std::map<std::string, int> idToIdx = std::map<std::string, int>();
    for(int i = 0; i < NUM_RECORDS; i++){
        idToIdx[(std::string)ids[i]] = i;
    }

    FILE* output = fopen("../output.csv", "w");
    if(!output){
        fprintf(stderr, "Erro ao abrir o arquivo de saída.\n");
        free(recordsA);
        free(recordsB);
        free(ids);
        return 1;
    }

    // PROCESS
    float cAValue;
    float cBValue;
    char* cAId;
    char* cBId;
    char* combinedId;
    float product;
    // A Cross JOIN
    for(int a1 = 0; a1 < NUM_RECORDS; a1++){
        if(recordsA[a1].value <= THRESHOLD_CA_MIN) continue;
        for(int a2 = 0; a2 < NUM_RECORDS; a2++){
            if(a1 == a2 || recordsA[a2].value <= THRESHOLD_CA_MIN) continue;
            if(recordsA[a1].value < recordsA[a2].value){
                cAValue = recordsA[a1].value;
                cAId = recordsA[a1].id;
            }
            else{
                cAValue = recordsA[a2].value;
                cAId = recordsA[a2].id;
            }
            // B Cross JOIN
            for(int b1 = 0; b1 < NUM_RECORDS; b1++){
                if(recordsB[b1].value >= THRESHOLD_CB_MAX) continue;
                for(int b2 = 0; b2 < NUM_RECORDS; b2++){
                    if(b1 == b2 || recordsB[b2].value >= THRESHOLD_CB_MAX) continue;
                    if(recordsB[b1].value > recordsB[b2].value){
                        cBValue = recordsB[b1].value;
                        cBId = recordsB[b1].id;
                    }
                    else{
                        cBValue = recordsB[b2].value;
                        cBId = recordsB[b2].id;
                    }
                    combinedId = combine_ids(cAId, cBId);
                    product = cAValue * cBValue;
                    auto it = idToIdx.find(combinedId);
                    if(it != idToIdx.end())
                        product *= recordsA[it->second].value * recordsB[it->second].value;
                    else
                        continue;
                    {
                        fprintf(output, "%s,%s,%s,%f,%f,%f\n", ids[a1], ids[b1],
                                combinedId, cAValue, cBValue, product);
                    }
                }
            }
        }
    }
    // Sort
    // Ordena os registros com base no valor da coluna f
    // NOTE: This is a simplified approach; for large datasets, external sorting
    // would be more appropriate
    system("sort -t, -k6 -n ../output.csv -o ../sorted_output.csv");
    // Descrição do comando:
    // sort: O comando para ordenar.
    // -t,: Define a vírgula (,) como delimitador de campo.
    // -k6: Especifica que a ordenação deve ser feita com base no sexto campo.
    // -n: Realiza uma ordenação numérica.
    // output.csv: O arquivo de entrada.
    // -o sorted_output.csv: Especifica o arquivo de saída.

    // Libera a memória alocada dinamicamente
    free(recordsA);
    free(recordsB);
    free(ids);

    printf("Processamento completo. Resultados salvos em sorted_output.csv.\n");
    return 0;
}
