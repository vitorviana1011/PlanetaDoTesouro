/**
 * @file planetadotesouro.c
 * @author Vitor Viana (vitorluizcviana@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "includes/raylib.h"
#include "includes/manipulaArquivos.h"
#include "includes/logicaJogo.h"
#include "includes/inimigo.h"
#include "includes/desenhos.h"
#include "includes/audio.h"
#include <string.h>

// Variável global para o nome do jogador
static char nomeJogador[MAX_NOME] = "";

int main(){
    // --- Inicialização ---
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Planeta do Tesouro");
    // Carregar recursos gráficos (texturas)
    carregarRecursos();
    listarMapasDisponiveis();

    int fase = 1;
    int vidasIniciais = 3;

    Inimigo* inimigo = NULL;
    Cronometro cronometro = {0};  // Cronômetro integrado ao jogo

    // Iniciar cronômetro automaticamente
    iniciarCronometro(&cronometro);

    Mapa mapa = carregaMapa(fase, &inimigo);
    Jogador jogador = encontrarJogador(mapa, vidasIniciais);

    int tesouroColetados = 0;
    int statusJogo = MENU;
    int statusJogoAnterior = -1; // Para controlar mudanças de estado

    SetTargetFPS(60);

    while(!WindowShouldClose()){
        // --- Atualizar áudio ---
        atualizarMusica();
        
        // Controlar música baseado na mudança de estado
        if (statusJogo != statusJogoAnterior) {
            switch (statusJogo) {
                case MENU:
                case TELA_RANKING:
                case TUTORIAL:
                    pararMusicaPrincipal();
                    iniciarMusicaMenu();
                    break;
                case JOGANDO:
                case ENTRE_FASES:
                    pararMusicaMenu();
                    iniciarMusicaPrincipal();
                    break;
                case GAME_OVER:
                    pararMusicaPrincipal();
                    tocarSomGameOver();
                    iniciarMusicaMenu();
                    break;
                case JOGO_COMPLETO:
                case TELA_RESULTADOS:
                case TELA_INPUT_NOME:
                    pararMusicaPrincipal();
                    pararMusicaMenu();
                    break;
            }
            statusJogoAnterior = statusJogo;
        }
        
        // --- Lógica ---
        movePersonagem(&jogador, &mapa, &statusJogo, &tesouroColetados);
        
        // Mover todos os inimigos independentemente (só se o jogo estiver ativo)
        if (inimigo != NULL && statusJogo == JOGANDO) {
            for (int i = 0; i < mapa.totalInimigos; i++) {
                andaInimigo(&jogador, &inimigo[i], &mapa);
            }
            
            // Verificar colisão após movimento de todos os inimigos
            verificaColisaoComInimigos(&jogador, inimigo, &mapa, &statusJogo);
        }

        if (statusJogo == ENTRE_FASES) {
            if (IsKeyPressed(KEY_ENTER)) {
                proximaFase(&mapa, &jogador, &statusJogo, &tesouroColetados, &fase, &inimigo, &cronometro);
            }
        }
        
        // Navegação entre telas
        if (statusJogo == TELA_RESULTADOS) {
            if (IsKeyPressed(KEY_ENTER)) {
                statusJogo = TELA_INPUT_NOME;
            }
        }
        
        if (statusJogo == TELA_INPUT_NOME) {
            int tecla = GetCharPressed();
            
            // Adicionar caracteres ao nome
            if (tecla >= 32 && tecla <= 125 && strlen(nomeJogador) < MAX_NOME - 1) {
                int len = strlen(nomeJogador);
                nomeJogador[len] = (char)tecla;
                nomeJogador[len + 1] = '\0';
            }
            
            // Backspace para apagar
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(nomeJogador) > 0) {
                nomeJogador[strlen(nomeJogador) - 1] = '\0';
            }
            
            // Enter para confirmar (apenas se há um nome)
            if (IsKeyPressed(KEY_ENTER) && strlen(nomeJogador) > 0) {
                salvarRecorde(&cronometro, nomeJogador);
                statusJogo = TELA_RANKING;
            }
            
            // ESC para cancelar
            if (IsKeyPressed(KEY_ESCAPE)) {
                salvarRecorde(&cronometro, "Anônimo");
                statusJogo = TELA_RANKING;
            }
        }
        
        if (statusJogo == TELA_RANKING) {
            if (IsKeyPressed(KEY_ESCAPE)) {
                CloseWindow();
                break;
            }
        }

        // --- Desenho ---
        BeginDrawing();
            ClearBackground(DARKGRAY);
            
            // Só desenha o mapa quando estiver jogando
            if (statusJogo == JOGANDO || statusJogo == ENTRE_FASES || statusJogo == GAME_OVER) {
                desenhaMapa(mapa);
            }
            
            switch (statusJogo) {
                case MENU:
                    desenhaTelaMenu();
                
                    // Verificar cliques nos botões
                    AcaoBotaoMenu acao = verificaCliqueBotoesMenu();
                    switch (acao) {
                        case ACAO_INICIAR:
                            tocarSomStart(); // Som de início do jogo
                            statusJogo = JOGANDO;
                            iniciarFase(&cronometro, fase);
                            break;
                        case ACAO_RANKING:
                            statusJogo = TELA_RANKING;
                            break;
                        case ACAO_TUTORIAL:
                            statusJogo = TUTORIAL;
                            break;
                        case ACAO_NENHUMA:
                        default:
                            break;
                    }
                    
                    // Manter compatibilidade com teclado
                    switch (GetKeyPressed()) {
                        case KEY_ENTER:
                            tocarSomStart(); // Som de início do jogo
                            statusJogo = JOGANDO;
                            iniciarFase(&cronometro, fase);
                            break;
                        case KEY_ESCAPE:
                            CloseWindow();
                            break;
                    }
                    break;
                case JOGANDO:
                    desenhaHUDJogo(fase, tesouroColetados, mapa, jogador, &cronometro);
                    desenhaPortais(mapa, jogador); // Desenhar portal embaixo do jogador primeiro
                    desenhaInimigos(mapa, inimigo);
                    desenhaJogadorComEfeito(jogador, mapa); // Jogador por último, em cima de tudo
                    break;
                case JOGO_COMPLETO:
                    desenhaTelaJogoCompleto(cronometro);
                    break;
                case ENTRE_FASES:
                    desenhaTelaEntreFases();
                    break;
                case GAME_OVER:
                    desenhaTelaGameOver();
                    
                    // Verificar clique no botão VOLTAR AO MENU ou tecla ESC (compatibilidade)
                    if (verificaCliqueBotaoGameOver() || IsKeyPressed(KEY_ESCAPE)) {
                        statusJogo = MENU;
                        fase = 1; // Resetar para a fase 1
                        tesouroColetados = 0; // Resetar tesouros
                        
                        // Recarregar o mapa da fase 1
                        liberaMapa(&mapa);
                        if (inimigo != NULL) {
                            free(inimigo);
                            inimigo = NULL;
                        }
                        mapa = carregaMapa(fase, &inimigo);
                        jogador = encontrarJogador(mapa, vidasIniciais);
                        iniciarCronometro(&cronometro);
                    }
                    break;
                case TELA_RESULTADOS:
                    desenhaTelaResultados(&cronometro);
                    break;
                case TELA_RANKING:
                    desenhaRanking();
                    
                    // Verificar clique no botão VOLTAR
                    if (verificaCliqueBotaoRanking()) {
                        statusJogo = MENU;
                    }
                    
                    // Voltar ao menu com ESC (manter compatibilidade)
                    if (IsKeyPressed(KEY_ESCAPE)) {
                        statusJogo = MENU;
                    }

                    fase = 1;

                    break;
                case TELA_INPUT_NOME:
                    desenhaTelaInputNome(nomeJogador, &cronometro);
                    break;
                case TUTORIAL:
                    desenhaTelaTutorial();
                    
                    // Verificar clique no botão VOLTAR
                    if (verificaCliqueBotaoTutorial()) {
                        statusJogo = MENU;
                    }
                    
                    // Voltar ao menu com ESC (manter compatibilidade)
                    if (IsKeyPressed(KEY_ESCAPE)) {
                        statusJogo = MENU;
                    }
                    break;
            }
            
        EndDrawing();
    }
    
    // --- Limpeza ---
    // Liberar texturas carregadas
    liberarRecursos();
    CloseWindow();
    liberaMapa(&mapa);
    
    // Liberar memória dos inimigos
    if (inimigo != NULL) {
        free(inimigo);
    }

    return 0;
}