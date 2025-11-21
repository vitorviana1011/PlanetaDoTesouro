#include "includes/raylib.h"
#include "includes/inimigo.h"

#define RED "\x1b[31m"
#define RESET "\x1b[0m"

static double ultimoMovimento = 0.0;

void andaInimigo(Jogador *jogador, Inimigo *inimigo, Mapa *mapa) {
    // Usar GetTime() para controle de velocidade não-bloqueante
    double tempoAtual = GetTime();
    
    // Verificar se passou tempo suficiente desde o último movimento (baseado na velocidade)
    if (tempoAtual - ultimoMovimento < inimigo->velocidade) {
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
        }
    } else if (jogador->x > inimigo->x) {
        // Verificar se pode mover para a direita
        char destino = mapa->dados[inimigo->y][inimigo->x + 1];
        if (destino != '#') {
            novoX = inimigo->x + 1;
        }
    }
    
    // Se não se moveu horizontalmente, tentar verticalmente
    if (novoX == inimigo->x) {
        if (jogador->y < inimigo->y) {
            // Verificar se pode mover para cima
            char destino = mapa->dados[inimigo->y - 1][inimigo->x];
            if (destino != '#') {
                novoY = inimigo->y - 1;
            }
        } else if (jogador->y > inimigo->y) {
            // Verificar se pode mover para baixo
            char destino = mapa->dados[inimigo->y + 1][inimigo->x];
            if (destino != '#') {
                novoY = inimigo->y + 1;
            }
        }
    }

    if (novoX == inimigo->x && novoY == inimigo->y) {
        // Não conseguiu se mover
        return;
    }

    // Verificar colisão com jogador
    if (novoX == jogador->x && novoY == jogador->y) {
        // Só causar dano se o jogador não estiver invencível
        if (tempoAtual > jogador->tempoInvencibilidade) {
            // Colisão detectada! Reduzir vida do jogador
            jogador->vidas--;
            
            // Tornar jogador invencível por 2 segundos
            jogador->tempoInvencibilidade = tempoAtual + 2.0;
            
            printf(RED"Jogador atingido! Vidas restantes: %d\n"RESET, jogador->vidas);
        }
        
        // Não mover o inimigo para a posição do jogador
        // Adicionar pequeno delay antes do próximo movimento do inimigo
        ultimoMovimento = tempoAtual + 0.3; // Delay menor
        return;
    }
        
    
    // Atualizar APENAS a posição lógica do inimigo
    inimigo->x = novoX;
    inimigo->y = novoY;
    
    // Atualizar timestamp do último movimento
    ultimoMovimento = tempoAtual;
}
