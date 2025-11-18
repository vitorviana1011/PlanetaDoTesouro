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

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

typedef struct {
    int x;
    int y;
}Jogador; // Posição do jogador

Jogador encontrarJogador(Mapa mapa) {
    Jogador jogador = {0, 0};
    for (int i = 0; i < mapa.linhas; i++) {
        for (int j = 0; j < mapa.colunas; j++) {
            if (mapa.dados[i][j] == '@') {
                jogador.y = i;
                jogador.x = j;
                return jogador;
            }
        }
    }
    return jogador; // Se não encontrar, retorna (0,0)
}

void desenhaMapa(Mapa mapa){
    for (int i = 0; i < mapa.linhas; i++) {
        for (int j = 0; j < mapa.colunas; j++) {
            char tile = mapa.dados[i][j];
            Color color;
            switch (tile) {
                case '#': color = BLACK; break; // Parede
                case '.': color = GRAY; break; // Caminho
                case '@': color = BLUE; break; // Jogador
                case 'E': color = YELLOW; break; // Tesouro
                default: color = LIGHTGRAY; break; // Espaço vazio
            }
            DrawRectangle(j * 32, i * 32, 32, 32, color);
        }
    }
}

void movePersonagem(Jogador *jogador, Mapa mapa){
    int novoY = jogador->y;
    int novoX = jogador->x;
    
    if (IsKeyPressed(KEY_UP)) {
        novoY = jogador->y - 1; // UP diminui Y
    }
    else if (IsKeyPressed(KEY_DOWN)) {
        novoY = jogador->y + 1; // DOWN aumenta Y
    }
    else if (IsKeyPressed(KEY_LEFT)) {
        novoX = jogador->x - 1; // LEFT diminui X
    }
    else if (IsKeyPressed(KEY_RIGHT)) {
        novoX = jogador->x + 1; // RIGHT aumenta X
    }
    else {
        return; // Nenhuma tecla pressionada
    }
    
    // Verificar limites do mapa
    if (novoY < 0 || novoY >= mapa.linhas || novoX < 0 || novoX >= mapa.colunas) {
        return;
    }
    
    // Verificar se não é parede
    if (mapa.dados[novoY][novoX] == '#') {
        return; 
    }
    
    // Movimento válido - atualizar posição
    mapa.dados[jogador->y][jogador->x] = '.';
    jogador->y = novoY;
    jogador->x = novoX;
    mapa.dados[jogador->y][jogador->x] = '@';
}

int main(){
    // --- Inicialização ---
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Planeta do Tesouro");

    Mapa mapa = carregaMapa();
    Jogador jogador = encontrarJogador(mapa);

    SetTargetFPS(60);

    while(!WindowShouldClose()){
        // --- Lógica ---
        movePersonagem(&jogador, mapa);

        // --- Desenho ---
        BeginDrawing();
            ClearBackground(DARKGRAY);
            desenhaMapa(mapa);
        EndDrawing();
    }

    // --- Limpeza ---
    CloseWindow();
    liberaMapa(&mapa);

    return 0;
}