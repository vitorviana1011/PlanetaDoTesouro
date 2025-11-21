#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "includes/manipulaArquivos.h"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define RESET "\x1b[0m"

bool arquivoExiste(const char *caminho) {
    return access(caminho, F_OK) == 0;
}

int tamanhoMapa(FILE *arq, int *linhas, int *colunas, int *totalTesouros){
    if(fscanf(arq, "%d %d %d", linhas, colunas, totalTesouros) != 3){
        printf(RED"Erro ao ler dimensões e quantidade de tesouros do mapa.\n");
        return 0;
    }
    return 1;
}

void mostrarMapa(char **mapa, int linhas){
    for (int i = 0; i < linhas; i++) {
        printf("%s\n", mapa[i]);
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
    mapa->totalTesouros = 0;

    printf(GREEN "Memória do mapa liberada com sucesso.\n" RESET);
}

Mapa carregaMapa(int fase){
    Mapa mapa = {NULL, 0, 0, 0};
    char caminho[50];
    snprintf(caminho, sizeof(caminho), "mapas/mapa%d.txt", fase);

    // Verifica se o arquivo existe antes de tentar abrir
    if (arquivoExiste(caminho)) {
        printf(GREEN "Arquivo '%s' encontrado.\n Carregando mapa...\n" RESET, caminho);
    } else {
        printf(RED "Acabou as fases ou o arquivo '%s' não foi encontrado.\n" RESET, caminho);
        return (Mapa){NULL, 0, 0, 0};
    }

    FILE *arq = fopen(caminho, "r");
    if (arq == NULL) {
        printf(RED "Erro ao abrir o arquivo '%s'.\n" RESET, caminho);
        printf("O arquivo existe mas não pode ser lido.\n");
        return mapa;
    }

    // Captura as dimensões e quantidade de tesouros da primeira linha
    if(tamanhoMapa(arq, &mapa.linhas, &mapa.colunas, &mapa.totalTesouros)){
        printf("Mapa carregado: %d linhas x %d colunas, %d tesouros\n", mapa.linhas, mapa.colunas, mapa.totalTesouros);
        
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
    
    fclose(arq);

    return mapa;
}

void listarMapasDisponiveis(void) {
    printf("Verificando mapas disponíveis:\n");
    
    for (int i = 1; i <= 5; i++) {
        char caminho[50];
        snprintf(caminho, sizeof(caminho), "mapas/mapa%d.txt", i);
        
        if (arquivoExiste(caminho)) {
            printf(GREEN "✓ %s - Disponível\n" RESET, caminho);
        } else {
            printf("✗ %s - Não encontrado\n", caminho);
        }
    }
    printf("\n");
}