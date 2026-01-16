#include "includes/raylib.h"
#include "includes/inimigo.h"
#include <stdio.h>

#define RED_TEXT "\x1b[31m"
#define RESET "\x1b[0m"

void andaInimigo(Jogador *jogador, Inimigo *inimigo, Mapa *mapa) {
    // Usar GetTime() para controle de velocidade não-bloqueante
    double tempoAtual = GetTime();
    
    // Verificar se passou tempo suficiente desde o último movimento (baseado na velocidade)
    if (tempoAtual - inimigo->ultimoMovimento < inimigo->velocidade) {
        return; // Ainda não é hora de mover
    }
    
    // NUNCA MODIFICAR O MAPA! Apenas calcular nova posição lógica
    int novoX = inimigo->x;
    int novoY = inimigo->y;
    
    // Lógica para mover em direção ao jogador
    if (jogador->x < inimigo->x) {
        // Verificar se pode mover para a esquerda
        char destino = mapa->dados[inimigo->y][inimigo->x - 1];
        if (destino != '#') {
            novoX = inimigo->x - 1;
            inimigo->direcao = 1; // Esquerda
        }
    } else if (jogador->x > inimigo->x) {
        // Verificar se pode mover para a direita
        char destino = mapa->dados[inimigo->y][inimigo->x + 1];
        if (destino != '#') {
            novoX = inimigo->x + 1;
            inimigo->direcao = 0; // Direita
        }
    }
    
    if (novoX == inimigo->x) {
        if (jogador->y < inimigo->y) {
            // Verificar se pode mover para cima
            char destino = mapa->dados[inimigo->y - 1][inimigo->x];
            if (destino != '#') {
                novoY = inimigo->y - 1;
                inimigo->direcao = 2; // Cima
            }
        } else if (jogador->y > inimigo->y) {
            // Verificar se pode mover para baixo
            char destino = mapa->dados[inimigo->y + 1][inimigo->x];
            if (destino != '#') {
                novoY = inimigo->y + 1;
                inimigo->direcao = 3; // Baixo
            }
        }
    }

    if (novoX == inimigo->x && novoY == inimigo->y) {
        // Não conseguiu se mover
        return;
    }

    // Atualizar APENAS a posição lógica do inimigo
    // (A colisão será verificada depois no main loop)
    inimigo->x = novoX;
    inimigo->y = novoY;
    
    // Atualizar timestamp do último movimento
    inimigo->ultimoMovimento = tempoAtual;
}
