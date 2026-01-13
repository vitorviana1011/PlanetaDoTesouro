#include "includes/desenhos.h"
#include "includes/manipulaArquivos.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Texturas carregadas a partir de recursos/sprites
static Texture2D texBackground = {0};
static Texture2D texLogo = {0};
static bool recursos_carregados = false;

void carregarRecursos(void) {
    if (recursos_carregados) return;

    // Tentativa de carregar texturas do diretório recursos/sprites
    texBackground = LoadTexture("recursos/sprites/background.png");
    texLogo = LoadTexture("recursos/sprites/logo.png");

    // Marca como carregado mesmo que alguma textura falhe (tratamento em desenho)
    recursos_carregados = true;
}

void liberarRecursos(void) {
    if (!recursos_carregados) return;
    if (texBackground.id) UnloadTexture(texBackground);
    if (texLogo.id) UnloadTexture(texLogo);
    texBackground = (Texture2D){0};
    texLogo = (Texture2D){0};
    recursos_carregados = false;
}

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
                case '#': color = BLACK; break;
                case '.': color = GRAY; break;
                case '@': color = BLUE; break;
                case 'T': color = YELLOW; break;
                case 'I': color = GRAY; break;
                case 'C': color = GREEN; break;
                case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    color = PURPLE; break;
                default: color = LIGHTGRAY; break;
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
        if (((int)(GetTime() * 10)) % 2 == 0) {
            DrawRectangle(x, y, tileSize, tileSize, SKYBLUE);
        } else {
            DrawRectangle(x, y, tileSize, tileSize, BLUE);
        }
    } else {
        DrawRectangle(x, y, tileSize, tileSize, BLUE);
    }
}

void desenhaHUDJogo(int fase, int tesouroColetados, Mapa mapa, Jogador jogador, Cronometro *cronometro) {
    DrawText(TextFormat("Fase: %d", fase), 10, 10, 20, WHITE);
    DrawText(TextFormat("Tesouros: %d/%d", tesouroColetados, mapa.totalTesouros), 10, 35, 20, WHITE);
    DrawText(TextFormat("Vidas: %d", jogador.vidas), 10, 60, 20, WHITE);
    
    double tempoAtual = GetTime();
    double tempoFaseAtual = tempoAtual - cronometro->inicioFase;
    double tempoTotalAtual = tempoAtual - cronometro->inicioJogo;
    
    DrawText(TextFormat("Tempo: %.1fs", tempoFaseAtual), SCREEN_WIDTH - 150, 10, 18, LIGHTGRAY);
    DrawText(TextFormat("Total: %.1fs", tempoTotalAtual), SCREEN_WIDTH - 150, 30, 16, DARKGRAY);
}

void desenhaTelaResultados(Cronometro *cronometro) {
    ClearBackground(DARKGREEN);
    
    DrawText("🎉 PARABÉNS! 🎉", SCREEN_WIDTH / 2 - MeasureText("🎉 PARABÉNS! 🎉", 50) / 2, 100, 50, GOLD);
    DrawText("JOGO COMPLETO!", SCREEN_WIDTH / 2 - MeasureText("JOGO COMPLETO!", 40) / 2, 170, 40, WHITE);
    
    DrawText("📊 ESTATÍSTICAS FINAIS", SCREEN_WIDTH / 2 - MeasureText("📊 ESTATÍSTICAS FINAIS", 30) / 2, 250, 30, LIGHTGRAY);
    
    char tempoTotal[100];
    snprintf(tempoTotal, sizeof(tempoTotal), "⏱️  Tempo Total: %.2f segundos", cronometro->tempoTotal);
    DrawText(tempoTotal, SCREEN_WIDTH / 2 - MeasureText(tempoTotal, 25) / 2, 320, 25, YELLOW);
    
    int fasesCompletas = 0;
    for (int i = 0; i < MAX_FASES; i++) {
        if (cronometro->temposPorFase[i] > 0) {
            fasesCompletas = i + 1;
        }
    }
    
    char fases[100];
    snprintf(fases, sizeof(fases), "🏁 Fases Completadas: %d", fasesCompletas);
    DrawText(fases, SCREEN_WIDTH / 2 - MeasureText(fases, 25) / 2, 360, 25, LIME);
    
    DrawText("📈 TEMPOS POR FASE:", 200, 420, 22, LIGHTGRAY);
    int yPos = 450;
    for (int i = 0; i < fasesCompletas && i < 6; i++) {
        char tempoFase[50];
        snprintf(tempoFase, sizeof(tempoFase), "Fase %d: %.2fs", i + 1, cronometro->temposPorFase[i]);
        DrawText(tempoFase, 220, yPos, 18, WHITE);
        yPos += 25;
    }
    
    DrawText("Pressione ENTER para registrar seu nome", SCREEN_WIDTH / 2 - MeasureText("Pressione ENTER para registrar seu nome", 22) / 2, SCREEN_HEIGHT - 100, 22, GREEN);
    DrawText("e visualizar o ranking!", SCREEN_WIDTH / 2 - MeasureText("e visualizar o ranking!", 20) / 2, SCREEN_HEIGHT - 70, 20, GREEN);
}

