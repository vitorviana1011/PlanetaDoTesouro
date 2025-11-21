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

void contarElementosMapa(Mapa mapa, int *totalTesouros, int *totalInimigos, Inimigo **inimigos) {
    *totalTesouros = 0;
    *totalInimigos = 0;

    for (int i = 0; i < mapa.linhas; i++) {
        for (int j = 0; j < mapa.colunas; j++) {
            if (mapa.dados[i][j] == 'T') {
                (*totalTesouros)++;
            } else if (mapa.dados[i][j] == 'I') {
                (*totalInimigos)++;
                *inimigos = realloc(*inimigos, (*totalInimigos) * sizeof(Inimigo));
                (*inimigos)[(*totalInimigos) - 1].x = j;
                (*inimigos)[(*totalInimigos) - 1].y = i;
                (*inimigos)[(*totalInimigos) - 1].velocidade = 0.5;
                (*inimigos)[(*totalInimigos) - 1].ultimoMovimento = 0.0;
            }
        }
    }
}

void liberaMapa(Mapa *mapa){
    if(mapa == NULL || mapa->dados == NULL) return;
    
    for(int i = 0; i < mapa->linhas; i++){
        free(mapa->dados[i]);
    }
    free(mapa->dados);
    mapa->dados = NULL;
    mapa->linhas = 0;
    mapa->colunas = 0;

    printf(GREEN_TEXT "Memória do mapa liberada com sucesso.\n" RESET);
}

Mapa carregaMapa(int fase, Inimigo **inimigos) {
    Mapa mapa = {NULL, 0, 0, 0, 0};
    char caminho[50];
    snprintf(caminho, sizeof(caminho), "mapas/mapa%d.txt", fase);

    // Verifica se o arquivo existe antes de tentar abrir
    if (arquivoExiste(caminho)) {
        printf(GREEN_TEXT "Arquivo '%s' encontrado.\n Carregando mapa...\n" RESET, caminho);
    } else {
        printf(RED_TEXT "Acabou as fases ou o arquivo '%s' não foi encontrado.\n" RESET, caminho);
        return (Mapa){NULL, 0, 0, 0, 0};
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
            return (Mapa){NULL, 0, 0, 0, 0};
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

    contarElementosMapa(mapa, &mapa.totalTesouros, &mapa.totalInimigos, inimigos);
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