#ifndef MANIPULA_ARQUIVOS_H
#define MANIPULA_ARQUIVOS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Forward declaration para evitar dependência circular
typedef struct {
    int x;
    int y;
    float velocidade;
    double ultimoMovimento;
} Inimigo;

typedef struct {
    char **dados;
    int linhas;
    int colunas;
    int totalTesouros;
    int totalInimigos;
} Mapa;

/**
 * @brief Lê as dimensões da primeira linha do arquivo
 * @param arq Ponteiro para o arquivo aberto
 * @param linhas Ponteiro para armazenar o número de linhas
 * @param colunas Ponteiro para armazenar o número de colunas
 * @return 1 em caso de sucesso, 0 em caso de erro
 */
int tamanhoMapa(FILE *arq, int *linhas, int *colunas);

/**
 * @brief Verifica se um arquivo existe no sistema
 * @param caminho Caminho para o arquivo a ser verificado
 * @return true se o arquivo existe, false caso contrário
 */
bool arquivoExiste(const char *caminho);

/**
 * @brief Exibe o mapa na tela
 * @param mapa Ponteiro para o array de strings que representa o mapa
 */
void mostrarMapa(char **mapa, int linhas);

/**
 * @brief Carrega o mapa do arquivo e o exibe na tela
 * @return Estrutura Mapa com dados, linhas e colunas
 */
Mapa carregaMapa(int fase, Inimigo **inimigos);

/**
 * @brief Libera a memória alocada para o mapa
 * @param mapa Estrutura Mapa a ser liberada
 */
void liberaMapa(Mapa *mapa);

/**
 * @brief Lista mapas disponíveis no diretório mapas/
 */
void listarMapasDisponiveis(void);

#endif // MANIPULA_ARQUIVOS_H
