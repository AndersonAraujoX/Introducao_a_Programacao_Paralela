#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Estrutura para representar a lista de adjacência
typedef struct
{
    int *tamanho_lista;
    int *indice;
    int **lista;
    int N;
} Grafo;

// Função para criar um grafo com lista de adjacência
Grafo *criarGrafo(int N, int k)
{
    // gerando o grafo
    Grafo *grafo = (Grafo *)malloc(sizeof(Grafo));
    grafo->N = N;
    // lista com tamanhos das listas
    grafo->tamanho_lista = (int *)malloc(N * sizeof(int));
    // salvando os indices
    grafo->indice = (int *)malloc(N * sizeof(int));
    // criando a lista de adjacencia

    grafo->lista = (int **)malloc(N * sizeof(int *));

    for (int i = 0; i < N; i++)
    {

        // criando vetor
        grafo->lista[i] = (int *)malloc(k * sizeof(int));

        // primeiro chute para a lista de adjcencia
        grafo->tamanho_lista[i] = k;

        // criando o primeiro ponteiro
        grafo->indice[i] = 0;

        for (int j = 0; j < k; j++)
        {
            grafo->lista[i][j] = -1;
        }
    }
    return grafo;
}

// Função para adicionar uma aresta ao grafo
void adicionarAresta(Grafo *grafo, int origem, int destino)
{

    // adicionando aresta
    grafo->lista[origem][grafo->indice[origem]] = destino;

    // verificando o tamanho
    if (grafo->indice[origem] == grafo->tamanho_lista[origem] - 1)
    {

        // dobrando o tamanho da lista de adjacencia
        grafo->tamanho_lista[origem] *= 2;
        grafo->lista[origem] = (int *)realloc(grafo->lista[origem], grafo->tamanho_lista[origem] * sizeof(int));

        for (int j = grafo->indice[origem] + 1; j < grafo->tamanho_lista[origem]; j++)
        {
            grafo->lista[origem][j] = -1;
        }
    }
    // alterando o ponteiro
    ++grafo->indice[origem];
}

// Função para liberar a memória alocada para o grafo
void liberarGrafo(Grafo *grafo)
{
    free(grafo->lista);
    free(grafo);
}

// Estrutura para representar uma fila com capacidade dinâmica
typedef struct
{
    int *itens;
    int frente, tras;
    int tamanho;
    int capacidade;
} Fila;

// Função para criar uma fila vazia com capacidade inicial
Fila *criarFila(int capacidade_inicial)
{
    Fila *q = (Fila *)malloc(sizeof(Fila));
    q->itens = (int *)malloc(capacidade_inicial * sizeof(int));
    q->frente = 0;
    q->tras = -1;
    q->tamanho = 0;
    q->capacidade = capacidade_inicial;
    return q;
}

int filaVazia(Fila *q)
{
    return q->tamanho == 0; // Função para verificar se a fila está vazia
}

// Função para enfileirar um elemento na fila,
void enfileirar(Fila *q, int valor)
{
    if (q->tamanho == q->capacidade)
    { // aumentando a capacidade se necessário
        q->capacidade *= 2;
        q->itens = (int *)realloc(q->itens, q->capacidade * sizeof(int));
    }
    q->tras = (q->tras + 1) % q->capacidade;
    q->itens[q->tras] = valor;
    q->tamanho++;
}

// Função para desenfileirar um elemento da fila
int desenfileirar(Fila *q)
{
    if (filaVazia(q))
    { // verificando se está vazia a lista
        fprintf(stderr, "Fila vazia!\n");
        exit(1);
    }
    int item = q->itens[q->frente]; // puxando a variavel
    q->frente = (q->frente + 1) % q->capacidade;
    q->tamanho--;
    return item;
}

void liberarFila(Fila *q)
{
    free(q->itens); // Função para liberar a memória alocada para a fila
    free(q);
}

