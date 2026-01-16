/**
 * @file audio.h
 * @author Vitor Viana (vitorluizcviana@gmail.com)
 * @brief Declarações do sistema de áudio do jogo
 * @version 0.1
 * @date 2026-01-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"
#include <stdbool.h>

// === FUNÇÕES DE INICIALIZAÇÃO E GERENCIAMENTO ===

/**
 * @brief Inicializa o sistema de áudio
 */
void inicializarSistemaAudio(void);

/**
 * @brief Carrega todos os recursos de áudio (sons e músicas)
 */
void carregarRecursosAudio(void);

/**
 * @brief Libera todos os recursos de áudio e fecha o sistema
 */
void liberarRecursosAudio(void);

/**
 * @brief Atualiza o sistema de música (deve ser chamado no loop principal)
 */
void atualizarMusica(void);

/**
 * @brief Verifica se o sistema de áudio está pronto para uso
 * @return bool true se o sistema está funcionando, false caso contrário
 */
bool sistemaAudioPronto(void);

// === FUNÇÕES DE EFEITOS SONOROS ===

/**
 * @brief Toca o som de dano quando o jogador é atingido
 */
void tocarSomDano(void);

/**
 * @brief Toca o som de coleta de tesouro
 */
void tocarSomTesouro(void);

/**
 * @brief Toca o som de cura/vida
 */
void tocarSomCura(void);

/**
 * @brief Toca o som de game over quando o jogador morre
 */
void tocarSomGameOver(void);

/**
 * @brief Toca o som de início do jogo
 */
void tocarSomStart(void);

/**
 * @brief Toca o som de teletransporte
 */
void tocarSomTeleporte(void);

// === FUNÇÕES DE MÚSICA ===

/**
 * @brief Inicia a música de fundo do jogo
 */
void iniciarMusicaPrincipal(void);

/**
 * @brief Para a música de fundo do jogo
 */
void pararMusicaPrincipal(void);

/**
 * @brief Inicia a música do menu
 */
void iniciarMusicaMenu(void);

/**
 * @brief Para a música do menu
 */
void pararMusicaMenu(void);

#endif // AUDIO_H