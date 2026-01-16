#include "includes/desenhos.h"
#include "includes/manipulaArquivos.h"
#include "includes/audio.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// Texturas carregadas a partir de recursos/sprites
static Texture2D texBackground = {0};
static Texture2D texLogo = {0};
static Texture2D texJogador = {0};
static Texture2D texInimigo = {0};
static Texture2D texTesouro = {0};
static Texture2D texVida = {0};
static Texture2D texPortal = {0};
static Texture2D texChao = {0};
static Texture2D texParede = {0};

static bool recursos_carregados = false;

void carregarRecursos(void) {
    if (recursos_carregados) return;

    // Inicializar e carregar recursos de áudio
    inicializarSistemaAudio();
    carregarRecursosAudio();

    // Tentativa de carregar texturas do diretório recursos/sprites
    texBackground = LoadTexture("recursos/sprites/background.png");
    texLogo = LoadTexture("recursos/sprites/logo.png");
    texJogador = LoadTexture("recursos/sprites/jogador.png");
    texInimigo = LoadTexture("recursos/sprites/inimigo.png");
    texTesouro = LoadTexture("recursos/sprites/tesouro.png");
    texVida = LoadTexture("recursos/sprites/vida.png");
    texPortal = LoadTexture("recursos/sprites/portal.png");
    texChao = LoadTexture("recursos/sprites/chao.png");
    texParede = LoadTexture("recursos/sprites/parede.png");

    // Marca como carregado mesmo que alguma textura falhe (tratamento em desenho)
    recursos_carregados = true;
}

void liberarRecursos(void) {
    if (!recursos_carregados) return;
    
    // Liberar texturas
    if (texBackground.id) UnloadTexture(texBackground);
    if (texLogo.id) UnloadTexture(texLogo);
    if (texJogador.id) UnloadTexture(texJogador);
    if (texInimigo.id) UnloadTexture(texInimigo);
    if (texTesouro.id) UnloadTexture(texTesouro);
    if (texVida.id) UnloadTexture(texVida);
    if (texPortal.id) UnloadTexture(texPortal);
    if (texChao.id) UnloadTexture(texChao);
    if (texParede.id) UnloadTexture(texParede);
    
    // Liberar recursos de áudio
    liberarRecursosAudio();
    
    // Reset das estruturas de textura
    texBackground = (Texture2D){0};
    texLogo = (Texture2D){0};
    texJogador = (Texture2D){0};
    texInimigo = (Texture2D){0};
    texTesouro = (Texture2D){0};
    texVida = (Texture2D){0};
    texPortal = (Texture2D){0};
    texChao = (Texture2D){0};
    texParede = (Texture2D){0};
    recursos_carregados = false;
}

