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
#include <string.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

// Variável global para o nome do jogador
static char nomeJogador[MAX_NOME] = "";

void desenhaMapa(Mapa mapa){
    int tileSize = 32;
    int mapaLargura = mapa.colunas * tileSize;
    int mapaAltura = mapa.linhas * tileSize;
    
    int offsetX = (SCREEN_WIDTH - mapaLargura) / 2;
    int offsetY = (SCREEN_HEIGHT - mapaAltura) / 2;
    
    for (int i = 0; i < mapa.linhas; i++) {
        for (int j = 0; j < mapa.colunas; j++) {
            char tile = mapa.dados[i][j];
            Color color;
            switch (tile) {
                case '#': color = BLACK; break; // Parede
                case '.': color = GRAY; break; // Caminho
                case '@': color = BLUE; break; // Jogador
                case 'T': color = YELLOW; break; // Tesouro
                case 'I': color = GRAY; break;
                case 'C': color = GREEN; break; // Cura
                case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    color = PURPLE; break; // Portal numerado
                default: color = LIGHTGRAY; break; // Espaço vazio
            }
            DrawRectangle(offsetX + j * tileSize, offsetY + i * tileSize, tileSize, tileSize, color);
        }
    }
}

void desenhaPortais(Mapa mapa, Jogador jogador) {
    int tileSize = 32;
    int mapaLargura = mapa.colunas * tileSize;
    int mapaAltura = mapa.linhas * tileSize;
    
    int offsetX = (SCREEN_WIDTH - mapaLargura) / 2;
    int offsetY = (SCREEN_HEIGHT - mapaAltura) / 2;
    
    // Desenhar portais onde o jogador está (se houver)
    char portal = verificaPortalNaPosicao(&mapa, jogador.x, jogador.y);
    if (portal >= '1' && portal <= '9') {
        int x = offsetX + jogador.x * tileSize;
        int y = offsetY + jogador.y * tileSize;
        DrawRectangle(x, y, tileSize, tileSize, BLUE);
    }
}

void desenhaInimigos(Mapa mapa, Inimigo *inimigos) {
    if (inimigos == NULL) return;
    
    int tileSize = 32;
    int mapaLargura = mapa.colunas * tileSize;
    int mapaAltura = mapa.linhas * tileSize;
    
    int offsetX = (SCREEN_WIDTH - mapaLargura) / 2;
    int offsetY = (SCREEN_HEIGHT - mapaAltura) / 2;
    
    // Desenhar todos os inimigos na sua posição lógica
    for (int i = 0; i < mapa.totalInimigos; i++) {
        int x = offsetX + inimigos[i].x * tileSize;
        int y = offsetY + inimigos[i].y * tileSize;
        DrawRectangle(x, y, tileSize, tileSize, RED);
    }
}

void desenhaJogadorComEfeito(Jogador jogador, Mapa mapa){
    int tileSize = 32;
    int x = jogador.x * tileSize + (SCREEN_WIDTH - (tileSize *  mapa.colunas)) / 2;
    int y = jogador.y * tileSize + (SCREEN_HEIGHT - (tileSize *  mapa.linhas)) / 2;
    
    // Se o jogador está invencível, fazer ele piscar
    double tempoAtual = GetTime();
    if (tempoAtual < jogador.tempoInvencibilidade) {
        // Piscar a cada 0.2 segundos
        if (((int)(tempoAtual * 5)) % 2 == 0) {
            DrawRectangle(x, y, tileSize, tileSize, SKYBLUE); // Cor alternativa
        } else {
            DrawRectangle(x, y, tileSize, tileSize, BLUE); // Cor normal
        }
    }
}

