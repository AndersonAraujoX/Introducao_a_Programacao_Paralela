#  Implementações de Algoritmos de Alto Desempenho

Este repositório contém implementações de algoritmos de alto desempenho, desenvolvidas como parte dos trabalhos práticos da disciplina de Introdução à Programação Paralela. Os projetos abordam diferentes problemas, com versões sequenciais e paralelas, buscando otimizar o desempenho através de diferentes técnicas e estruturas de dados.

##  Projeto A: Encontrando os M Vizinhos Mais Próximos

###  Descrição do Problema

Dado um conjunto de N partículas em um espaço tridimensional, o objetivo é encontrar os M vizinhos mais próximos de cada partícula, utilizando a distância euclidiana. 

###  Implementação Sequencial

A implementação sequencial lê um arquivo de entrada contendo as posições das partículas, calcula as distâncias entre elas, ordena as partículas por proximidade e grava o resultado em um arquivo de saída. 
O código foi estruturado em quatro etapas principais:

1.  Leitura do arquivo e armazenamento das coordenadas das partículas.
   
2.  Cálculo das distâncias entre as partículas.
   
3.  Ordenação dos vizinhos mais próximos utilizando o algoritmo Quicksort para maior eficiência.
   
4.  Escrita dos resultados em um arquivo de saída.
5.  
A escolha do Quicksort se mostrou mais eficiente em comparação com o Select Sort, especialmente para grandes conjuntos de dados.  A otimização do cálculo das distâncias e o uso de alocação dinâmica de memória também contribuíram para o desempenho do código.

###  Implementação Paralela

A versão paralela do projeto A utiliza a biblioteca MPI para distribuir o cálculo das distâncias entre os processos. As etapas da implementação paralela são:

1.  Leitura do arquivo e distribuição das coordenadas para os processos.
   
2.  Cálculo paralelo das distâncias.
   
3.  Ordenação dos vizinhos mais próximos. 
   
4.  Coleta dos resultados no processo raiz e escrita do arquivo de saída. 

A comunicação entre os processos é realizada utilizando `MPI_Bcast` e `MPI_Gatherv` para distribuir as coordenadas e coletar os resultados, respectivamente.  A complexidade do código paralelo é da ordem de O(N²M), com a etapa de cálculo das distâncias sendo a mais custosa. A não utilização da simetria no cálculo das distâncias ($d_{ij} = d_{ji}$) foi justificada pela redução na complexidade de tempo. 

Os resultados experimentais demonstraram que, para um pequeno número de partículas, o aumento no número de processos pode aumentar o custo computacional devido à comunicação.  No entanto, para um grande número de partículas, a paralelização resulta em um ganho de desempenho significativo.

##  Projeto B: Calculando a Eficiência de um Grafo

###  Descrição do Problema

Dado um grafo direcionado e não ponderado, o problema consiste em calcular a sua eficiência, definida em termos das distâncias entre os vértices do grafo.

###  Implementação Sequencial

A implementação sequencial lê um arquivo de entrada que descreve o grafo, calcula a eficiência do grafo e imprime o resultado na tela. 
O código utiliza uma lista de adjacência para representar o grafo e o algoritmo de busca em largura (BFS) para calcular as distâncias entre os vértices. 
As principais etapas são:

1.  Leitura do arquivo e criação do grafo utilizando lista de adjacência.
   
2.  Cálculo da eficiência do grafo utilizando BFS para encontrar as distâncias entre os vértices.
3.  
4.  Impressão do resultado e do tempo de execução. 

A escolha da lista de adjacência em vez da matriz de adjacência foi motivada pela redução na complexidade de tempo, de O(N²) para O(N), na verificação de vizinhança entre os nós. 
Os resultados experimentais demonstraram a eficiência da lista de adjacência em comparação com a matriz de adjacência, especialmente para grafos esparsos.

###  Implementação Paralela

A implementação paralela do Projeto B também utiliza MPI para distribuir o cálculo da eficiência do grafo entre os processos. 
As etapas da implementação paralela são:

1. Leitura do arquivo e distribuição das informações do grafo para os processos, incluindo o número de nós, arestas e a lista de adjacência.
2. A comunicação é realizada utilizando `MPI_Bcast`. 
   
4.  Cálculo paralelo da eficiência do grafo.
5. Cada processo calcula a eficiência para um subconjunto de nós utilizando o algoritmo BFS. 
   
6.  Coleta dos resultados parciais e cálculo da eficiência global no processo raiz. 
7.   A função `MPI_Reduce` é utilizada para somar as eficiências calculadas por cada processo.
   
8.  Impressão do resultado e do tempo de execução. 

A paralelização permitiu reduzir a complexidade do algoritmo de O(NM) para O(NM/p), onde p é o número de processos.  Os resultados experimentais demonstraram que a implementação paralela apresenta um bom desempenho, especialmente para grafos com um grande número de nós e arestas.

##  Considerações Finais

As implementações apresentadas neste repositório exploram diferentes técnicas de programação sequencial e paralela para resolver problemas de alto desempenho. A escolha de estruturas de dados e algoritmos adequados, juntamente com a utilização de recursos de paralelização, são fundamentais para otimizar o desempenho e obter resultados eficientes.
