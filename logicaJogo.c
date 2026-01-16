#include "includes/raylib.h"
#include "includes/manipulaArquivos.h"
#include "includes/logicaJogo.h"
#include "includes/inimigo.h"
#include "includes/desenhos.h"
#include "includes/audio.h"

Jogador encontrarJogador(Mapa mapa, int vidas) {
    Jogador jogador = {-1, -1, vidas, 0.0}; // Inicializar com posições inválidas
    
    // Verificar se o mapa é válido
    if (mapa.dados == NULL || mapa.linhas <= 0 || mapa.colunas <= 0) {
        printf("ERRO: Mapa inválido ao procurar jogador\n");
        return jogador;
    }
    
    printf("Procurando jogador em mapa %dx%d...\n", mapa.linhas, mapa.colunas);
    
    for (int i = 0; i < mapa.linhas; i++) {
        if (mapa.dados[i] == NULL) {
            printf("ERRO: Linha %d do mapa é nula!\n", i);
            return jogador;
        }
        
        for (int j = 0; j < mapa.colunas; j++) {
            if (mapa.dados[i][j] == '@') {
                jogador.y = i;
                jogador.x = j;
                printf("Jogador encontrado na posição (%d, %d)\n", jogador.x, jogador.y);
                return jogador;
            }
        }
    }
    
    printf("AVISO: Jogador '@' não encontrado no mapa!\n");
    return jogador;
}

bool confereTesouro(Mapa *mapa, int x, int y){
    if(mapa->dados[y][x] == 'T'){
        mapa->dados[y][x] = '.';
        return true;
    }
    return false;
}

void proximaFase(Mapa *mapa, Jogador *jogador, int *statusJogo, int *tesouroColetados, int *fase, Inimigo **inimigo, Cronometro *cronometro){
    if (!mapa || !jogador || !statusJogo || !tesouroColetados || !fase || !inimigo || !cronometro) {
        printf("ERRO: Parâmetros nulos em proximaFase!\n");
        return;
    }
    
    // Finalizar cronômetro da fase anterior
    if (*fase > 0) {
        finalizarFase(cronometro);
    }
    
    int novaFase = (*fase) + 1;
    printf("Tentando carregar fase %d...\n", novaFase);
    
    // Liberar inimigos anteriores com segurança
    if (*inimigo != NULL) {
        printf("Liberando inimigos anteriores...\n");
        free(*inimigo);
        *inimigo = NULL;
    }
    
    // Carregar novo mapa
    Mapa novoMapa = carregaMapa(novaFase, inimigo);
    
    // Verificar se o carregamento foi bem-sucedido
    if (novoMapa.dados != NULL && novoMapa.linhas > 0 && novoMapa.colunas > 0) {
        printf("Novo mapa carregado com sucesso, liberando mapa anterior...\n");
        
        // Liberar mapa anterior com segurança
        if (mapa->dados != NULL) {
            liberaMapa(mapa);
        }
        
        // Atribuir novo mapa
        *mapa = novoMapa;
        
        // Verificar se encontrou o jogador no novo mapa
        Jogador novoJogador = encontrarJogador(*mapa, jogador->vidas);
        if (novoJogador.x >= 0 && novoJogador.y >= 0) {
            *jogador = novoJogador;
            *tesouroColetados = 0;
            (*fase)++;
            *statusJogo = JOGANDO;
            
            // Iniciar cronômetro da nova fase
            iniciarFase(cronometro, *fase);
            
            printf("Fase %d carregada com sucesso!\n", *fase);
        } else {
            printf("ERRO: Jogador não encontrado no mapa da fase %d\n", novaFase);
            *statusJogo = GAME_OVER;
        }
    } else {
        printf("Fim do jogo - Fase %d não encontrada ou erro no carregamento\n", novaFase);
        
        // Finalizar jogo
        finalizarJogo(cronometro);
        
        *statusJogo = TELA_RESULTADOS;
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
                tocarSomDano(); // Tocar som de dano
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

    if (*statusJogo != JOGANDO) {
        return;
    }

    if (jogador->vidas <= 0) {
        tocarSomGameOver(); // Som de game over
        pararMusicaPrincipal(); // Parar música de fundo
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

    if (mapa->dados[novoY][novoX] == 'C') {
        tocarSomCura(); // Tocar som de cura
        jogador->vidas++;
        mapa->dados[novoY][novoX] = '.';
    }

    if (mapa->dados[novoY][novoX] >= '1' && mapa->dados[novoY][novoX] <= '9') {
        verificaPortalNumerado(jogador, mapa, novoY, novoX);
        return;
    }

    // Lógica para coletar tesouro
    if (confereTesouro(mapa, novoX, novoY)) {
        tocarSomTesouro(); // Tocar som de coleta de tesouro
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

// Função O(1) usando a estrutura de portais - MUITO mais eficiente!
char verificaPortalNaPosicao(Mapa *mapa, int x, int y) {
    // Verificar se a posição (x,y) corresponde a algum portal nos pares armazenados
    for (int i = 0; i < mapa->totalPortais; i++) {
        if ((mapa->portais[i].x1 == x && mapa->portais[i].y1 == y) ||
            (mapa->portais[i].x2 == x && mapa->portais[i].y2 == y)) {
            return mapa->portais[i].numero;
        }
    }
    return '.'; // Não é um portal
}

// Função O(1) para teleporte usando pares pré-calculados
void verificaPortalNumerado(Jogador *jogador, Mapa *mapa, int y, int x) {
    // Buscar o par do portal na estrutura - O(1) amortizado
    for (int i = 0; i < mapa->totalPortais; i++) {
        if (mapa->portais[i].x1 == x && mapa->portais[i].y1 == y) {
            // Teleportar para a segunda posição
            tocarSomTeleporte(); // Som de teletransporte
            mapa->dados[jogador->y][jogador->x] = '.';
            jogador->x = mapa->portais[i].x2;
            jogador->y = mapa->portais[i].y2;
            return;
        } else if (mapa->portais[i].x2 == x && mapa->portais[i].y2 == y) {
            // Teleportar para a primeira posição
            tocarSomTeleporte(); // Som de teletransporte
            mapa->dados[jogador->y][jogador->x] = '.';
            jogador->x = mapa->portais[i].x1;
            jogador->y = mapa->portais[i].y1;
            return;
        }
    }
}