void desenhaMapa(Mapa mapa) {
    // Desenhar background primeiro (antes de qualquer outro elemento)
    if (recursos_carregados && texBackground.id) {
        // Calcular quantas cópias precisamos para cobrir toda a tela
        int bgWidth = texBackground.width;
        int bgHeight = texBackground.height;
        int tilesX = (SCREEN_WIDTH / bgWidth) + 2; // +2 para garantir cobertura total
        int tilesY = (SCREEN_HEIGHT / bgHeight) + 2;
        
        for (int x = -1; x < tilesX; x++) {
            for (int y = -1; y < tilesY; y++) {
                Rectangle src = { 0.0f, 0.0f, (float)bgWidth, (float)bgHeight };
                Rectangle dest = { 
                    (float)(x * bgWidth), 
                    (float)(y * bgHeight), 
                    (float)bgWidth, 
                    (float)bgHeight 
                };
                Vector2 origin = { 0.0f, 0.0f };
                DrawTexturePro(texBackground, src, dest, origin, 0.0f, WHITE);
            }
        }
    } else {
        // Fallback para cor sólida se background não carregar
        ClearBackground(DARKBLUE);
    }

    int tileSize = 40;  // Ajustado para mapa de 24 colunas (24*40=960px)
    int mapaLargura = mapa.colunas * tileSize;
    int mapaAltura = mapa.linhas * tileSize;
    
    int offsetX = (SCREEN_WIDTH - mapaLargura) / 2;
    int offsetY = (SCREEN_HEIGHT - mapaAltura) / 2;
    
    for (int i = 0; i < mapa.linhas; i++) {
        for (int j = 0; j < mapa.colunas; j++) {
            char tile = mapa.dados[i][j];
            int x = offsetX + j * tileSize;
            int y = offsetY + i * tileSize;
            
            Rectangle src = { 0.0f, 0.0f, 32.0f, 32.0f }; // Assumindo sprites 32x32
            Rectangle dest = { (float)x, (float)y, (float)tileSize, (float)tileSize };
            Vector2 origin = { 0.0f, 0.0f };
            
            switch (tile) {
                case '#': // Parede
                    if (recursos_carregados && texParede.id) {
                        // Usar textura inteira da parede
                        Rectangle srcParede = { 0.0f, 0.0f, (float)texParede.width, (float)texParede.height };
                        DrawTexturePro(texParede, srcParede, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, BLACK);
                    }
                    break;
                case '.': // Chão
                    if (recursos_carregados && texChao.id) {
                        // Usar textura inteira do chão
                        Rectangle srcChao = { 0.0f, 0.0f, (float)texChao.width, (float)texChao.height };
                        DrawTexturePro(texChao, srcChao, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, GRAY);
                    }
                    break;
                case '@': // Posição inicial do jogador (desenhar como chão)
                    if (recursos_carregados && texChao.id) {
                        // Usar textura completa do chão
                        Rectangle srcChao = { 0.0f, 0.0f, (float)texChao.width, (float)texChao.height };
                        DrawTexturePro(texChao, srcChao, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, GRAY);
                    }
                    break;
                case 'T': // Tesouro
                    // Desenhar chão primeiro
                    if (recursos_carregados && texChao.id) {
                        Rectangle srcChao = { 0.0f, 0.0f, (float)texChao.width, (float)texChao.height };
                        DrawTexturePro(texChao, srcChao, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, GRAY);
                    }
                    // Desenhar tesouro por cima
                    if (recursos_carregados && texTesouro.id) {
                        src.width = (float)texTesouro.width;
                        src.height = (float)texTesouro.height;
                        DrawTexturePro(texTesouro, src, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, YELLOW);
                    }
                    break;
                case 'I': // Posição do inimigo (desenhar como chão)
                    if (recursos_carregados && texChao.id) {
                        Rectangle srcChao = { 0.0f, 0.0f, (float)texChao.width, (float)texChao.height };
                        DrawTexturePro(texChao, srcChao, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, GRAY);
                    }
                    break;
                case 'C': // Item de vida/cura
                    // Desenhar chão por baixo
                    if (recursos_carregados && texChao.id) {
                        Rectangle srcChao = { 0.0f, 0.0f, (float)texChao.width, (float)texChao.height };
                        DrawTexturePro(texChao, srcChao, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, GRAY);
                    }
                    // Desenhar item de vida por cima
                    if (recursos_carregados && texVida.id) {
                        src.width = (float)texVida.width;
                        src.height = (float)texVida.height;
                        DrawTexturePro(texVida, src, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, GREEN);
                    }
                    break;
                case '1': case '2': case '3': case '4': case '5': 
                case '6': case '7': case '8': case '9': // Portais numerados
                    // Desenhar chão por baixo
                    if (recursos_carregados && texChao.id) {
                        Rectangle srcChao = { 0.0f, 0.0f, (float)texChao.width, (float)texChao.height };
                        DrawTexturePro(texChao, srcChao, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, GRAY);
                    }
                    // Desenhar portal animado por cima
                    if (recursos_carregados && texPortal.id) {
                        // Animação do portal: 3 frames horizontais (32x32 cada)
                        // Sprite do portal: 96x64 (3 frames x 2 linhas)
                        float tempo = GetTime();
                        int frameAtual = (int)(tempo * 4.0f) % 3; // 4 FPS, 3 frames
                        
                        Rectangle srcPortal = { 
                            frameAtual * 32.0f, // X: 0, 32, ou 64
                            0.0f,               // Y: sempre primeira linha
                            32.0f,              // Largura do frame
                            32.0f               // Altura do frame
                        };
                        DrawTexturePro(texPortal, srcPortal, dest, origin, 0.0f, WHITE);
                    } else {
                        DrawRectangle(x, y, tileSize, tileSize, PURPLE);
                    }
                    break;
                default:
                    DrawRectangle(x, y, tileSize, tileSize, LIGHTGRAY);
                    break;
            }
        }
    }
}

