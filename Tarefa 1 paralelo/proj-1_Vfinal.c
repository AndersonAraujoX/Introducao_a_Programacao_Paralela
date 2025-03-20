#include <bits/time.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <float.h>
#include <mpi.h>

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
  return dx * dx + dy * dy + dz * dz;
}

void add_vizinho(int *lista, double *dista, double d, int i, int j, int m)
{
  // caso a ultima particula tem uma distancia menor que a nova não continuar
  if (d > dista[i * m + (m - 1)] && dista[i * m + (m - 1)] > 0)
    return;
  for (int k = 0; k < m; ++k)
  {
    if ((d < dista[i * m + k]) || (dista[i * m + k] == 0.0))
    {
      for (int l = m - 1; l > k; --l)
      { // deslocando os valores do vetor
        dista[i * m + l] = dista[i * m + (l - 1)];
        lista[i * m + l] = lista[i * m + (l - 1)];
      }
      dista[i * m + k] = d; // adicionando o valor
      lista[i * m + k] = j; // adicionando o indice
      return;
    }
  }
}

void particao(int N, int p, int m, int *contador, int *deslocamento)
{
  int q = ceil(N / p); // calculo para calcular o tamanho das matrizes
  int r = N % p;
  int corte = 0;
  for (int i = 0; i < p; i++)
  {
    contador[i] = i == (p - 1) ? (q + r) * m : q * m; // O ultimo processo terá mais elementos
    deslocamento[i] = corte;
    corte += contador[i];
  }
}

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  struct timespec inicio, fim;
  double tempo_decorrido1, tempo_decorrido2, tempo_decorrido3, tempo_decorrido4, distancia;

  if (argc > 3)
  {
    if (rank == 0)
    {
      fprintf(stderr, "esperado dois valores de entrada\n");
    }
    MPI_Finalize();
    return 3;
  }
  // convertendo
  char *p;
  int m;
  // convertendo para int
  long conv = strtol(argv[2], &p, 10);

  m = conv;

  int numCoordenadas = 0;
  struct Coordenada *coordenadas = NULL;

  if (rank == 0)
  {
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    // verificando  o arquivo

    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
      fprintf(stderr, "nao pode abrir\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
    // vendo o tamanho do arquivo

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

    coordenadas = malloc(numCoordenadas * sizeof(struct Coordenada));

    if (coordenadas == NULL)
    {
      fprintf(stderr, "Erro ao alocar memória\n");
      fclose(file);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
    // Lendo o arquivo
    if (rank == 0)
    {
      for (int i = 0; i < numCoordenadas; i++)
      {
        int nread = fscanf(file, "%lf %lf %lf", &coordenadas[i].x, &coordenadas[i].y, &coordenadas[i].z);
        if (nread != 3)
        {
          fprintf(stderr, "não conseguiu ler\n");
          MPI_Abort(MPI_COMM_WORLD, 1);
        }
      }
    }
    fclose(file);
    clock_gettime(CLOCK_MONOTONIC, &fim);

    tempo_decorrido1 = (fim.tv_sec - inicio.tv_sec) +
                       (fim.tv_nsec - inicio.tv_nsec) / 1e9;
  }

  // Transmite o número de coordenadas para todos os processos
  MPI_Bcast(&numCoordenadas, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Aloca memória para coordenadas em todos os processos
  if (rank != 0)
  {
    coordenadas = malloc(numCoordenadas * sizeof(struct Coordenada));
    if (coordenadas == NULL)
    {
      fprintf(stderr, "Erro ao alocar memória\n");
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &inicio);
  // Transmite as coordenadas para todos os processos
  MPI_Bcast(coordenadas, numCoordenadas * sizeof(struct Coordenada), MPI_BYTE, 0, MPI_COMM_WORLD);

  // Calcula o número de linhas que cada processo irá processar
  int linhas_por_processo = (rank == size - 1) ? (numCoordenadas / size + numCoordenadas % size) : numCoordenadas / size;
  int inicio_linha = rank * floor(numCoordenadas / size);
  int fim_linha = (rank == size - 1) ? numCoordenadas : (rank + 1) * linhas_por_processo;

  double *distancias = (double *)calloc(linhas_por_processo * m, sizeof(double));
  int *indices = (int *)malloc(linhas_por_processo * m * sizeof(int));// Utilizando um vetor de indice para transmitir menos dados quando for utilizar o gather
  // Calculando distâncias

  for (int i = inicio_linha; i < fim_linha; i++)
  {
    for (int j = 0; j < numCoordenadas; j++)
    {
      if (i != j)
      {
        distancia = calcularDistancia(coordenadas[i], coordenadas[j]); // excluido valores da diagonal
        add_vizinho(indices, distancias, distancia, i - inicio_linha, j, m);
      }
    }
  }

  free(coordenadas);

  clock_gettime(CLOCK_MONOTONIC, &fim);

  tempo_decorrido2 = (fim.tv_sec - inicio.tv_sec) +
                     (fim.tv_nsec - inicio.tv_nsec) / 1e9;

  clock_gettime(CLOCK_MONOTONIC, &inicio);
  // Reunir os resultados de todos os processos no processo 0
  int *vizinhos = NULL;
  if (rank == 0)
  {
    vizinhos = (int *)malloc(numCoordenadas * m * sizeof(int));
  }

  // vetores auxiliares
  int *contador = (int *)malloc(size * sizeof(int));
  int *deslocamento = (int *)malloc(size * sizeof(int));
  particao(numCoordenadas, size, m, contador, deslocamento);

  // Comunicação para quando tamanho dos vetores não são distribuidos
  MPI_Gatherv(indices, contador[rank], MPI_INT, vizinhos, contador, deslocamento, MPI_INT, 0, MPI_COMM_WORLD);
  free(indices);
  clock_gettime(CLOCK_MONOTONIC, &fim);

  tempo_decorrido3 = (fim.tv_sec - inicio.tv_sec) +
                     (fim.tv_nsec - inicio.tv_nsec) / 1e9;
  if (rank == 0)
  {

    char *arquivo_saida = strcat(argv[1], ".ngb");
    // criando o arquivo de saida
    FILE *arquivo = fopen(arquivo_saida, "w");

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    // salvando em um arquivo
    for (int i = 0; i < numCoordenadas; i++)
    {
      for (int k = 0; k < m; k++)
      {
        // salvando em um arquivo
        int nchar = fprintf(arquivo, "%d ", vizinhos[i * m + k]);
        if (nchar < 0)
        {
          fprintf(stderr, "erro na saida do arquivo.\n");
          fclose(arquivo);
          MPI_Abort(MPI_COMM_WORLD, 1);
        }
      }
      fprintf(arquivo, "\n");
    }
    fclose(arquivo);

    clock_gettime(CLOCK_MONOTONIC, &fim);

    tempo_decorrido4 = (fim.tv_sec - inicio.tv_sec) +
                       (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Tempo de Leitura: %f\n", tempo_decorrido1);
    printf("Tempo de Calculo/Ordenação:%f\n", tempo_decorrido2);
    printf("Tempo de Comunicação: %f\n", tempo_decorrido3);
    printf("Tempo de Escrita: %f\n", tempo_decorrido4);
  }
  free(distancias);
  free(vizinhos);

  MPI_Finalize();
  return 0;
}