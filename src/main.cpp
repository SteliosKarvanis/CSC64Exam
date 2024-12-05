#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <omp.h>
#include <algorithm>
#include <fstream>
#include "utils.h"

// Estrutura Record
// typedef struct {
//     int idIdx;     // Índice do ID no vetor `ids`
//     float value;   // Valor associado ao registro
// } Record;

// Funções auxiliares (assumindo que existam no "utils.h")
extern void loadA(const char *path, Record *records, int &count);
extern void loadB(const char *path, Record *records, int &count);
extern void load_ids(const char *path, char ids[][6]);

// Função para combinar IDs usando o operador "◦"
std::string combine_ids(const std::string &idA, const std::string &idB)
{
    return std::string() + idA[0] + idB[0] + idA[1] + idB[1] + idA[2];
}

// Função para carregar a base de dados
void load_database(const std::string &pathA, const std::string &pathB, const std::string &pathIDs,
                   std::vector<Record> &recordsA, std::vector<Record> &recordsB, std::vector<std::string> &ids)
{
    // Aloca memória para os dados brutos
    Record *rawRecordsA = (Record *)malloc(NUM_RECORDS * sizeof(Record));
    Record *rawRecordsB = (Record *)malloc(NUM_RECORDS * sizeof(Record));
    char(*rawIds)[6] = (char(*)[6])malloc(NUM_RECORDS * sizeof(*rawIds));

    if (!rawRecordsA || !rawRecordsB || !rawIds)
    {
        fprintf(stderr, "Erro ao alocar memória para a base de dados.\n");
        exit(1);
    }

    int countA = 0, countB = 0;
    loadA(pathA.c_str(), rawRecordsA, countA); // Carrega registros A
    loadB(pathB.c_str(), rawRecordsB, countB); // Carrega registros B
    load_ids(pathIDs.c_str(), rawIds);         // Carrega IDs alfabéticos

    // Salva os IDs em um vetor de strings
    for (int i = 0; i < NUM_RECORDS; ++i)
    {
        ids.push_back(std::string(rawIds[i])); // Converte para std::string
    }

    // Transfere registros de A e B para os vetores
    for (int i = 0; i < countA; ++i)
    {
        recordsA.push_back({rawRecordsA[i].idIdx, rawRecordsA[i].value});
    }

    for (int i = 0; i < countB; ++i)
    {
        recordsB.push_back({rawRecordsB[i].idIdx, rawRecordsB[i].value});
    }

    // Libera memória alocada dinamicamente
    free(rawRecordsA);
    free(rawRecordsB);
    free(rawIds);

    std::cout << "Base de dados carregada com sucesso!" << std::endl;
}

int main()
{
    // Inicializa os vetores e mapas necessários
    std::vector<Record> recordsA, recordsB;
    std::vector<std::string> ids;
    std::map<std::string, float> reducedA, reducedB;

    // Caminhos para os arquivos
    std::string pathA = "../db/A.txt";
    std::string pathB = "../db/B.txt";
    std::string pathIDs = "../db/ids.txt";

    // Carrega a base de dados
    load_database(pathA, pathB, pathIDs, recordsA, recordsB, ids);

    // Passo 1: Reduzir os IDs para A (posições 1, 3, 5) e filtrar por a_m > 0.25
#pragma omp parallel for
    for (size_t i = 0; i < recordsA.size(); ++i)
    {
        const std::string &id = ids[recordsA[i].idIdx];
        std::string reducedId = std::string() + id[0] + id[2] + id[4];
        float value = recordsA[i].value;

        if (value > 0.25)
        {
#pragma omp critical
            reducedA[reducedId] = std::max(reducedA[reducedId], value);
        }
    }

    // Passo 2: Reduzir os IDs para B (posições 2, 4) e filtrar por b_M < 0.75
#pragma omp parallel for
    for (size_t i = 0; i < recordsB.size(); ++i)
    {
        const std::string &id = ids[recordsB[i].idIdx];
        std::string reducedId = std::string() + id[1] + id[3];
        float value = recordsB[i].value;

        if (value < 0.75)
        {
#pragma omp critical
            reducedB[reducedId] = std::max(reducedB[reducedId], value);
        }
    }

    // Converter mapas reduzidos para vetores para compatibilidade com OpenMP
    std::vector<std::pair<std::string, float>> vectorReducedA(reducedA.begin(), reducedA.end());
    std::vector<std::pair<std::string, float>> vectorReducedB(reducedB.begin(), reducedB.end());

    // Passo 3: Combinar IDs reduzidos e calcular f
    std::vector<std::tuple<std::string, std::string, std::string, float, float, float>> results;

#pragma omp parallel for collapse(2)
    for (size_t i = 0; i < vectorReducedA.size(); ++i)
    {
        for (size_t j = 0; j < vectorReducedB.size(); ++j)
        {
            const auto &entryA = vectorReducedA[i];
            const auto &entryB = vectorReducedB[j];

            std::string combinedId = combine_ids(entryA.first, entryB.first);
            #pragma omp critical
            std::cout << combinedId << " " << entryA.first << " " << entryB.first << std::endl;
            auto it = std::find(ids.begin(), ids.end(), combinedId);

            if (it != ids.end())
            {
                int idx = std::distance(ids.begin(), it);
                float p = recordsA[idx].value * recordsB[idx].value;
                float f = entryA.second * entryB.second * p;

#pragma omp critical
                results.emplace_back(entryA.first, entryB.first, combinedId, entryA.second, entryB.second, f);
            }
        }
    }
    // Ordenar os resultados com base em f
    std::sort(results.begin(), results.end(), [](const auto &a, const auto &b)
              { return std::get<5>(a) < std::get<5>(b); });

    // Salvar resultados no arquivo CSV
    std::ofstream output("../sorted_output.csv");
    output << "ID_a_m,ID_b_M,ID',a_m,b_M,f\n";
    for (const auto &row : results)
    {
        output << std::get<0>(row) << "," << std::get<1>(row) << "," << std::get<2>(row) << ","
               << std::get<3>(row) << "," << std::get<4>(row) << "," << std::get<5>(row) << "\n";
    }

    std::cout << "Processamento completo. Resultados salvos em sorted_output.csv." << std::endl;
    return 0;
}