void desenhaTelaInputNome(char *nomeJogador, Cronometro *cronometro) {
    ClearBackground(DARKGREEN);
    
    DrawText("NOVO RECORDE!", SCREEN_WIDTH / 2 - MeasureText("NOVO RECORDE!", 40) / 2, 100, 40, GOLD);
    
    char textoTempo[100];
    snprintf(textoTempo, sizeof(textoTempo), "Seu tempo: %.2f segundos", cronometro->tempoTotal);
    DrawText(textoTempo, SCREEN_WIDTH / 2 - MeasureText(textoTempo, 25) / 2, 170, 25, WHITE);
    
    DrawText("Digite seu nome:", SCREEN_WIDTH / 2 - MeasureText("Digite seu nome:", 25) / 2, 250, 25, LIGHTGRAY);
    
    Rectangle textBox = { SCREEN_WIDTH/2 - 150, 300, 300, 50 };
    DrawRectangleRec(textBox, LIGHTGRAY);
    DrawRectangleLinesEx(textBox, 3, DARKGRAY);
    
    DrawText(nomeJogador, textBox.x + 10, textBox.y + 15, 20, BLACK);
    
    if (((int)(GetTime() * 2)) % 2 == 0) {
        int textWidth = MeasureText(nomeJogador, 20);
        DrawText("|", textBox.x + 10 + textWidth, textBox.y + 15, 20, BLACK);
    }
    
    DrawText("Pressione ENTER para salvar", SCREEN_WIDTH / 2 - MeasureText("Pressione ENTER para salvar", 20) / 2, 400, 20, YELLOW);
    DrawText("Pressione ESC para usar nome 'Anônimo'", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para usar nome 'Anônimo'", 18) / 2, 430, 18, GRAY);
}

void desenhaRanking(void) {
    float tempo = GetTime();
    
    // Background animado deslizando infinitamente na diagonal (mesmo do menu)
    if (recursos_carregados && texBackground.id) {
        // Velocidade do movimento diagonal (em pixels por segundo)
        float velocidade = 30.0f;
        
        // Calcular offset baseado no tempo para movimento contínuo
        float offsetX = fmod(tempo * velocidade, texBackground.width);
        float offsetY = fmod(tempo * velocidade * 0.7f, texBackground.height);
        
        // Desenhar múltiplas cópias do background para preencher a tela
        for (int x = -1; x <= 2; x++) {
            for (int y = -1; y <= 2; y++) {
                Rectangle src = { 0.0f, 0.0f, (float)texBackground.width, (float)texBackground.height };
                Rectangle dest = { 
                    x * texBackground.width - offsetX, 
                    y * texBackground.height - offsetY, 
                    (float)texBackground.width, 
                    (float)texBackground.height 
                };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(texBackground, src, dest, origin, 0.0f, WHITE);
            }
        }
    } else {
        ClearBackground(DARKBLUE);
    }
    
    // Título com movimento flutuante
    float flutuacao = sin(tempo * 1.5f) * 5.0f;
    DrawText("🏆 RANKING 🏆", SCREEN_WIDTH / 2 - MeasureText("🏆 RANKING 🏆", 30) / 2, 40 + flutuacao, 30, GOLD);

    // Retângulo principal com estilo madeira (centralizado)
    Color corMadeira = (Color){139, 69, 19, 255};
    Color corBorda = (Color){255, 215, 0, 255};
    
    // Dimensões centralizadas do retângulo
    float larguraRetangulo = 800.0f;
    float alturaRetangulo = 500.0f;
    float centroX = SCREEN_WIDTH / 2.0f - larguraRetangulo / 2.0f;
    float centroY = SCREEN_HEIGHT / 2.0f - alturaRetangulo / 2.0f + 30.0f; // +30 para ajustar com o título
    
    Rectangle retanguloRanking = {centroX, centroY, larguraRetangulo, alturaRetangulo};
    
    // Sombra do retângulo
    Rectangle sombra = {retanguloRanking.x + 5, retanguloRanking.y + 5, retanguloRanking.width, retanguloRanking.height};
    DrawRectangleRec(sombra, (Color){0, 0, 0, 100});
    
    // Fundo madeira
    DrawRectangleRec(retanguloRanking, corMadeira);
    
    // Borda dourada
    DrawRectangleLinesEx(retanguloRanking, 4.0f, corBorda);
    
    // Carregar dados do ranking
    FILE *arquivo = fopen("ranking.bin", "rb");
    if (arquivo == NULL) {
        // Caso não haja dados
        int yPos = (int)(retanguloRanking.y + retanguloRanking.height / 2 - 40);
        
        const char* mensagem = "Nenhum tempo registrado ainda.";
        int larguraTexto = MeasureText(mensagem, 24);
        int textoX = (int)(retanguloRanking.x + (retanguloRanking.width - larguraTexto) / 2);
        
        DrawText(mensagem, textoX, yPos, 24, WHITE);
        
        const char* instrucao = "Complete o jogo para aparecer no ranking!";
        larguraTexto = MeasureText(instrucao, 20);
        textoX = (int)(retanguloRanking.x + (retanguloRanking.width - larguraTexto) / 2);
        
        DrawText(instrucao, textoX, yPos + 40, 20, YELLOW);
    } else {
        // Carregar e ordenar recordes
        Recorde recordes[MAX_RECORDES];
        int totalRecordes = 0;
        
        while (fread(&recordes[totalRecordes], sizeof(Recorde), 1, arquivo) == 1 && 
               totalRecordes < MAX_RECORDES) {
            totalRecordes++;
        }
        fclose(arquivo);
        
        // Ordenar por tempo
        for (int i = 0; i < totalRecordes - 1; i++) {
            for (int j = 0; j < totalRecordes - i - 1; j++) {
                if (recordes[j].tempoTotal > recordes[j + 1].tempoTotal) {
                    Recorde temp = recordes[j];
                    recordes[j] = recordes[j + 1];
                    recordes[j + 1] = temp;
                }
            }
        }
        
        // Cabeçalho da tabela
        int inicioTabelaY = (int)(retanguloRanking.y + 30);
        int margemEsquerda = (int)(retanguloRanking.x + 30);
        
        DrawText("Pos", margemEsquerda, inicioTabelaY, 20, YELLOW);
        DrawText("Nome", margemEsquerda + 80, inicioTabelaY, 20, YELLOW);
        DrawText("Tempo", margemEsquerda + 280, inicioTabelaY, 20, YELLOW);
        DrawText("Fases", margemEsquerda + 400, inicioTabelaY, 20, YELLOW);
        DrawText("Data", margemEsquerda + 500, inicioTabelaY, 20, YELLOW);
        
        // Linha divisória
        DrawLine((int)(retanguloRanking.x + 20), inicioTabelaY + 25, 
                 (int)(retanguloRanking.x + retanguloRanking.width - 20), inicioTabelaY + 25, GOLD);
        
        // Exibir recordes (máximo 12 para caber na tela)
        int yPos = inicioTabelaY + 40;
        int maxRecordes = totalRecordes < 12 ? totalRecordes : 12;
        
        for (int i = 0; i < maxRecordes; i++) {
            Color cor = WHITE;
            if (i == 0) cor = GOLD;        // 1º lugar
            else if (i == 1) cor = LIGHTGRAY; // 2º lugar
            else if (i == 2) cor = ORANGE;    // 3º lugar
            
            char posStr[10];
            snprintf(posStr, sizeof(posStr), "%d°", i + 1);
            DrawText(posStr, margemEsquerda, yPos, 18, cor);
            
            // Nome (limitado a 15 caracteres)
            char nomeCompleto[MAX_NOME];
            strncpy(nomeCompleto, recordes[i].nomeJogador, sizeof(nomeCompleto) - 1);
            nomeCompleto[sizeof(nomeCompleto) - 1] = '\0';
            if (strlen(nomeCompleto) > 15) {
                nomeCompleto[12] = '.';
                nomeCompleto[13] = '.';
                nomeCompleto[14] = '.';
                nomeCompleto[15] = '\0';
            }
            DrawText(nomeCompleto, margemEsquerda + 80, yPos, 18, cor);
            
            char tempoStr[20];
            snprintf(tempoStr, sizeof(tempoStr), "%.2fs", recordes[i].tempoTotal);
            DrawText(tempoStr, margemEsquerda + 280, yPos, 18, cor);
            
            char fasesStr[10];
            snprintf(fasesStr, sizeof(fasesStr), "%d", recordes[i].fasesCompletas);
            DrawText(fasesStr, margemEsquerda + 420, yPos, 18, cor);
            
            char dataStr[20];
            strncpy(dataStr, recordes[i].dataHora, 10);
            dataStr[10] = '\0';
            DrawText(dataStr, margemEsquerda + 500, yPos, 16, cor);
            
            yPos += 30;
        }
        
        // Destacar melhor tempo se existir
        if (totalRecordes > 0) {
            int yDestaque = (int)(retanguloRanking.y + retanguloRanking.height - 80);
            DrawText("MELHOR TEMPO:", margemEsquerda, yDestaque, 22, GOLD);
            
            char detalhes[200];
            snprintf(detalhes, sizeof(detalhes), "%s - %.2fs (%d fases)", 
                    recordes[0].nomeJogador, recordes[0].tempoTotal, recordes[0].fasesCompletas);
            DrawText(detalhes, margemEsquerda, yDestaque + 30, 20, WHITE);
        }
    }
    
    // Botão VOLTAR na parte inferior
    float larguraBotao = 200.0f;
    float alturaBotao = 50.0f;
    float centroXBotao = SCREEN_WIDTH / 2.0f - larguraBotao / 2.0f;
    float posYBotao = SCREEN_HEIGHT - 100.0f;
    
    // Verificar hover do mouse no botão
    Vector2 mousePosicao = GetMousePosition();
    Rectangle botaoVoltar = {centroXBotao, posYBotao, larguraBotao, alturaBotao};
    bool hoverBotao = CheckCollisionPointRec(mousePosicao, botaoVoltar);
    
    // Cores do botão (mesmo estilo dos botões do menu)
    Color corMadeiraBotao = (Color){139, 69, 19, 255};
    Color corBordaBotao = (Color){255, 215, 0, 255};
    Color corTextoBotao = hoverBotao ? (Color){255, 215, 0, 255} : WHITE;
    
    // Efeito hover - botão sobe um pouco
    float offsetYBotao = hoverBotao ? -3.0f : 0.0f;
    Rectangle botaoRect = {centroXBotao, posYBotao + offsetYBotao, larguraBotao, alturaBotao};
    
    // Sombra do botão (apenas quando não está em hover)
    if (!hoverBotao) {
        Rectangle sombraBotao = {botaoRect.x + 3, botaoRect.y + 3, botaoRect.width, botaoRect.height};
        DrawRectangleRec(sombraBotao, (Color){0, 0, 0, 100});
    }
    
    // Fundo do botão (madeira)
    DrawRectangleRec(botaoRect, corMadeiraBotao);
    
    // Borda dourada
    DrawRectangleLinesEx(botaoRect, 3.0f, corBordaBotao);
    
    // Texto centralizado no botão
    const char* textoBotao = "VOLTAR";
    int tamanhoTextoBotao = 22;
    int larguraTextoBotao = MeasureText(textoBotao, tamanhoTextoBotao);
    float textoXBotao = botaoRect.x + (botaoRect.width - larguraTextoBotao) / 2;
    float textoYBotao = botaoRect.y + (botaoRect.height - tamanhoTextoBotao) / 2;
    
    DrawText(textoBotao, (int)textoXBotao, (int)textoYBotao, tamanhoTextoBotao, corTextoBotao);
}

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

void desenhaBotaoMenu(BotaoMenu* botao) {
    // Cores do botão
    Color corMadeira = (Color){139, 69, 19, 255};        // Marrom madeira
    Color corBorda = (Color){255, 215, 0, 255};          // Dourado
    Color corTexto = botao->hover ? (Color){255, 215, 0, 255} : WHITE;  // Branco normal, dourado no hover
    
    // Ajuste de posição para efeito hover (botão sobe um pouco)
    float offsetY = botao->hover ? -5.0f : 0.0f;
    Rectangle botaoRect = {
        botao->bounds.x, 
        botao->bounds.y + offsetY, 
        botao->bounds.width, 
        botao->bounds.height
    };
    
    // Sombra do botão (apenas quando não está em hover)
    if (!botao->hover) {
        Rectangle sombra = {botaoRect.x + 3, botaoRect.y + 3, botaoRect.width, botaoRect.height};
        DrawRectangleRec(sombra, (Color){0, 0, 0, 100});
    }
    
    // Fundo do botão (madeira)
    DrawRectangleRec(botaoRect, corMadeira);
    
    // Borda dourada
    DrawRectangleLinesEx(botaoRect, 3.0f, corBorda);
    
    // Texto centralizado
    int tamanhoTexto = 24;
    int larguraTexto = MeasureText(botao->texto, tamanhoTexto);
    float textoX = botaoRect.x + (botaoRect.width - larguraTexto) / 2;
    float textoY = botaoRect.y + (botaoRect.height - tamanhoTexto) / 2;
    
    DrawText(botao->texto, (int)textoX, (int)textoY, tamanhoTexto, corTexto);
}

void atualizaHoverBotao(BotaoMenu* botao) {
    Vector2 mousePosicao = GetMousePosition();
    botao->hover = CheckCollisionPointRec(mousePosicao, botao->bounds);
}

AcaoBotaoMenu verificaCliqueBotoesMenu(void) {
    // Verificar se o botão esquerdo do mouse foi pressionado
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return ACAO_NENHUMA;
    }
    
    Vector2 mousePosicao = GetMousePosition();
    
    // Dimensões dos botões
    float larguraBotao = 250.0f;
    float alturaBotao = 60.0f;
    float centroX = SCREEN_WIDTH / 2.0f - larguraBotao / 2.0f;
    float espacamento = 80.0f;
    float inicioY = SCREEN_HEIGHT / 2.0f + 50.0f;
    
    // Verificar clique em cada botão
    Rectangle botaoIniciar = {centroX, inicioY, larguraBotao, alturaBotao};
    Rectangle botaoRanking = {centroX, inicioY + espacamento, larguraBotao, alturaBotao};
    Rectangle botaoTutorial = {centroX, inicioY + espacamento * 2, larguraBotao, alturaBotao};
    
    if (CheckCollisionPointRec(mousePosicao, botaoIniciar)) {
        return ACAO_INICIAR;
    }
    if (CheckCollisionPointRec(mousePosicao, botaoRanking)) {
        return ACAO_RANKING;
    }
    if (CheckCollisionPointRec(mousePosicao, botaoTutorial)) {
        return ACAO_TUTORIAL;
    }
    
    return ACAO_NENHUMA;
}

