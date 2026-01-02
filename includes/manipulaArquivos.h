#ifndef MANIPULA_ARQUIVOS_H
#define MANIPULA_ARQUIVOS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

// Limites do mapa
#define MAX_LINHAS 30
#define MAX_COLUNAS 30
#define MAX_FASES 10
#define MAX_RECORDES 100
#define MAX_NOME 50

// Estrutura para pares de portais
typedef struct {
    int x1, y1; // Coordenadas do primeiro portal
    int x2, y2; // Coordenadas do segundo portal
    char numero; // Número do portal ('1', '2', etc)
} ParPortal;

// Estrutura para cronômetro de speedrun
typedef struct {
    double inicioFase;
    double fimFase;
    double tempoFase;
    double inicioJogo;
    double tempoTotal;
    double temposPorFase[MAX_FASES];
    int faseAtual;
} Cronometro;

// Estrutura para recordes
typedef struct {
    char nomeJogador[MAX_NOME];
    double tempoTotal;
    double temposPorFase[MAX_FASES];
    char dataHora[50];
    int fasesCompletas;
} Recorde;

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
    ParPortal portais[9]; // Máximo 9 pares de portais ('1' a '9')
    int totalPortais;     // Quantidade de pares válidos
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

// Funções de Speedrun
/**
 * @brief Inicializa o cronômetro de speedrun
 * @param cronometro Ponteiro para a estrutura do cronômetro
 */
void iniciarCronometro(Cronometro *cronometro);

/**
 * @brief Inicia o cronômetro de uma nova fase
 * @param cronometro Ponteiro para a estrutura do cronômetro
 * @param numeroFase Número da fase que está iniciando
 */
void iniciarFase(Cronometro *cronometro, int numeroFase);

/**
 * @brief Finaliza o cronômetro da fase atual
 * @param cronometro Ponteiro para a estrutura do cronômetro
 */
void finalizarFase(Cronometro *cronometro);

/**
 * @brief Finaliza o jogo e calcula tempo total
 * @param cronometro Ponteiro para a estrutura do cronômetro
 */
void finalizarJogo(Cronometro *cronometro);

/**
 * @brief Solicita nome do jogador e salva o recorde
 * @param cronometro Cronômetro com os tempos do jogo
 * @param nomeJogador Nome personalizado do jogador
 */
void salvarRecorde(Cronometro *cronometro, const char *nomeJogador);

/**
 * @brief Carrega e exibe o ranking de recordes
 */
void exibirRanking(void);

/**
 * @brief Gera relatório completo de speedrun
 * @param cronometro Cronômetro com os tempos
 */
void gerarRelatorioSpeedrun(Cronometro *cronometro);

#endif // MANIPULA_ARQUIVOS_H
