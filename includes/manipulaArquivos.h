#ifndef MANIPULA_ARQUIVOS_H
#define MANIPULA_ARQUIVOS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char **dados;
    int linhas;
    int colunas;
    int totalTesouros;
} Mapa;

/**
 * @brief Lê as dimensões e quantidade de tesouros da primeira linha do arquivo
 * @param arq Ponteiro para o arquivo aberto
 * @param linhas Ponteiro para armazenar o número de linhas
 * @param colunas Ponteiro para armazenar o número de colunas
 * @param totalTesouros Ponteiro para armazenar a quantidade total de tesouros
 * @return 1 em caso de sucesso, 0 em caso de erro
 */
int tamanhoMapa(FILE *arq, int *linhas, int *colunas, int *totalTesouros);

/**
 * @brief Exibe o mapa na tela
 * @param mapa Ponteiro para o array de strings que representa o mapa
 */
void mostrarMapa(char **mapa, int linhas);

/**
 * @brief Carrega o mapa do arquivo e o exibe na tela
 * @return Estrutura Mapa com dados, linhas e colunas
 */
Mapa carregaMapa(void);

/**
 * @brief Libera a memória alocada para o mapa
 * @param mapa Estrutura Mapa a ser liberada
 */
void liberaMapa(Mapa *mapa);

#endif // MANIPULA_ARQUIVOS_H