void desenhaTelaMenu(void) {
    // Botões do menu (static para manter estado)
    static BotaoMenu botoes[3] = {0};
    static bool botoesInicializados = false;
    
    // Inicializar botões apenas uma vez
    if (!botoesInicializados) {
        // Dimensões dos botões
        float larguraBotao = 250.0f;
        float alturaBotao = 60.0f;
        float centroX = SCREEN_WIDTH / 2.0f - larguraBotao / 2.0f;
        float espacamento = 80.0f;
        float inicioY = SCREEN_HEIGHT / 2.0f + 50.0f;
        
        // Configurar botões
        botoes[0] = (BotaoMenu){{centroX, inicioY, larguraBotao, alturaBotao}, "INICIAR", false};
        botoes[1] = (BotaoMenu){{centroX, inicioY + espacamento, larguraBotao, alturaBotao}, "RANKING", false};
        botoes[2] = (BotaoMenu){{centroX, inicioY + espacamento * 2, larguraBotao, alturaBotao}, "TUTORIAL", false};
        
        botoesInicializados = true;
    }
    
    float tempo = GetTime();
    
    // Background animado deslizando infinitamente na diagonal
    if (recursos_carregados && texBackground.id) {
        // Velocidade do movimento diagonal (em pixels por segundo)
        float velocidade = 30.0f;
        
        // Calcular offset baseado no tempo para movimento contínuo
        float offsetX = fmod(tempo * velocidade, texBackground.width);
        float offsetY = fmod(tempo * velocidade * 0.7f, texBackground.height);
        
        // Desenhar múltiplas cópias do background para preencher a tela
        for (int x = -1; x <= 2; x++) {
            for (int y = -1; y <= 2; y++) {
                Rectangle src = { 0.0f, 0.0f, (float)texBackground.width, (float)texBackground.height };
                Rectangle dest = { 
                    x * texBackground.width - offsetX, 
                    y * texBackground.height - offsetY, 
                    (float)texBackground.width, 
                    (float)texBackground.height 
                };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(texBackground, src, dest, origin, 0.0f, WHITE);
            }
        }
    } else {
        ClearBackground(DARKBLUE);
    }

    // Logo flutuante com movimento suave de subir/descer
    if (recursos_carregados && texLogo.id) {
        float targetW = SCREEN_WIDTH * 0.6f;
        float scale = targetW / texLogo.width;
        float targetH = texLogo.height * scale;
        
        // Movimento flutuante: oscila 20 pixels para cima e para baixo
        float flutuacao = sin(tempo * 1.5f) * 10.0f;
        float posY = 80.0f + flutuacao;
        
        Rectangle src = { 0.0f, 0.0f, (float)texLogo.width, (float)texLogo.height };
        Rectangle dest = { SCREEN_WIDTH/2.0f - targetW/2.0f, posY, targetW, targetH };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(texLogo, src, dest, origin, 0.0f, WHITE);
    } else {
        // Texto também pode flutuar se não há logo
        float flutuacao = sin(tempo * 1.5f) * 10.0f;
        DrawText("PLANETA DO TESOURO", SCREEN_WIDTH / 2 - MeasureText("PLANETA DO TESOURO", 40) / 2, 200 + flutuacao, 40, GOLD);
    }

    // Atualizar e desenhar botões
    for (int i = 0; i < 3; i++) {
        atualizaHoverBotao(&botoes[i]);
        desenhaBotaoMenu(&botoes[i]);
    }

    // Instruções na parte inferior
    float pulso = 0.8f + sin(tempo * 3.0f) * 0.2f;
    Color corPrincipal = ColorAlpha(WHITE, pulso);
    
    DrawText("Use o mouse para navegar", SCREEN_WIDTH / 2 - MeasureText("Use o mouse para navegar", 20) / 2, SCREEN_HEIGHT - 100, 20, corPrincipal);
    DrawText("Pressione ESC para sair", SCREEN_WIDTH / 2 - MeasureText("Pressione ESC para sair", 18) / 2, SCREEN_HEIGHT - 70, 18, LIGHTGRAY);
}

