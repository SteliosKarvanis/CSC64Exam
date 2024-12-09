#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <omp.h>
#include "utils.h"

std::string combine_ids(const std::string &idA, const std::string &idB)
{
    return std::string() + idA[0] + idB[0] + idA[1] + idB[1] + idA[2];
}

int main(){
    omp_set_nested(1);
    int sizeA = 0, sizeB = 0;
    Record* recordsA = (Record*)malloc(NUM_RECORDS * sizeof(Record));
    Record* recordsB = (Record*)malloc(NUM_RECORDS * sizeof(Record));
    char(*ids)[6] = (char(*)[6])malloc(NUM_RECORDS * sizeof(*ids));
    if(!recordsA || !recordsB || !ids){
        fprintf(stderr, "Erro ao alocar memória.\n");
        exit(1);
    }
    int countA, countB;
    sizeA = loadA("../db/A.txt", recordsA, countA);
    sizeB = loadB("../db/B.txt", recordsB, countB);
    load_ids("../db/ids.txt", ids); // Assuming ids are the same for both A and B

    std::unordered_map<std::string, int> idToIdx;
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

    fprintf(output, "ID_a_m,ID_b_M,ID',a_m,b_M,f,numTimes\n");

    std::vector<std::pair<std::string, std::pair<float, float>>> dataBase;
    dataBase.reserve(NUM_RECORDS);

    std::unordered_map<std::string, std::pair<float, float>> dataBaseFirstOnly;

    for(int i = 0; i < NUM_RECORDS; i++){
        dataBase.push_back(std::make_pair(ids[i], std::make_pair(recordsA[i].value, recordsB[i].value)));
        if (dataBaseFirstOnly.find(ids[i]) == dataBaseFirstOnly.end())
            dataBaseFirstOnly[ids[i]] = std::make_pair(recordsA[i].value, recordsB[i].value);
    }

    std::unordered_map<std::string, std::vector<int>> reducedA;
    std::unordered_map<std::string, std::vector<int>> reducedB;

    #pragma omp parallel for
    for (size_t i = 0; i < (size_t)sizeA; ++i)
    {
        const std::string &id = ids[i];
        std::string reducedId = std::string() + id[0] + id[2] + id[4];
        float value = recordsA[i].value;

        if (value > 0.25)
        {
            #pragma omp critical
            reducedA[reducedId].push_back(i);
        }
    }

    #pragma omp parallel for
    for (size_t i = 0; i < (size_t)sizeB; ++i)
    {
        const std::string &id = ids[i];
        std::string reducedId = std::string() + id[1] + id[3];
        float value = recordsB[i].value;

        if (value < 0.75)
        {
            #pragma omp critical
            reducedB[reducedId].push_back(i);
        }
    }

    std::vector<float> timesToRepeatA;
    timesToRepeatA.reserve(sizeA);
    for (size_t i = 0; i < (size_t)sizeA; ++i)
    {
        float value = recordsA[i].value;
        int times = 0;
        for (size_t j = 0; j < (size_t)sizeA; ++j)
        {
            if (i == j) continue;

            float value2 = recordsA[j].value;
            if (value < 0.25 || value2 < 0.25)
                continue;
            if (value < value2)
            {
                times++;
            }
        }
        timesToRepeatA.push_back(times);
    }

    std::vector<float> timesToRepeatB;
    timesToRepeatB.reserve(sizeB);
    for (size_t i = 0; i < (size_t)sizeB; ++i)
    {
        float value = recordsB[i].value;
        int times = 0;
        for (size_t j = 0; j < (size_t)sizeB; ++j)
        {
            if (i == j) continue;
            float value2 = recordsB[j].value;
            if (value > 0.75 || value2 > 0.75)
                continue;
            if (value > value2)
            {
                times++;
            }
        }
        timesToRepeatB.push_back(times);
    }

    // Prepare vectors for parallel iteration
    std::vector<std::pair<std::string, std::vector<int>>> vecA(reducedA.begin(), reducedA.end());
    std::vector<std::pair<std::string, std::vector<int>>> vecB(reducedB.begin(), reducedB.end());

    long long int total = 0;

    #pragma omp parallel for collapse(2) reduction(+:total)
    for (size_t iA = 0; iA < vecA.size(); iA++)
    {
        for (size_t iB = 0; iB < vecB.size(); iB++)
        {
            const std::string& idA = vecA[iA].first;
            const std::vector<int>& indicesA = vecA[iA].second;

            const std::string& idB = vecB[iB].first;
            const std::vector<int>& indicesB = vecB[iB].second;

            const std::string combinedId = combine_ids(idA, idB);
            if (dataBaseFirstOnly.find(combinedId) != dataBaseFirstOnly.end())
            {
                std::pair<float, float> pairAB = dataBaseFirstOnly[combinedId];
                float p = pairAB.first * pairAB.second;
                for (const auto& idxA : indicesA)
                {
                    for (const auto& idxB : indicesB)
                    {
                        float f = recordsA[idxA].value * recordsB[idxB].value * p;

                        int timesA = (int)timesToRepeatA[idxA];
                        int timesB = (int)timesToRepeatB[idxB];
                        long long int timesToRepeat = timesA * timesB * 4;
                        if (timesToRepeat < 1)
                        {
                            continue;
                        }
                        #pragma omp critical
                        {
                            fprintf(output, "%s,%s,%s,%f,%f,%f,%lld\n", ids[idxA], ids[idxB], combinedId.c_str(), recordsA[idxA].value, recordsB[idxB].value, f, timesToRepeat);
                            total+=timesToRepeat;
                        }
                    }
                }
            }
        }
    }

    fclose(output);
    free(recordsA);
    free(recordsB);
    free(ids);

    system("sort -t, -k6 -n ../output.csv -o ../sorted_output.csv");

    std::cout << "Processamento completo. Resultados salvos em output.csv." << std::endl << "Numero de linhas: " << total << std::endl;
    return 0;
}
