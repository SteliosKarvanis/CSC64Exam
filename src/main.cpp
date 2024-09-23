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
    int countA, countB;
    loadA("../db/A.txt", recordsA, countA);
    loadB("../db/B.txt", recordsB, countB);
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
    // A Cross JOIN
#pragma omp parallel for
    for(int a1 = 0; a1 < countA - 1; a1++){
        for(int a2 = a1 + 1; a2 < countA; a2++){
            int cAId;
            float cAValue;
            if(recordsA[a1].value < recordsA[a2].value){
                cAValue = recordsA[a1].value;
                cAId = recordsA[a1].idIdx;
            }
            else{
                cAValue = recordsA[a2].value;
                cAId = recordsA[a2].idIdx;
            }
            // B Cross JOIN
#pragma omp parallel for
            for(int b1 = 0; b1 < countB - 1; b1++){
                for(int b2 = b1 + 1; b2 < countB; b2++){
                    int cBId;
                    float cBValue;
                    if(recordsB[b1].value > recordsB[b2].value){
                        cBValue = recordsB[b1].value;
                        cBId = recordsB[b1].idIdx;
                    }
                    else{
                        cBValue = recordsB[b2].value;
                        cBId = recordsB[b2].idIdx;
                    }
                    char* combinedId = combine_ids(ids[cAId], ids[cBId]);
                    float product = cAValue * cBValue;
                    auto it = idToIdx.find(combinedId);
                    if(it != idToIdx.end()){
                        product *= recordsA[it->second].value * recordsB[it->second].value;
#pragma omp critical
                        {
                            fprintf(output, "%s,%s,%s,%f,%f,%f\n", ids[a1], ids[b1],
                                    combinedId, cAValue, cBValue, product);
                            fprintf(output, "%s,%s,%s,%f,%f,%f\n", ids[a2], ids[b2],
                                    combinedId, cAValue, cBValue, product);
                            fprintf(output, "%s,%s,%s,%f,%f,%f\n", ids[a1], ids[b2],
                                    combinedId, cAValue, cBValue, product);
                            fprintf(output, "%s,%s,%s,%f,%f,%f\n", ids[a2], ids[b1],
                                    combinedId, cAValue, cBValue, product);
                        }
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