bool verificaCliqueBotaoTutorial(void) {
    // Verificar se o botão esquerdo do mouse foi pressionado
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return false;
    }
    
    Vector2 mousePosicao = GetMousePosition();
    
    // Dimensões do botão voltar (mesmo estilo dos botões do menu)
    float larguraBotao = 200.0f;
    float alturaBotao = 50.0f;
    float centroX = SCREEN_WIDTH / 2.0f - larguraBotao / 2.0f;
    float posY = SCREEN_HEIGHT - 100.0f; // 100px da parte inferior
    
    Rectangle botaoVoltar = {centroX, posY, larguraBotao, alturaBotao};
    
    return CheckCollisionPointRec(mousePosicao, botaoVoltar);
}

bool verificaCliqueBotaoRanking(void) {
    // Verificar se o botão esquerdo do mouse foi pressionado
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return false;
    }
    
    Vector2 mousePosicao = GetMousePosition();
    
    // Dimensões do botão voltar (mesmo estilo dos botões do menu)
    float larguraBotao = 200.0f;
    float alturaBotao = 50.0f;
    float centroX = SCREEN_WIDTH / 2.0f - larguraBotao / 2.0f;
    float posY = SCREEN_HEIGHT - 100.0f; // 100px da parte inferior
    
    Rectangle botaoVoltar = {centroX, posY, larguraBotao, alturaBotao};
    
    return CheckCollisionPointRec(mousePosicao, botaoVoltar);
}

