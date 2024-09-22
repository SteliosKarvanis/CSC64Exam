# Proposta de Implementação e Otimização

## Introdução

Este documento detalha as ideias de implementação e otimização de uma aplicação que processa uma base de dados simulada contendo registros de IDs alfabéticos e dois valores aleatórios. O objetivo principal é otimizar o tempo de execução (runtime) da aplicação, mantendo a lógica correta e os resultados esperados.

A solução começa com uma implementação sequencial simples, cujo código foi fornecido (`seq.c`). A partir disso, propomos uma série de melhorias focadas em desempenho, como paralelização e uso de estruturas de dados mais eficientes.

## Objetivos

- Otimizar o tempo de execução da aplicação.
- Realizar operações de filtragem e cálculo de produtos entre registros de duas bases de dados.
- Organizar a saída final em um arquivo CSV ordenado.

## Ideias para Implementação

### 1. Estrutura Atual
O código fornecido `seq.c` realiza as seguintes operações de maneira sequencial:
- **Leitura dos registros**: Lê valores aleatórios e IDs de arquivos em disco.
- **Filtragem**: Filtra registros de acordo com os limites fornecidos para os valores mínimos de uma base (\(Ca\)) e máximos de outra base (\(Cb\)).
- **Combinação de IDs e cálculo de produto**: Combina IDs dos registros e calcula produtos envolvendo valores de duas bases.
- **Ordenação e saída**: Gera um arquivo CSV e utiliza o comando `sort` para ordenar o arquivo final com base nos valores da última coluna.

### 2. Otimizações Propostas

#### 2.1. Paralelização do Processamento
Grande parte do código pode ser paralelizada, já que as operações em cada registro são independentes. A paralelização será implementada usando **OpenMP** ou **pthreads**:
- **Leitura dos dados**: A leitura de arquivos pode ser feita em paralelo, com diferentes threads lendo pedaços do arquivo e carregando os registros em memória.
- **Filtragem**: Tanto a filtragem de \(Ca\) (valores mínimos) quanto de \(Cb\) (valores máximos) pode ser paralelizada, processando blocos de registros simultaneamente.
- **Cálculo de produtos**: As combinações de IDs e os cálculos de produtos podem ser executados em threads separadas, permitindo uma otimização adicional.

#### 2.2. Uso de Estruturas de Dados Eficientes
Atualmente, o código percorre todos os registros em busca de combinações de IDs. Isso pode ser otimizado com o uso de estruturas de dados mais eficientes:
- **Hash tables**: Utilizar tabelas hash para armazenar e buscar rapidamente os IDs já lidos. Em vez de percorrer linearmente a lista de IDs para encontrar uma combinação, o uso de uma tabela hash pode reduzir o tempo de busca de O(n) para O(1).
- **Árvores balanceadas**: Outra abordagem seria utilizar árvores balanceadas (como árvores AVL ou Red-Black) para armazenar e buscar os registros, garantindo uma busca logarítmica O(log n).

#### 2.3. Otimização de Entrada e Saída
A leitura e escrita em arquivos pode se tornar um gargalo, especialmente com grandes volumes de dados. Propostas de otimização incluem:
- **Leitura e escrita em blocos**: Em vez de realizar operações de leitura/escrita registro por registro, podemos carregar blocos de dados em memória de uma só vez, o que pode reduzir o overhead de I/O.
- **Uso de formatos binários**: Os arquivos de dados podem ser armazenados em formatos binários, em vez de texto. Isso reduzirá o tempo de leitura/escrita e o tamanho dos arquivos, acelerando o processo.

#### 2.4. Ordenação Externa
O arquivo gerado pode ser muito grande para ser ordenado em memória. Propomos a implementação de uma **ordenação externa** mais eficiente, como o algoritmo **External Merge Sort**:
- Divide o arquivo de saída em blocos menores que cabem em memória, ordena cada bloco individualmente e, em seguida, faz a fusão dos blocos ordenados.

#### 2.5. Redução de Operações Repetitivas
Atualmente, o código recalcula produtos e combinações para registros já processados. Uma otimização seria:
- **Cache dos resultados intermediários**: Armazenar os resultados de combinações e produtos já computados em uma tabela hash para evitar recalculá-los.

### 3. Organização dos Arquivos

A aplicação seguirá uma estrutura modular organizada em diretórios:

/projeto

    │ ├── db/ # Base de dados
        │ ├── db.c # Geração da base de dados 
        │ ├── A.txt 
        │ ├── B.txt 
        │ └── ids.txt 
    │ ├── src/ # Código-fonte 
        │ ├── seq.c # Implementação sequencial 
        │ └── parallel.c # Implementação paralelizada 
    │ ├── doc/ # Documentação 
        │ ├── projeto.md # Descrição do projeto
        │ └── relatorio.md # Relatório final 
    │ ├── tutorial/ # Passos para instalação e execução 
        │ └── tutorial.md 
    │ └── README.md # Descrição geral do repositório


### 4. Ferramentas e Dependências

A implementação será feita em **C**, utilizando as seguintes bibliotecas:
- **OpenMP**: Para paralelização (ou alternativamente, pthreads).
- **GNU `sort`**: Para a ordenação externa temporária (pode ser substituído por uma ordenação interna se possível).
- **Libs padrão de C**: `stdio.h`, `stdlib.h`, `string.h`.

### 5. Cronograma

O desenvolvimento será dividido em fases:
1. **Fase 1**: Implementação da versão sequencial (conforme fornecido no código `seq.c`).
2. **Fase 2**: Introdução de paralelização e teste de escalabilidade.
3. **Fase 3**: Implementação de melhorias de I/O e uso de estruturas de dados eficientes.
4. **Fase 4**: Implementação da ordenação externa e refinamento final.

### 6. Conclusão

A solução proposta visa otimizar o tempo de execução da aplicação através de paralelização, uso de estruturas de dados eficientes e melhorias de entrada/saída. O foco está em garantir que as operações críticas sejam executadas de forma eficiente, reduzindo o tempo total de processamento sem comprometer a precisão dos resultados.

Aguardamos sugestões e feedback sobre as abordagens apresentadas.
