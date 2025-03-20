#include <bits/time.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <float.h>

// Lembrende de como compilar
// gcc -Wall -Wextra -pedantic -O arquivo.c -o nome
//  Estrtura para salvar cada coordenada
struct Coordenada
{
  double x, y, z;
};

typedef struct Coordenada Coordenada;

// Função para calcular a distância euclidiana entre dois pontos
double calcularDistancia(Coordenada p1, Coordenada p2)
{
  double dx = p2.x - p1.x;
  double dy = p2.y - p1.y;
  double dz = p2.z - p1.z;
  return sqrt(dx * dx + dy * dy + dz * dz);
}

// Estrutura para armazenar um valor e seu índice original
typedef struct
{
  double valor;
  int indice;
} ValorIndice;

// Função para comparar dois ValorIndice (usado no qsort)
int comparar(const void *a, const void *b)
{
  ValorIndice *valorA = (ValorIndice *)a;
  ValorIndice *valorB = (ValorIndice *)b;
  if (valorA->valor < valorB->valor)
    return -1;
  if (valorA->valor > valorB->valor)
    return 1;
  return 0;
}

// Função para ordenar um array de doubles e salvar os índices originais
void quicksort_indices(double vetor[], int indices[], int n)
{
  // Criar um array auxiliar de ValorIndice
  ValorIndice *aux = (ValorIndice *)malloc(n * sizeof(ValorIndice));
  if (aux == NULL)
  {
    fprintf(stderr, "Erro ao alocar memoria\n");
    exit(1);
  }

  // Inicializar o array auxiliar com os valores e índices
  for (int i = 0; i < n; i++)
  {
    aux[i].valor = vetor[i];
    aux[i].indice = i;
  }

  // Ordenar o array auxiliar usando qsort
  qsort(aux, n, sizeof(ValorIndice), comparar);

  // Salvar os índices ordenados no array indices
  for (int i = 0; i < n; i++)
  {
    indices[i] = aux[i].indice;
  }

  // Liberar a memória do array auxiliar
  free(aux);
}

int main(int argc, char *argv[])
{
  struct timespec inicio, fim;
  double tempo_decorrido;

  if (argc > 3)
  {
    fprintf(stderr, "esperado dois valores de entrada\n");
    return 3;
  }
  // convertendo
  char *p;
  int m;
  // convertendo para int
  long conv = strtol(argv[1], &p, 10);

  m = conv;
  clock_gettime(CLOCK_MONOTONIC, &inicio);
  // verificando  o arquivo

  FILE *file = fopen(argv[2], "r");
  if (file == NULL)
  {
    fprintf(stderr, "nao pode abrir\n");
    return 1;
  }
  // vendo o tamanho do arquivo

  int numCoordenadas = 0;
  char ch;

  while ((ch = fgetc(file)) != EOF)
  {
    if (ch == '\n')
    {
      numCoordenadas++;
    }
  }

  rewind(file);
  // gerando o array com a estrutura

  struct Coordenada *coordenadas = malloc(numCoordenadas * sizeof(struct Coordenada));

  if (coordenadas == NULL)
  {
    fprintf(stderr, "Erro ao alocar memória\n");
    fclose(file);
    return 2;
  }
  // Lendo o arquivo
  for (int i = 0; i < numCoordenadas; i++)
  {
    int nread = fscanf(file, "%lf %lf %lf", &coordenadas[i].x, &coordenadas[i].y, &coordenadas[i].z);
    if (nread != 3)
    {
      fprintf(stderr, "não conseguiu ler\n");
      return 3;
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &fim);

  tempo_decorrido = (fim.tv_sec - inicio.tv_sec) +
                    (fim.tv_nsec - inicio.tv_nsec) / 1e9;

  printf("Tempo da leitura: %f segundos\n", tempo_decorrido);

  // Alocando memória para a matriz de distâncias

  double **distancias = (double **)malloc(numCoordenadas * sizeof(double *));
  for (int r = 0; r < numCoordenadas; ++r)
  {
    distancias[r] = (double *)malloc(numCoordenadas * sizeof(double));
  }
  // Calculando distâncias
  clock_gettime(CLOCK_MONOTONIC, &inicio);
  for (int i = 0; i < numCoordenadas; i++)
  {
    for (int j = i + 1; j < numCoordenadas; j++)
    {
      distancias[i][j] = calcularDistancia(coordenadas[i], coordenadas[j]);
      distancias[j][i] = distancias[i][j];
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &fim);

  tempo_decorrido = (fim.tv_sec - inicio.tv_sec) +
                    (fim.tv_nsec - inicio.tv_nsec) / 1e9;

  printf("Tempo do calculo: %f segundos\n", tempo_decorrido);
  // excluido valores da diagonal
  for (int i = 0; i < numCoordenadas; i++)
  {
    distancias[i][i] = DBL_MAX;
  }
  // matriz de vizinhos próximos
  int **vizinhos = (int **)malloc(numCoordenadas * sizeof(int *));
  for (int r = 0; r < numCoordenadas; ++r)
  {
    vizinhos[r] = (int *)malloc(m * sizeof(int));
  }
  strcat(argv[2], ".ngb");

  // criando o arquivo de saida
  FILE *arquivo = fopen(argv[2], "w");
  clock_gettime(CLOCK_MONOTONIC, &inicio);
  // algoritmo de ordenação
  for (int i = 0; i < numCoordenadas; i++)
  {
    int *indices = (int *)malloc(numCoordenadas * sizeof(int)); // criando o vetor de indices
    quicksort_indices(distancias[i], indices, numCoordenadas);
    for (int j = 0; j < m; ++j)
    { // passsando os dados para uma matriz
      vizinhos[i][j] = indices[j];
    }
    free(indices);
  }
  free(distancias);

  clock_gettime(CLOCK_MONOTONIC, &fim);

  tempo_decorrido = (fim.tv_sec - inicio.tv_sec) +
                    (fim.tv_nsec - inicio.tv_nsec) / 1e9;

  printf("Tempo ordenação das particulas: %f segundos\n", tempo_decorrido);

  clock_gettime(CLOCK_MONOTONIC, &inicio);
  // salvando em um arquivo
  for (int i = 0; i < numCoordenadas; ++i)
  {
    for (int k = 0; k < m; ++k)
    {
      // salvando em um arquivo
      int nchar = fprintf(arquivo, "%d ", vizinhos[i][k]);
      if (nchar < 0)
      {
        fprintf(stderr, "erro na saida do arquivo.\n");
        fclose(arquivo);
        return 2;
      }
    }
    fprintf(arquivo, "\n");
  }
  fclose(arquivo);

  clock_gettime(CLOCK_MONOTONIC, &fim);

  tempo_decorrido = (fim.tv_sec - inicio.tv_sec) +
                    (fim.tv_nsec - inicio.tv_nsec) / 1e9;

  printf("Tempo de escrita: %f segundos\n", tempo_decorrido);
  return 0;
}