void desenhaPortais(Mapa mapa, Jogador jogador) {
    int tileSize = 40;  // Ajustado para mapa de 24 colunas
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
    
    int tileSize = 40;  // Ajustado para mapa de 24 colunas
    int mapaLargura = mapa.colunas * tileSize;
    int mapaAltura = mapa.linhas * tileSize;
    
    int offsetX = (SCREEN_WIDTH - mapaLargura) / 2;
    int offsetY = (SCREEN_HEIGHT - mapaAltura) / 2;
    
    for (int i = 0; i < mapa.totalInimigos; i++) {
        int x = offsetX + inimigos[i].x * tileSize;
        int y = offsetY + inimigos[i].y * tileSize;
        
        // Usar sprite do inimigo se disponível, senão fallback para retângulo
        if (recursos_carregados && texInimigo.id) {
            // Sprite com direção correta para 4 direções
            float frameY = 0.0f; // Padrão: para baixo (primeiro frame)
            bool flipHorizontal = false; // Para espelhamento horizontal
            
            // Definir frame baseado na direção
            switch (inimigos[i].direcao) {
                case 0: // Direita
                    frameY = 128.0f; // Segundo frame
                    flipHorizontal = false;
                    break;
                case 1: // Esquerda
                    frameY = 128.0f; // Segundo frame
                    flipHorizontal = true; // Espelhar
                    break;
                case 2: // Cima
                    frameY = 256.0f; // Terceiro frame
                    flipHorizontal = false;
                    break;
                case 3: // Baixo
                default:
                    frameY = 0.0f; // Primeiro frame
                    flipHorizontal = false;
                    break;
            }
            
            // Aumentar o tamanho do sprite (zoom)
            float spriteSize = (float)tileSize * 1.5f;
            float offsetX = (spriteSize - (float)tileSize) / 2.0f;
            float offsetY = (spriteSize - (float)tileSize) / 2.0f;
            
            Rectangle frameSource = { 0, frameY, 32, 32 };
            Rectangle frameDest = { 
                (float)x - offsetX, 
                (float)y - offsetY, 
                spriteSize, 
                spriteSize 
            };
            
            // Para espelhamento, invertemos a largura do source rectangle
            if (flipHorizontal) {
                frameSource.width = -frameSource.width;
            }
            
            Vector2 origin = { 0, 0 };
            
            DrawTexturePro(texInimigo, frameSource, frameDest, origin, 0.0f, WHITE);
        } else {
            DrawRectangle(x, y, tileSize, tileSize, ORANGE);
            DrawRectangleLines(x, y, tileSize, tileSize, BLACK);
        }
    }
}