int main(){
    // --- Inicialização ---
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Planeta do Tesouro");
    listarMapasDisponiveis();

    int fase = 6;
    int vidasIniciais = 3;

    Inimigo* inimigo = NULL;
    Cronometro cronometro = {0};  // Cronômetro integrado ao jogo

    // Iniciar cronômetro automaticamente
    iniciarCronometro(&cronometro);

    Mapa mapa = carregaMapa(fase, &inimigo);
    Jogador jogador = encontrarJogador(mapa, vidasIniciais);

    // Iniciar cronômetro da primeira fase
    iniciarFase(&cronometro, fase);

    int tesouroColetados = 0;
    int statusJogo = JOGANDO;

    SetTargetFPS(60);

    while(!WindowShouldClose()){
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
                // TODO: Implementar menu
                break;
                case JOGANDO:
                    // Info principal do jogo
                    DrawText(TextFormat("Fase: %d", fase), 10, 10, 20, WHITE);
                    DrawText(TextFormat("Tesouros: %d/%d", tesouroColetados, mapa.totalTesouros), 10, 35, 20, WHITE);
                    DrawText(TextFormat("Vidas: %d", jogador.vidas), 10, 60, 20, WHITE);
                    
                    // Cronômetro discreto no canto
                    double tempoAtual = GetTime();
                    double tempoFaseAtual = tempoAtual - cronometro.inicioFase;
                    double tempoTotalAtual = tempoAtual - cronometro.inicioJogo;
                    
                    DrawText(TextFormat("Tempo: %.1fs", tempoFaseAtual), SCREEN_WIDTH - 150, 10, 18, LIGHTGRAY);
                    DrawText(TextFormat("Total: %.1fs", tempoTotalAtual), SCREEN_WIDTH - 150, 30, 16, DARKGRAY);
                    
                    desenhaPortais(mapa, jogador); // Desenhar portal embaixo do jogador primeiro
                    desenhaInimigos(mapa, inimigo);
                    desenhaJogadorComEfeito(jogador, mapa); // Jogador por último, em cima de tudo
                    break;
                case JOGO_COMPLETO:
                    DrawText("PARABÉNS! JOGO COMPLETO!", SCREEN_WIDTH / 2 - MeasureText("PARABÉNS! JOGO COMPLETO!", 30) / 2, 40, 30, GREEN);
                    DrawText(TextFormat("Tempo Final: %.2f segundos", cronometro.tempoTotal), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Tempo Final: %.2f segundos", cronometro.tempoTotal), 25) / 2, 80, 25, YELLOW);
                    DrawText("Relatório de tempo salvo!", SCREEN_WIDTH / 2 - MeasureText("Relatório de tempo salvo!", 20) / 2, 120, 20, WHITE);
                    DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para sair", 20) / 2, 150, 20, LIGHTGRAY);
                    break;
                case ENTRE_FASES:
                    DrawText("Fase Completa! Aperte ENTER para continuar", SCREEN_WIDTH / 2 - MeasureText("Fase Completa! Preparando próxima fase...", 20) / 2, 40, 20, GREEN);
                    break;
                case GAME_OVER:
                    DrawText("GAME OVER!", SCREEN_WIDTH / 2 - MeasureText("GAME OVER!", 40) / 2, SCREEN_HEIGHT / 2 - 60, 40, RED);
                    DrawText("Suas vidas acabaram!", SCREEN_WIDTH / 2 - MeasureText("Suas vidas acabaram!", 20) / 2, SCREEN_HEIGHT / 2 - 10, 20, WHITE);
                    DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para sair", 20) / 2, SCREEN_HEIGHT / 2 + 20, 20, YELLOW);
                    break;
                case TELA_RESULTADOS:
                    desenhaTelaResultados(&cronometro);
                    break;
                case TELA_RANKING:
                    desenhaRanking();
                    break;
                case TELA_INPUT_NOME:
                    desenhaTelaInputNome(nomeJogador, &cronometro);
                    break;
            }
            
        EndDrawing();
    }
    
    // --- Limpeza ---
    CloseWindow();
    liberaMapa(&mapa);
    
    // Liberar memória dos inimigos
    if (inimigo != NULL) {
        free(inimigo);
    }

    return 0;
}