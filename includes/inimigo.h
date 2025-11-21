/**
 * @file inimigo.h
 * @author Vitor Viana (vitorluizcviana@gmail.com)
 * @brief Sistema de inimigos para o jogo Planeta do Tesouro
 * @version 0.1
 * @date 2025-11-21
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef INIMIGO_H
#define INIMIGO_H

#include "manipulaArquivos.h"
#include "logicaJogo.h"

// A estrutura Inimigo está definida em manipulaArquivos.h

/**
 * @brief Move o inimigo em direção ao jogador
 * @param jogador Ponteiro para a estrutura do jogador
 * @param inimigo Ponteiro para a estrutura do inimigo a ser movido
 * @param mapa Ponteiro para o mapa do jogo (para verificar colisões)
 */
void andaInimigo(Jogador *jogador, Inimigo *inimigo, Mapa *mapa);

#endif // INIMIGO_H