void desenhaJogadorComEfeito(Jogador jogador, Mapa mapa) {
    int tileSize = 40;  // Ajustado para mapa de 24 colunas
    int mapaLargura = mapa.colunas * tileSize;
    int mapaAltura = mapa.linhas * tileSize;
    
    int offsetX = (SCREEN_WIDTH - mapaLargura) / 2;
    int offsetY = (SCREEN_HEIGHT - mapaAltura) / 2;
    
    int x = offsetX + jogador.x * tileSize;
    int y = offsetY + jogador.y * tileSize;
    
    // Usar sprite do jogador se disponível, senão fallback para retângulo
    if (recursos_carregados && texJogador.id) {
        // Variáveis estáticas para detectar direção do movimento
        static int posAnteriorX = -1, posAnteriorY = -1;
        static int direcao = 0; // 0=baixo, 1=esquerda, 2=direita, 3=cima
        
        // Detectar mudança de posição e atualizar direção
        if (posAnteriorX != jogador.x || posAnteriorY != jogador.y) {
            // Remapeamento correto das direções baseado na spritesheet real
            if (jogador.x > posAnteriorX) direcao = 2;      // direita -> linha 2
            else if (jogador.x < posAnteriorX) direcao = 3; // esquerda -> linha 3  
            else if (jogador.y > posAnteriorY) direcao = 0; // baixo -> linha 0
            else if (jogador.y < posAnteriorY) direcao = 1; // cima -> linha 1
            
            posAnteriorX = jogador.x;
            posAnteriorY = jogador.y;
        }
        
        // Sprite baseado na direção (sem animação, apenas direção)
        // Assumindo spritesheet: 3 frames horizontais x 4 direções verticais
        Rectangle src = { 
            0.0f,                    // X = 0 (sempre primeiro frame, sem animação)
            direcao * 15.75f,        // Y baseado na direção (0, 15.75, 31.5, 47.25)
            15.0f,                   // Largura: 15px
            15.75f                   // Altura: 15.75px (63/4 = 15.75)
        };
        Rectangle dest = { (float)x, (float)y, (float)tileSize, (float)tileSize };
        Vector2 origin = { 0.0f, 0.0f };
        
        if (jogador.tempoInvencibilidade > GetTime()) {
            // Efeito de invencibilidade: alternar entre normal e semi-transparente
            if (((int)(GetTime() * 10)) % 2 == 0) {
                DrawTexturePro(texJogador, src, dest, origin, 0.0f, WHITE);
            } else {
                DrawTexturePro(texJogador, src, dest, origin, 0.0f, ColorAlpha(WHITE, 0.5f));
            }
        } else {
            DrawTexturePro(texJogador, src, dest, origin, 0.0f, WHITE);
        }
    } else {
        // Fallback para retângulos coloridos caso o sprite não carregue
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
    DrawText(" RANKING ", SCREEN_WIDTH / 2 - MeasureText(" RANKING ", 30) / 2, 40 + flutuacao, 30, GOLD);

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
        // Fallback para cor sólida se background não carregar
        ClearBackground(DARKBLUE);
    }
    
    // Retângulo principal de madeira (mesmo estilo do tutorial/ranking)
    Rectangle retanguloMadeira = {
        SCREEN_WIDTH / 2 - 350,     // x (centralizado)
        SCREEN_HEIGHT / 2 - 200,    // y (centralizado)
        700,                        // largura
        400                         // altura
    };
    
    // Cores
    Color corMadeira = (Color){139, 69, 19, 255};        // Marrom madeira
    Color corBorda = (Color){255, 215, 0, 255};          // Dourado
    
    // Desenhar retângulo de madeira com borda dourada
    DrawRectangleRec(retanguloMadeira, corMadeira);
    DrawRectangleLinesEx(retanguloMadeira, 5, corBorda);
    
    // Título "GAME OVER!" grande e dramático
    const char* titulo = "GAME OVER!";
    int tamanhoTitulo = 50;
    int larguraTitulo = MeasureText(titulo, tamanhoTitulo);
    float xTitulo = SCREEN_WIDTH / 2 - larguraTitulo / 2;
    float yTitulo = retanguloMadeira.y + 50;
    
    // Efeito de sombra no título
    DrawText(titulo, (int)xTitulo + 3, (int)yTitulo + 3, tamanhoTitulo, BLACK);
    DrawText(titulo, (int)xTitulo, (int)yTitulo, tamanhoTitulo, RED);
    
    // Mensagem principal
    const char* mensagem = "Suas vidas acabaram!";
    int tamanhoMensagem = 25;
    int larguraMensagem = MeasureText(mensagem, tamanhoMensagem);
    float xMensagem = SCREEN_WIDTH / 2 - larguraMensagem / 2;
    float yMensagem = yTitulo + 80;
    
    DrawText(mensagem, (int)xMensagem, (int)yMensagem, tamanhoMensagem, WHITE);
    
    // Decoração adicional - linhas douradas decorativas
    float linhaY1 = yTitulo + 65;
    float linhaY2 = yMensagem + 40;
    float margemLinha = 100;
    
    DrawLineEx(
        (Vector2){retanguloMadeira.x + margemLinha, linhaY1},
        (Vector2){retanguloMadeira.x + retanguloMadeira.width - margemLinha, linhaY1},
        3.0f, corBorda
    );
    
    DrawLineEx(
        (Vector2){retanguloMadeira.x + margemLinha, linhaY2},
        (Vector2){retanguloMadeira.x + retanguloMadeira.width - margemLinha, linhaY2},
        2.0f, corBorda
    );
    
    // Botão "VOLTAR AO MENU"
    Vector2 mousePosicao = GetMousePosition();
    float larguraBotao = 250.0f;
    float alturaBotao = 60.0f;
    float centroX = SCREEN_WIDTH / 2.0f - larguraBotao / 2.0f;
    float posY = SCREEN_HEIGHT / 2 + 120;
    
    Rectangle botaoRect = {centroX, posY, larguraBotao, alturaBotao};
    bool hover = CheckCollisionPointRec(mousePosicao, botaoRect);
    
    // Cores do botão (mesmo padrão dos outros botões)
    Color corBotaoMadeira = (Color){139, 69, 19, 255};     // Marrom madeira
    Color corBordaBotao = (Color){255, 215, 0, 255};       // Dourado
    Color corTextoBotao = hover ? (Color){255, 215, 0, 255} : WHITE; // Dourado no hover, branco normal
    
    // Efeito hover - botão sobe um pouco
    if (hover) {
        botaoRect.y -= 3.0f;
    }
    
    // Desenhar o botão
    DrawRectangleRec(botaoRect, corBotaoMadeira);
    DrawRectangleLinesEx(botaoRect, 3, corBordaBotao);
    
    // Texto centralizado no botão
    const char* textoBotao = "VOLTAR AO MENU";
    int tamanhoTextoBotao = 24;
    int larguraTextoBotao = MeasureText(textoBotao, tamanhoTextoBotao);
    float textoXBotao = botaoRect.x + (botaoRect.width - larguraTextoBotao) / 2;
    float textoYBotao = botaoRect.y + (botaoRect.height - tamanhoTextoBotao) / 2;
    
    DrawText(textoBotao, (int)textoXBotao, (int)textoYBotao, tamanhoTextoBotao, corTextoBotao);
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

bool verificaCliqueBotaoGameOver(void) {
    // Verificar se o botão esquerdo do mouse foi pressionado
    if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return false;
    }
    
    Vector2 mousePosicao = GetMousePosition();
    
    // Dimensões do botão voltar na tela de Game Over
    float larguraBotao = 250.0f;
    float alturaBotao = 60.0f;
    float centroX = SCREEN_WIDTH / 2.0f - larguraBotao / 2.0f;
    float posY = SCREEN_HEIGHT / 2 + 120; // Dentro do retângulo de madeira
    
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