void desenhaTelaTutorial(void) {
    float tempo = GetTime();
    
    // Background animado deslizando infinitamente na diagonal (mesmo do menu)
    if (recursos_carregados && texBackground.id) {
        // Velocidade do movimento diagonal (em pixels por segundo)
        float velocidade = 30.0f;
        
        // Calcular offset baseado no tempo para movimento contínuo
        float offsetX = fmod(tempo * velocidade, texBackground.width);
        float offsetY = fmod(tempo * velocidade * 0.7f, texBackground.height);
        
        // Desenhar múltiplas cópias do background para preencher a tela
        for (int x = -1; x <= 2; x++) {
            for (int y = -1; y <= 2; y++) {
                Rectangle src = { 0.0f, 0.0f, (float)texBackground.width, (float)texBackground.height };
                Rectangle dest = { 
                    x * texBackground.width - offsetX, 
                    y * texBackground.height - offsetY, 
                    (float)texBackground.width, 
                    (float)texBackground.height 
                };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(texBackground, src, dest, origin, 0.0f, WHITE);
            }
        }
    } else {
        ClearBackground(DARKBLUE);
    }
    
    // Título com movimento flutuante
    float flutuacao = sin(tempo * 1.5f) * 5.0f;
    DrawText("TUTORIAL DO JOGO", SCREEN_WIDTH / 2 - MeasureText("TUTORIAL DO JOGO", 30) / 2, 40 + flutuacao, 30, GOLD);

    // Retângulo principal com estilo madeira (centralizado)
    Color corMadeira = (Color){139, 69, 19, 255};
    Color corBorda = (Color){255, 215, 0, 255};
    
    // Dimensões centralizadas do retângulo
    float larguraRetangulo = 700.0f;
    float alturaRetangulo = 450.0f;
    float centroX = SCREEN_WIDTH / 2.0f - larguraRetangulo / 2.0f;
    float centroY = SCREEN_HEIGHT / 2.0f - alturaRetangulo / 2.0f + 30.0f; // +30 para ajustar com o título
    
    Rectangle retanguloTutorial = {centroX, centroY, larguraRetangulo, alturaRetangulo};
    
    // Sombra do retângulo
    Rectangle sombra = {retanguloTutorial.x + 5, retanguloTutorial.y + 5, retanguloTutorial.width, retanguloTutorial.height};
    DrawRectangleRec(sombra, (Color){0, 0, 0, 100});
    
    // Fundo madeira
    DrawRectangleRec(retanguloTutorial, corMadeira);
    
    // Borda dourada
    DrawRectangleLinesEx(retanguloTutorial, 4.0f, corBorda);
    
    const char *instrucoes[] = {
        "Objetivo: Coletar todos os tesouros e chegar ao portal de saída.",
        "Controles:",
        "- Use as setas do teclado para mover o seu personagem.",
        "- Evite os inimigos que patrulham o mapa.",
        "- Use portais para teletransportar-se entre eles.",
        "Dicas:",
        "- Fique atento ao tempo e suas vidas!",
        "Boa sorte, aventureiro!"
    };
    
    // Texto centralizado dentro do retângulo
    int yPos = (int)(retanguloTutorial.y + 30); // Margem superior dentro do retângulo
    int totalInstrucoes = sizeof(instrucoes)/sizeof(instrucoes[0]);
    for (int i = 0; i < totalInstrucoes; i++) {
        // Texto com cor mais clara para contraste com fundo madeira
        Color corTexto = (i == 1 || i == 5) ? YELLOW : WHITE; // Destacar "Controles:" e "Dicas:"
        
        // Centralizar cada linha de texto
        int larguraTexto = MeasureText(instrucoes[i], 20);
        int textoX = (int)(retanguloTutorial.x + (retanguloTutorial.width - larguraTexto) / 2);
        
        DrawText(instrucoes[i], textoX, yPos, 20, corTexto);
        yPos += 40; // Espaçamento entre linhas
    }
    
    // Botão VOLTAR na parte inferior
    float larguraBotao = 200.0f;
    float alturaBotao = 50.0f;
    float centroXBotao = SCREEN_WIDTH / 2.0f - larguraBotao / 2.0f;
    float posYBotao = SCREEN_HEIGHT - 100.0f;
    
    // Verificar hover do mouse no botão
    Vector2 mousePosicao = GetMousePosition();
    Rectangle botaoVoltar = {centroXBotao, posYBotao, larguraBotao, alturaBotao};
    bool hoverBotao = CheckCollisionPointRec(mousePosicao, botaoVoltar);
    
    // Cores do botão (mesmo estilo dos botões do menu)
    Color corMadeiraBotao = (Color){139, 69, 19, 255};
    Color corBordaBotao = (Color){255, 215, 0, 255};
    Color corTextoBotao = hoverBotao ? (Color){255, 215, 0, 255} : WHITE;
    
    // Efeito hover - botão sobe um pouco
    float offsetYBotao = hoverBotao ? -3.0f : 0.0f;
    Rectangle botaoRect = {centroXBotao, posYBotao + offsetYBotao, larguraBotao, alturaBotao};
    
    // Sombra do botão (apenas quando não está em hover)
    if (!hoverBotao) {
        Rectangle sombraBotao = {botaoRect.x + 3, botaoRect.y + 3, botaoRect.width, botaoRect.height};
        DrawRectangleRec(sombraBotao, (Color){0, 0, 0, 100});
    }
    
    // Fundo do botão (madeira)
    DrawRectangleRec(botaoRect, corMadeiraBotao);
    
    // Borda dourada
    DrawRectangleLinesEx(botaoRect, 3.0f, corBordaBotao);
    
    // Texto centralizado no botão
    const char* textoBotao = "VOLTAR";
    int tamanhoTextoBotao = 22;
    int larguraTextoBotao = MeasureText(textoBotao, tamanhoTextoBotao);
    float textoXBotao = botaoRect.x + (botaoRect.width - larguraTextoBotao) / 2;
    float textoYBotao = botaoRect.y + (botaoRect.height - tamanhoTextoBotao) / 2;
    
    DrawText(textoBotao, (int)textoXBotao, (int)textoYBotao, tamanhoTextoBotao, corTextoBotao);
}