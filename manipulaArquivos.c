#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "includes/inimigo.h"
#include "includes/manipulaArquivos.h"

#define RED_TEXT "\x1b[31m"
#define GREEN_TEXT "\x1b[32m"
#define RESET "\x1b[0m"

bool arquivoExiste(const char *caminho) {
    return access(caminho, F_OK) == 0;
}

int tamanhoMapa(FILE *arq, int *linhas, int *colunas){
    if(fscanf(arq, "%d %d", linhas, colunas) != 2){
        printf(RED_TEXT"Erro ao ler dimensões do mapa.\n"RESET);
        return 0;
    }
    return 1;
}

void mostrarMapa(char **mapa, int linhas){
    for (int i = 0; i < linhas; i++) {
        printf("%s\n", mapa[i]);
    }
}

void processarPortais(Mapa *mapa) {
    mapa->totalPortais = 0;
    
    // Array temporário para armazenar posições de cada portal
    struct {
        int x[2], y[2]; // Máximo 2 posições por número
        int count;
    } temp[10] = {0}; // Para portais '0' a '9'
    
    // Primeira passada: coletar todas as posições dos portais
    for (int i = 0; i < mapa->linhas; i++) {
        for (int j = 0; j < mapa->colunas; j++) {
            char tile = mapa->dados[i][j];
            if (tile >= '1' && tile <= '9') {
                int idx = tile - '0';
                if (temp[idx].count < 2) {
                    temp[idx].x[temp[idx].count] = j;
                    temp[idx].y[temp[idx].count] = i;
                    temp[idx].count++;
                }
            }
        }
    }
    
    // Segunda passada: criar pares válidos (que têm exatamente 2 portais)
    for (int i = 1; i <= 9; i++) {
        if (temp[i].count == 2 && mapa->totalPortais < 9) {
            mapa->portais[mapa->totalPortais].x1 = temp[i].x[0];
            mapa->portais[mapa->totalPortais].y1 = temp[i].y[0];
            mapa->portais[mapa->totalPortais].x2 = temp[i].x[1];
            mapa->portais[mapa->totalPortais].y2 = temp[i].y[1];
            mapa->portais[mapa->totalPortais].numero = '0' + i;
            mapa->totalPortais++;
        }
    }
    
    printf("Portais encontrados: %d pares\n", mapa->totalPortais);
}

void contarElementosMapa(Mapa *mapa, int *totalTesouros, int *totalInimigos, Inimigo **inimigos) {
    *totalTesouros = 0;
    *totalInimigos = 0;
    
    // Garantir que inimigos seja NULL se não houver inimigos
    if (*inimigos != NULL) {
        free(*inimigos);
        *inimigos = NULL;
    }

    for (int i = 0; i < mapa->linhas; i++) {
        for (int j = 0; j < mapa->colunas; j++) {
            if (mapa->dados[i][j] == 'T') {
                (*totalTesouros)++;
            } else if (mapa->dados[i][j] == 'I') {
                (*totalInimigos)++;
                *inimigos = realloc(*inimigos, (*totalInimigos) * sizeof(Inimigo));
                if (*inimigos == NULL) {
                    printf("ERRO: Falha na alocação de memória para inimigos!\n");
                    *totalInimigos = 0;
                    return;
                }
                (*inimigos)[(*totalInimigos) - 1].x = j;
                (*inimigos)[(*totalInimigos) - 1].y = i;
                (*inimigos)[(*totalInimigos) - 1].velocidade = 0.5;
                (*inimigos)[(*totalInimigos) - 1].ultimoMovimento = 0.0;
            }
        }
    }
    
    // Processar portais após contar outros elementos
    processarPortais(mapa);
}

