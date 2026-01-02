/**
 * @file desenhos.h
 * @author Vitor Viana (vitorluizcviana@gmail.com)
 * @brief Declarações de todas as funções de renderização visual do jogo
 * @version 0.1
 * @date 2026-01-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef DESENHOS_H
#define DESENHOS_H

#include "raylib.h"
#include "manipulaArquivos.h"
#include "inimigo.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

// ===== FUNÇÕES DE DESENHO DO JOGO =====

/**
 * @brief Desenha o mapa completo do jogo
 * @param mapa Estrutura do mapa a ser desenhado
 */
void desenhaMapa(Mapa mapa);

/**
 * @brief Desenha os portais na posição do jogador
 * @param mapa Estrutura do mapa
 * @param jogador Estrutura do jogador
 */
void desenhaPortais(Mapa mapa, Jogador jogador);

/**
 * @brief Desenha todos os inimigos no mapa
 * @param mapa Estrutura do mapa
 * @param inimigos Array de inimigos
 */
void desenhaInimigos(Mapa mapa, Inimigo *inimigos);

/**
 * @brief Desenha o jogador com efeitos visuais (invencibilidade)
 * @param jogador Estrutura do jogador
 * @param mapa Estrutura do mapa
 */
void desenhaJogadorComEfeito(Jogador jogador, Mapa mapa);

/**
 * @brief Desenha as informações da HUD durante o jogo
 * @param fase Fase atual
 * @param tesouroColetados Tesouros coletados
 * @param mapa Estrutura do mapa
 * @param jogador Estrutura do jogador
 * @param cronometro Cronômetro do jogo
 */
void desenhaHUDJogo(int fase, int tesouroColetados, Mapa mapa, Jogador jogador, Cronometro *cronometro);

// ===== TELAS DO SISTEMA DE SPEEDRUN =====

/**
 * @brief Desenha a tela de resultados finais
 * @param cronometro Cronômetro com dados do jogo
 */
void desenhaTelaResultados(Cronometro *cronometro);

/**
 * @brief Desenha a tela de entrada de nome personalizado
 * @param nomeJogador String do nome sendo digitado
 * @param cronometro Cronômetro com dados do jogo
 */
void desenhaTelaInputNome(char *nomeJogador, Cronometro *cronometro);

/**
 * @brief Desenha o ranking de melhores tempos
 */
void desenhaRanking(void);

// ===== TELAS DE STATUS DO JOGO =====

/**
 * @brief Desenha a tela de jogo completo
 * @param cronometro Cronômetro com dados finais
 */
void desenhaTelaJogoCompleto(Cronometro cronometro);

/**
 * @brief Desenha a tela de transição entre fases
 */
void desenhaTelaEntreFases(void);

/**
 * @brief Desenha a tela de game over
 */
void desenhaTelaGameOver(void);

#endif // DESENHOS_H