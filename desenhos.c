/**
 * @file desenhos.c
 * @author Vitor Viana (vitorluizcviana@gmail.com)
 * @brief Implementações de todas as funções de renderização visual do jogo
 * @version 0.1
 * @date 2026-01-02
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "includes/desenhos.h"
#include "includes/manipulaArquivos.h"
#include <stdio.h>
#include <string.h>


void desenhaMapa(Mapa mapa) {
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

void desenhaJogadorComEfeito(Jogador jogador, Mapa mapa) {
    int tileSize = 32;
    int mapaLargura = mapa.colunas * tileSize;
    int mapaAltura = mapa.linhas * tileSize;
    
    int offsetX = (SCREEN_WIDTH - mapaLargura) / 2;
    int offsetY = (SCREEN_HEIGHT - mapaAltura) / 2;
    
    int x = offsetX + jogador.x * tileSize;
    int y = offsetY + jogador.y * tileSize;
    
    if (jogador.tempoInvencibilidade > GetTime()) {
        if (((int)(GetTime() * 10)) % 2 == 0) { // Piscar mais rápido
            DrawRectangle(x, y, tileSize, tileSize, SKYBLUE); // Cor alternativa
        } else {
            DrawRectangle(x, y, tileSize, tileSize, BLUE); // Cor normal
        }
    } else {
        DrawRectangle(x, y, tileSize, tileSize, BLUE);
    }
}

void desenhaHUDJogo(int fase, int tesouroColetados, Mapa mapa, Jogador jogador, Cronometro *cronometro) {
    // Info principal do jogo
    DrawText(TextFormat("Fase: %d", fase), 10, 10, 20, WHITE);
    DrawText(TextFormat("Tesouros: %d/%d", tesouroColetados, mapa.totalTesouros), 10, 35, 20, WHITE);
    DrawText(TextFormat("Vidas: %d", jogador.vidas), 10, 60, 20, WHITE);
    
    // Cronômetro discreto no canto
    double tempoAtual = GetTime();
    double tempoFaseAtual = tempoAtual - cronometro->inicioFase;
    double tempoTotalAtual = tempoAtual - cronometro->inicioJogo;
    
    DrawText(TextFormat("Tempo: %.1fs", tempoFaseAtual), SCREEN_WIDTH - 150, 10, 18, LIGHTGRAY);
    DrawText(TextFormat("Total: %.1fs", tempoTotalAtual), SCREEN_WIDTH - 150, 30, 16, DARKGRAY);
}

// ===== TELAS DO SISTEMA DE SPEEDRUN =====

void desenhaTelaResultados(Cronometro *cronometro) {
    ClearBackground(DARKGREEN);
    
    // Título principal
    DrawText("🎉 PARABÉNS! 🎉", SCREEN_WIDTH / 2 - MeasureText("🎉 PARABÉNS! 🎉", 50) / 2, 100, 50, GOLD);
    DrawText("JOGO COMPLETO!", SCREEN_WIDTH / 2 - MeasureText("JOGO COMPLETO!", 40) / 2, 170, 40, WHITE);
    
    // Estatísticas do speedrun
    DrawText("📊 ESTATÍSTICAS FINAIS", SCREEN_WIDTH / 2 - MeasureText("📊 ESTATÍSTICAS FINAIS", 30) / 2, 250, 30, LIGHTGRAY);
    
    char tempoTotal[100];
    snprintf(tempoTotal, sizeof(tempoTotal), "⏱️  Tempo Total: %.2f segundos", cronometro->tempoTotal);
    DrawText(tempoTotal, SCREEN_WIDTH / 2 - MeasureText(tempoTotal, 25) / 2, 320, 25, YELLOW);
    
    // Calcular fases completadas
    int fasesCompletas = 0;
    for (int i = 0; i < MAX_FASES; i++) {
        if (cronometro->temposPorFase[i] > 0) {
            fasesCompletas = i + 1;
        }
    }
    
    char fases[100];
    snprintf(fases, sizeof(fases), "Fases Completadas: %d", fasesCompletas);
    DrawText(fases, SCREEN_WIDTH / 2 - MeasureText(fases, 25) / 2, 360, 25, LIME);
    
    // Detalhes por fase
    DrawText("TEMPOS POR FASE:", 200, 420, 22, LIGHTGRAY);
    int yPos = 450;
    for (int i = 0; i < fasesCompletas && i < 6; i++) {
        char tempoFase[50];
        snprintf(tempoFase, sizeof(tempoFase), "Fase %d: %.2fs", i + 1, cronometro->temposPorFase[i]);
        DrawText(tempoFase, 220, yPos, 18, WHITE);
        yPos += 25;
    }
    
    // Instruções
    DrawText("Pressione ENTER para registrar seu nome", SCREEN_WIDTH / 2 - MeasureText("Pressione ENTER para registrar seu nome", 22) / 2, SCREEN_HEIGHT - 100, 22, GREEN);
    DrawText("e visualizar o ranking!", SCREEN_WIDTH / 2 - MeasureText("e visualizar o ranking!", 20) / 2, SCREEN_HEIGHT - 70, 20, GREEN);
}

void desenhaTelaInputNome(char *nomeJogador, Cronometro *cronometro) {
    ClearBackground(DARKGREEN);
    
    // Título
    DrawText("NOVO RECORDE!", SCREEN_WIDTH / 2 - MeasureText("NOVO RECORDE!", 40) / 2, 100, 40, GOLD);
    
    // Tempo alcançado
    char textoTempo[100];
    snprintf(textoTempo, sizeof(textoTempo), "Seu tempo: %.2f segundos", cronometro->tempoTotal);
    DrawText(textoTempo, SCREEN_WIDTH / 2 - MeasureText(textoTempo, 25) / 2, 170, 25, WHITE);
    
    // Campo de input
    DrawText("Digite seu nome:", SCREEN_WIDTH / 2 - MeasureText("Digite seu nome:", 25) / 2, 250, 25, LIGHTGRAY);
    
    // Caixa de texto
    Rectangle textBox = { SCREEN_WIDTH/2 - 150, 300, 300, 50 };
    DrawRectangleRec(textBox, LIGHTGRAY);
    DrawRectangleLinesEx(textBox, 3, DARKGRAY);
    
    // Texto digitado
    DrawText(nomeJogador, textBox.x + 10, textBox.y + 15, 20, BLACK);
    
    // Cursor piscando
    if (((int)(GetTime() * 2)) % 2 == 0) {
        int textWidth = MeasureText(nomeJogador, 20);
        DrawText("|", textBox.x + 10 + textWidth, textBox.y + 15, 20, BLACK);
    }
    
    // Instruções
    DrawText("Pressione ENTER para salvar", SCREEN_WIDTH / 2 - MeasureText("Pressione ENTER para salvar", 20) / 2, 400, 20, YELLOW);
    DrawText("Pressione ESC para usar nome 'Anônimo'", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para usar nome 'Anônimo'", 18) / 2, 430, 18, GRAY);
}

void desenhaRanking(void) {
    ClearBackground(BLACK);  // Fundo preto limpo
    
    // Carregar recordes do arquivo binário
    FILE *arquivo = fopen("ranking.bin", "rb");
    if (arquivo == NULL) {
        DrawText("🏆 RANKING 🏆", SCREEN_WIDTH / 2 - MeasureText("🏆 RANKING 🏆", 40) / 2, 200, 40, GOLD);
        DrawText("Nenhum tempo registrado ainda.", SCREEN_WIDTH / 2 - MeasureText("Nenhum tempo registrado ainda.", 25) / 2, 300, 25, WHITE);
        DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para sair", 20) / 2, SCREEN_HEIGHT - 100, 20, YELLOW);
        return;
    }
    
    Recorde recordes[MAX_RECORDES];
    int totalRecordes = 0;
    
    // Ler todos os recordes
    while (fread(&recordes[totalRecordes], sizeof(Recorde), 1, arquivo) == 1 && 
           totalRecordes < MAX_RECORDES) {
        totalRecordes++;
    }
    fclose(arquivo);
    
    // Ordenar por tempo (bubble sort)
    for (int i = 0; i < totalRecordes - 1; i++) {
        for (int j = 0; j < totalRecordes - i - 1; j++) {
            if (recordes[j].tempoTotal > recordes[j + 1].tempoTotal) {
                Recorde temp = recordes[j];
                recordes[j] = recordes[j + 1];
                recordes[j + 1] = temp;
            }
        }
    }
    
    // Desenhar título
    DrawText("🏆 RANKING DE TEMPOS 🏆", SCREEN_WIDTH / 2 - MeasureText("🏆 RANKING DE TEMPOS 🏆", 35) / 2, 50, 35, GOLD);
    
    // Desenhar cabeçalho da tabela
    DrawText("Pos", 80, 120, 20, LIGHTGRAY);
    DrawText("Nome", 150, 120, 20, LIGHTGRAY);
    DrawText("Tempo", 400, 120, 20, LIGHTGRAY);
    DrawText("Fases", 550, 120, 20, LIGHTGRAY);
    DrawText("Data", 650, 120, 20, LIGHTGRAY);
    
    // Linha separadora
    DrawLine(50, 145, SCREEN_WIDTH - 50, 145, LIGHTGRAY);
    
    // Desenhar top 10
    int yPos = 160;
    for (int i = 0; i < totalRecordes && i < 10 && yPos < SCREEN_HEIGHT - 150; i++) {
        Color cor = WHITE;
        if (i == 0) cor = GOLD;           // 1º lugar - Dourado
        else if (i == 1) cor = LIGHTGRAY; // 2º lugar - Prata  
        else if (i == 2) cor = ORANGE;    // 3º lugar - Bronze
        
        // Posição
        char posStr[10];
        snprintf(posStr, sizeof(posStr), "%d°", i + 1);
        DrawText(posStr, 80, yPos, 20, cor);
        
        // Nome
        DrawText(recordes[i].nomeJogador, 150, yPos, 20, cor);
        
        // Tempo
        char tempoStr[20];
        snprintf(tempoStr, sizeof(tempoStr), "%.2fs", recordes[i].tempoTotal);
        DrawText(tempoStr, 400, yPos, 20, cor);
        
        // Fases
        char fasesStr[10];
        snprintf(fasesStr, sizeof(fasesStr), "%d", recordes[i].fasesCompletas);
        DrawText(fasesStr, 570, yPos, 20, cor);
        
        // Data (só dia/mês)
        char dataStr[20];
        strncpy(dataStr, recordes[i].dataHora, 10);
        dataStr[10] = '\0';
        DrawText(dataStr, 650, yPos, 18, cor);
        
        yPos += 35;
    }
    
    // Linha separadora inferior
    DrawLine(50, yPos + 10, SCREEN_WIDTH - 50, yPos + 10, LIGHTGRAY);
    
    // Mostrar detalhes do primeiro lugar se houver recordes
    if (totalRecordes > 0) {
        DrawText("🥇 MELHOR TEMPO:", 80, yPos + 40, 25, GOLD);
        
        char detalhes[200];
        snprintf(detalhes, sizeof(detalhes), "%s - %.2fs (%d fases)", 
                recordes[0].nomeJogador, recordes[0].tempoTotal, recordes[0].fasesCompletas);
        DrawText(detalhes, 100, yPos + 75, 22, WHITE);
    }
    
    // Instruções
    DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para sair", 20) / 2, SCREEN_HEIGHT - 50, 20, YELLOW);
}

// ===== TELAS DE STATUS DO JOGO =====

void desenhaTelaJogoCompleto(Cronometro cronometro) {
    DrawText("PARABÉNS! JOGO COMPLETO!", SCREEN_WIDTH / 2 - MeasureText("PARABÉNS! JOGO COMPLETO!", 30) / 2, 40, 30, GREEN);
    DrawText(TextFormat("Tempo Final: %.2f segundos", cronometro.tempoTotal), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Tempo Final: %.2f segundos", cronometro.tempoTotal), 25) / 2, 80, 25, YELLOW);
    DrawText("Relatório de tempo salvo!", SCREEN_WIDTH / 2 - MeasureText("Relatório de tempo salvo!", 20) / 2, 120, 20, WHITE);
    DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para sair", 20) / 2, 150, 20, LIGHTGRAY);
}

void desenhaTelaEntreFases(void) {
    DrawText("Fase Completa! Aperte ENTER para continuar", SCREEN_WIDTH / 2 - MeasureText("Fase Completa! Preparando próxima fase...", 20) / 2, 40, 20, GREEN);
}

void desenhaTelaGameOver(void) {
    DrawText("GAME OVER!", SCREEN_WIDTH / 2 - MeasureText("GAME OVER!", 40) / 2, SCREEN_HEIGHT / 2 - 60, 40, RED);
    DrawText("Suas vidas acabaram!", SCREEN_WIDTH / 2 - MeasureText("Suas vidas acabaram!", 20) / 2, SCREEN_HEIGHT / 2 - 10, 20, WHITE);
    DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para sair", 20) / 2, SCREEN_HEIGHT / 2 + 20, 20, YELLOW);
}