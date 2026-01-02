#include "includes/raylib.h"
#include "includes/manipulaArquivos.h"
#include "includes/logicaJogo.h"
#include "includes/inimigo.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 1000

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

    if (mapa->dados[novoY][novoX] == 'C') {
        jogador->vidas++;
        mapa->dados[novoY][novoX] = '.';
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
            mapa->dados[jogador->y][jogador->x] = '.';
            jogador->x = mapa->portais[i].x2;
            jogador->y = mapa->portais[i].y2;
            return;
        } else if (mapa->portais[i].x2 == x && mapa->portais[i].y2 == y) {
            // Teleportar para a primeira posição
            mapa->dados[jogador->y][jogador->x] = '.';
            jogador->x = mapa->portais[i].x1;
            jogador->y = mapa->portais[i].y1;
            return;
        }
    }
}

void desenhaTelaResultados(Cronometro *cronometro) {
    ClearBackground(DARKBLUE);
    
    // Título principal
    DrawText("🏁 JOGO COMPLETO! 🏁", SCREEN_WIDTH / 2 - MeasureText("🏁 JOGO COMPLETO! 🏁", 40) / 2, 50, 40, GOLD);
    
    // Tempo total
    char textoTempo[100];
    snprintf(textoTempo, sizeof(textoTempo), "Tempo Total: %.2f segundos", cronometro->tempoTotal);
    DrawText(textoTempo, SCREEN_WIDTH / 2 - MeasureText(textoTempo, 30) / 2, 120, 30, WHITE);
    
    // Detalhes por fase
    DrawText("Tempos por Fase:", 50, 180, 25, LIME);
    
    int yPos = 220;
    for (int i = 0; i < MAX_FASES && yPos < SCREEN_HEIGHT - 100; i++) {
        if (cronometro->temposPorFase[i] > 0) {
            char textoFase[100];
            snprintf(textoFase, sizeof(textoFase), "Fase %d: %.2f segundos", i + 1, cronometro->temposPorFase[i]);
            DrawText(textoFase, 70, yPos, 20, LIGHTGRAY);
            yPos += 25;
        }
    }
    
    // Instruções
    DrawText("Pressione ENTER para inserir seu nome", SCREEN_WIDTH / 2 - MeasureText("Pressione ENTER para inserir seu nome", 20) / 2, SCREEN_HEIGHT - 80, 20, YELLOW);
    DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para sair", 20) / 2, SCREEN_HEIGHT - 50, 20, GRAY);
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
    DrawText("Pressione BACKSPACE para apagar", SCREEN_WIDTH / 2 - MeasureText("Pressione BACKSPACE para apagar", 18) / 2, 430, 18, GRAY);
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