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

// Estrutura para representar um botão do menu
typedef struct {
    Rectangle bounds;    // Área do botão (posição e tamanho)
    const char* texto;   // Texto exibido no botão
    bool hover;          // Se o mouse está sobre o botão
} BotaoMenu;

// Enumeração para ações dos botões do menu
typedef enum {
    ACAO_NENHUMA = 0,
    ACAO_INICIAR,
    ACAO_RANKING,
    ACAO_TUTORIAL
} AcaoBotaoMenu;

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

/**
 * @brief Desenha a tela do menu principal
 */
void desenhaTelaMenu(void);

/**
 * @brief Desenha um botão com estilo madeira e borda dourada
 * @param botao Estrutura do botão a ser desenhado
 */
void desenhaBotaoMenu(BotaoMenu* botao);

/**
 * @brief Atualiza o estado de hover de um botão baseado na posição do mouse
 * @param botao Ponteiro para o botão a ser atualizado
 */
void atualizaHoverBotao(BotaoMenu* botao);

/**
 * @brief Verifica se algum botão do menu foi clicado e retorna a ação correspondente
 * @return AcaoBotaoMenu A ação do botão clicado ou ACAO_NENHUMA se nenhum foi clicado
 */
AcaoBotaoMenu verificaCliqueBotoesMenu(void);

/**
 * @brief Desenha a tela de tutorial do jogo
 */
void desenhaTelaTutorial(void);

/**
 * @brief Verifica se o botão voltar do tutorial foi clicado
 * @return bool true se o botão foi clicado, false caso contrário
 */
bool verificaCliqueBotaoTutorial(void);

/**
 * @brief Verifica se o botão voltar do ranking foi clicado
 * @return bool true se o botão foi clicado, false caso contrário
 */
bool verificaCliqueBotaoRanking(void);

/**
 * @brief Carrega recursos gráficos (texturas) usados pelas telas
 * Deve ser chamado após InitWindow()
 */
void carregarRecursos(void);

/**
 * @brief Libera recursos gráficos carregados por carregarRecursos()
 */
void liberarRecursos(void);

#endif // DESENHOS_H