// Função para realizar a busca em largura a partir de um vértice de origem
void buscaEmLargura(Grafo *grafo, int origem, int *distancia)
{
    int u, v;                                             // criando variaveis para auxiliar
    int *visitado = (int *)calloc(grafo->N, sizeof(int)); // criando vetor com os valores visitados

    Fila *q = criarFila(grafo->N); // criando fila com tamanho N

    visitado[origem] = 1;  // colocar o primeiro no como visitado
    distancia[origem] = 0; // colocando distancia 0 no primeiro no
    enfileirar(q, origem); // adcionionando na fila

    while (!filaVazia(q))
    { // parar while quando a fila tiver vazia

        u = desenfileirar(q);
        grafo->indice[u] = 0;                  // começar lista de vizinhos
        v = grafo->lista[u][grafo->indice[u]]; // inserir o primeiro vizinho

        while (v != -1)
        { // continuar até todos vizinhos sejam visitados

            if (visitado[v] == 0)
            {
                visitado[v] = 1;                 // colocar como vizitado
                distancia[v] = distancia[u] + 1; // calculo da distancia
                enfileirar(q, v);                // adicionar vizinho
            }

            ++grafo->indice[u]; // ir para o proximo vizinho
            v = grafo->lista[u][grafo->indice[u]];
        }
    }
    liberarFila(q); // liberar a fila e visitados
    free(visitado);
}

// Função para calcular a eficiência do grafo direcionado
double calcularEficiencia(Grafo *grafo)
{
    double eficiencia = 0;
    int *distancia = (int *)calloc(grafo->N, sizeof(int));

    for (int origem = 0; origem < grafo->N; origem++)
    {                                             // percorrer todos os nos
        buscaEmLargura(grafo, origem, distancia); // algoritmo de busca em largura
        for (int i = 0; i < grafo->N; i++)
        {
            if (i != origem && distancia[i] > 0)
            {
                eficiencia += 1.0 / distancia[i];
                distancia[i] = 0;
            }
        }
    }
    free(distancia);
    eficiencia = eficiencia / (grafo->N * (grafo->N - 1)); // calculo de eficiência
    return eficiencia;
}

int main(int argc, char *argv[])
{
    int N, M, u, v;
    struct timespec inicio, fim;
    double tempo_decorrido;
    // FILE *arquivo;

    if (argc != 2)
    { // verificando a entrada pelo terminal
        fprintf(stderr, "Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    FILE *arquivo = fopen(argv[1], "r");

    if (arquivo == NULL)
    { // verificando se consegue abrir o arquivo
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return 2;
    }

    int read1 = fscanf(arquivo, " %d", &N);
    int read2 = fscanf(arquivo, " %d", &M);

    if (read1 != 1)
    { // verificando se os parametros de arestas e nos
        fprintf(stderr, "Erro na leitura da leitura da quantidade de nó.\n");
        return 3;
    }
    if (read2 != 1)
    {
        fprintf(stderr, "Erro na leitura na leitura da quantidade de aresta.\n");
        return 4;
    }
    // Valor medio de vizinhos como tamanho das listas de adjacencia
    int k = (int)ceil(M / N);
    // criando o grafo usando lista de adjacencia
    Grafo *grafo = criarGrafo(N, k);
    // adicionando as arestas
    for (int i = 0; i < M; i++)
    {
        if (fscanf(arquivo, "%d %d", &u, &v) != 2)
        {
            fprintf(stderr, "Erro ao ler aresta.\n");
            return 1;
        }
        adicionarAresta(grafo, u - 1, v - 1); // Ajusta para índices baseados em 0
    }

    fclose(arquivo);
    // começando leitura do tempo
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    // calculo da eficiência
    double eficiencia = calcularEficiencia(grafo);
    // terminando a leitura do tempo
    clock_gettime(CLOCK_MONOTONIC, &fim);

    tempo_decorrido = (fim.tv_sec - inicio.tv_sec) +
                      (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    // imprindo resultado no terminal
    printf("Eficiência do grafo: %6f\n", eficiencia);

    printf("Tempo decorrido: %6f segundos\n", tempo_decorrido);

    liberarGrafo(grafo);

    return 0;
}