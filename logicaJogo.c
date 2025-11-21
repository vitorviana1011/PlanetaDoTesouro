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

void verificaColisaoComInimigos(Jogador *jogador, Inimigo *inimigos, Mapa *mapa, int *statusJogo) {
    if (inimigos == NULL) return;
    
    double tempoAtual = GetTime();
    
    // Só verificar colisão se o jogador não estiver invencível
    if (tempoAtual > jogador->tempoInvencibilidade) {
        for (int i = 0; i < mapa->totalInimigos; i++) {
            if (jogador->x == inimigos[i].x && jogador->y == inimigos[i].y) {
                // Colisão detectada!
                jogador->vidas--;
                jogador->tempoInvencibilidade = tempoAtual + 2.0; // 2 segundos de invencibilidade
                verificaGameOver(jogador, statusJogo);
                return;
            }
        }
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

    if (mapa->dados[novoY][novoX] >= '1' && mapa->dados[novoY][novoX] <= '9') {
        verificaPortalNumerado(jogador, mapa, novoY, novoX);
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
    // Verificar se a posição atual do jogador deveria ter um portal
    char portalNaPosicao = verificaPortalNaPosicao(mapa, jogador->x, jogador->y);
    mapa->dados[jogador->y][jogador->x] = portalNaPosicao;
    
    jogador->y = novoY;
    jogador->x = novoX;
    mapa->dados[jogador->y][jogador->x] = '@';
}

// Função auxiliar para verificar se uma posição deveria ter um portal
char verificaPortalNaPosicao(Mapa *mapa, int x, int y) {
    // Procurar todos os portais no mapa
    for (int i = 0; i < mapa->linhas; i++) {
        for (int j = 0; j < mapa->colunas; j++) {
            if (mapa->dados[i][j] >= '1' && mapa->dados[i][j] <= '9') {
                char portalNum = mapa->dados[i][j];
                // Verificar se há um par para este portal
                for (int ii = 0; ii < mapa->linhas; ii++) {
                    for (int jj = 0; jj < mapa->colunas; jj++) {
                        if (mapa->dados[ii][jj] == portalNum && (ii != i || jj != j)) {
                            // Há um par! Verificar se a posição (x,y) corresponde a um dos portais
                            if ((i == y && j == x) || (ii == y && jj == x)) {
                                return portalNum;
                            }
                        }
                    }
                }
            }
        }
    }
    return '.'; // Não é um portal
}

// No mapa: '1' e '1' são um par, '2' e '2' são outro par
void verificaPortalNumerado(Jogador *jogador, Mapa *mapa, int y, int x) {
    char tile = mapa->dados[y][x];
    
    for (int i = 0; i < mapa->linhas; i++) {
        for (int j = 0; j < mapa->colunas; j++) {
            if (mapa->dados[i][j] == tile && (i != y || j != x)) {

                mapa->dados[jogador->y][jogador->x] = '.';
                
                jogador->y = i;
                jogador->x = j;
                
                return;
            }
        }
    }
}