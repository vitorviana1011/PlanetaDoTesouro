#include "includes/raylib.h"
#include "includes/manipulaArquivos.h"
#include "includes/logicaJogo.h"
#include "includes/inimigo.h"

Jogador encontrarJogador(Mapa mapa, int vidas) {
    Jogador jogador = {0, 0, vidas, 0.0};
    for (int i = 0; i < mapa.linhas; i++) {
        for (int j = 0; j < mapa.colunas; j++) {
            if (mapa.dados[i][j] == '@') {
                jogador.y = i;
                jogador.x = j;
                return jogador;
            }
        }
    }
    return jogador;
}

bool confereTesouro(Mapa *mapa, int x, int y){
    if(mapa->dados[y][x] == 'T'){
        mapa->dados[y][x] = '.';
        return true;
    }
    return false;
}

void proximaFase(Mapa *mapa, Jogador *jogador, int *statusJogo, int *tesouroColetados, int *fase, Inimigo **inimigo){
    int novaFase = (*fase) + 1;
    
    // Liberar inimigos anteriores
    if (*inimigo != NULL) {
        free(*inimigo);
        *inimigo = NULL;
    }
    
    Mapa novoMapa = carregaMapa(novaFase, inimigo);
    
    if (novoMapa.dados != NULL) {
        liberaMapa(mapa);
        *mapa = novoMapa;
        *jogador = encontrarJogador(*mapa, jogador->vidas);
        *tesouroColetados = 0;
        (*fase)++;
        *statusJogo = JOGANDO;
    } else {
        *statusJogo = JOGO_COMPLETO;
    }
}

void verificaGameOver(Jogador *jogador, int *statusJogo) {
    if (jogador->vidas <= 0) {
        *statusJogo = GAME_OVER;
    }
}

void movePersonagem(Jogador *jogador, Mapa *mapa, int *statusJogo, int *tesouroColetados){
    // Verificar se o jogo acabou antes de permitir movimento
    if (jogador->vidas <= 0) {
        *statusJogo = GAME_OVER;
        return;
    }
    
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
    if (novoY < 0 || novoY >= mapa->linhas || novoX < 0 || novoX >= mapa->colunas) {
        return;
    }
    
    // Verificar se não é parede
    if (mapa->dados[novoY][novoX] == '#') {
        return; 
    }

    // Lógica para coletar tesouro
    if (confereTesouro(mapa, novoX, novoY)) {
        (*tesouroColetados)++;
        // Verificar se todos os tesouros foram coletados
        if (*tesouroColetados >= mapa->totalTesouros) {
            *statusJogo = ENTRE_FASES;
            return; // Não mover o jogador, apenas mudar o status
        }
    }

    // Movimento válido - atualizar posição
    mapa->dados[jogador->y][jogador->x] = '.';
    jogador->y = novoY;
    jogador->x = novoX;
    mapa->dados[jogador->y][jogador->x] = '@';
}