void liberaMapa(Mapa *mapa){
    if(mapa == NULL) {
        printf("AVISO: Tentativa de liberar mapa nulo\n");
        return;
    }
    
    if(mapa->dados == NULL) {
        printf("AVISO: Dados do mapa já são nulos\n");
        return;
    }
    
    printf("Liberando mapa %dx%d...\n", mapa->linhas, mapa->colunas);
    
    for(int i = 0; i < mapa->linhas; i++){
        if(mapa->dados[i] != NULL){
            free(mapa->dados[i]);
            mapa->dados[i] = NULL;
        }
    }
    free(mapa->dados);
    mapa->dados = NULL;
    mapa->linhas = 0;
    mapa->colunas = 0;
    mapa->totalTesouros = 0;
    mapa->totalInimigos = 0;
    mapa->totalPortais = 0;
    
    // Limpar array de portais
    for(int i = 0; i < 9; i++){
        mapa->portais[i].x1 = 0;
        mapa->portais[i].y1 = 0;
        mapa->portais[i].x2 = 0;
        mapa->portais[i].y2 = 0;
        mapa->portais[i].numero = '0';
    }

    printf(GREEN_TEXT "Memória do mapa liberada com sucesso.\n" RESET);
}

Mapa carregaMapa(int fase, Inimigo **inimigos) {
    Mapa mapa = {0}; // Inicialização completa com zeros
    char caminho[50];
    snprintf(caminho, sizeof(caminho), "mapas/mapa%d.txt", fase);

    // Verifica se o arquivo existe antes de tentar abrir
    if (arquivoExiste(caminho)) {
        printf(GREEN_TEXT "Arquivo '%s' encontrado.\n Carregando mapa...\n" RESET, caminho);
    } else {
        printf(RED_TEXT "Acabou as fases ou o arquivo '%s' não foi encontrado.\n" RESET, caminho);
        Mapa mapaVazio = {0};
        return mapaVazio;
    }

    FILE *arq = fopen(caminho, "r");
    if (arq == NULL) {
        printf(RED_TEXT "Erro ao abrir o arquivo '%s'.\n" RESET, caminho);
        printf("O arquivo existe mas não pode ser lido.\n");
        return mapa;
    }

    // Captura as dimensões da primeira linha
    if(tamanhoMapa(arq, &mapa.linhas, &mapa.colunas)){
        printf("Mapa carregado: %d linhas x %d colunas\n", mapa.linhas, mapa.colunas);
        
        // Validar tamanho máximo permitido
        if(mapa.linhas > MAX_LINHAS || mapa.colunas > MAX_COLUNAS){
            printf(RED_TEXT "ERRO: Mapa muito grande! Máximo permitido: %dx%d\n" RESET, MAX_LINHAS, MAX_COLUNAS);
            printf("Tamanho atual: %dx%d\n", mapa.linhas, mapa.colunas);
            fclose(arq);
            Mapa mapaVazio = {0};
            return mapaVazio;
        }
        
        mapa.dados = malloc(mapa.linhas * sizeof(char*));
        for(int i = 0; i < mapa.linhas; i++){
            mapa.dados[i] = malloc((mapa.colunas + 1) * sizeof(char));
        }

        // Lê o mapa linha por linha
        for(int i = 0; i < mapa.linhas; i++){
            fscanf(arq, "%s", mapa.dados[i]);
        }

        mostrarMapa(mapa.dados, mapa.linhas);
    }

    contarElementosMapa(&mapa, &mapa.totalTesouros, &mapa.totalInimigos, inimigos);
    printf("Total de Tesouros: %d\n", mapa.totalTesouros);
    printf("Total de Inimigos: %d\n", mapa.totalInimigos);
    
    fclose(arq);

    return mapa;
}

void listarMapasDisponiveis(void) {
    printf("Verificando mapas disponíveis:\n");
    
    for (int i = 1; i <= 5; i++) {
        char caminho[50];
        snprintf(caminho, sizeof(caminho), "mapas/mapa%d.txt", i);
        
        if (arquivoExiste(caminho)) {
            printf(GREEN_TEXT "✓ %s - Disponível\n" RESET, caminho);
        } else {
            printf("✗ %s - Não encontrado\n", caminho);
        }
    }
    printf("\n");
}