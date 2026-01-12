#ifndef LOGICA_JOGO_H
#define LOGICA_JOGO_H

#include <stdbool.h>
#include "raylib.h"
#include "manipulaArquivos.h"

// Estados do jogo
enum {MENU = 0, JOGANDO, JOGO_COMPLETO, ENTRE_FASES, GAME_OVER, TELA_RESULTADOS, TELA_INPUT_NOME, TELA_RANKING, TUTORIAL};

// Estrutura do jogador
typedef struct {
    int x;
    int y;
    int vidas;
    double tempoInvencibilidade;
} Jogador;

/**
 * @brief Encontra a posição inicial do jogador no mapa
 * @param mapa Mapa onde procurar o jogador
 * @return Posição do jogador (estrutura Jogador)
 */
Jogador encontrarJogador(Mapa mapa, int vidas);

/**
 * @brief Verifica se há um tesouro na posição e o coleta
 * @param mapa Ponteiro para o mapa
 * @param x Coordenada X (coluna)
 * @param y Coordenada Y (linha)
 * @return true se coletou um tesouro, false caso contrário
 */
bool confereTesouro(Mapa *mapa, int x, int y);

/**
 * @brief Avança para a próxima fase do jogo
 * @param mapa Ponteiro para o mapa atual
 * @param jogador Ponteiro para o jogador
 * @param statusJogo Ponteiro para o status do jogo
 * @param tesouroColetados Ponteiro para contador de tesouros
 * @param fase Ponteiro para número da fase
 * @param inimigo Ponteiro duplo para array de inimigos
 * @param cronometro Ponteiro para o cronômetro de speedrun
 */
void proximaFase(Mapa *mapa, Jogador *jogador, int *statusJogo, int *tesouroColetados, int *fase, Inimigo **inimigo, Cronometro *cronometro);

/**
 * @brief Verifica se o jogador perdeu todas as vidas
 * @param jogador Ponteiro para o jogador
 * @param statusJogo Ponteiro para o status do jogo
 */
void verificaGameOver(Jogador *jogador, int *statusJogo);

/**
 * @brief Verifica colisão entre jogador e inimigos
 * @param jogador Ponteiro para o jogador
 * @param inimigos Array de inimigos
 * @param mapa Ponteiro para o mapa
 * @param statusJogo Ponteiro para o status do jogo
 */
void verificaColisaoComInimigos(Jogador *jogador, Inimigo *inimigos, Mapa *mapa, int *statusJogo);

/**
 * @brief Processa movimento do jogador e lógica de coleta
 * @param jogador Ponteiro para o jogador
 * @param mapa Ponteiro para o mapa
 * @param statusJogo Ponteiro para o status do jogo
 * @param tesouroColetados Ponteiro para contador de tesouros
 */
void movePersonagem(Jogador *jogador, Mapa *mapa, int *statusJogo, int *tesouroColetados);

/**
 * @brief Verifica se uma posição deveria ter um portal
 * @param mapa Ponteiro para o mapa
 * @param x Coordenada X
 * @param y Coordenada Y
 * @return Número do portal ou '.' se não for portal
 */
char verificaPortalNaPosicao(Mapa *mapa, int x, int y);

/**
 * @brief Verifica se o jogador está em um portal numerado e o teleporta
 * @param jogador Ponteiro para o jogador
 * @param mapa Ponteiro para o mapa
 */
void verificaPortalNumerado(Jogador *jogador, Mapa *mapa, int y, int x);

#endif // LOGICA_JOGO_H